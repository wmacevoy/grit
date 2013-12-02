#include <csignal>
#include <signal.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <memory>
#include <thread>
#include <mutex>
#include <map>
#include <list>
#include <assert.h>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <memory>
#include <string.h>
#include <mutex>

#include "BodyGlobals.h"

#include "CreateZMQServoController.h"
#include "ZMQHub.h"
#include "CSVRead.h"
#include "BodyMessage.h"
#include "Lock.h"
#include "now.h"
#include "BodyMover.h"
#include "glovestruct.h"
#include "HandTape.h"
#include "Vec3d.h"
#include "Arc.h"
#include "Mat3d.h"

#define USE_PY 0

#if USE_PY != 0
#include "Script.h"
#endif

#include "StdCapture.h"
#include "LeapMessage.h"
#include "joystick.h"
#include "ik_leftarm.h"
#include "ik_rightarm.h"

#if USE_PY != 0
SPScript py;
#endif

using namespace std;

class LeapRx : public ZMQHub
{
public:
  LeapMessage message;
  float origin[3],scale[3];

  LeapRx() {
    subscribers.push_back(cfg->str("leap.provider.subscribe"));
    origin[0]=cfg->num("leap.origin.x");
    origin[1]=cfg->num("leap.origin.y");
    origin[2]=cfg->num("leap.origin.z");
    scale[0]=cfg->num("leap.scale.x");
    scale[1]=cfg->num("leap.scale.y");
    scale[2]=cfg->num("leap.scale.z");
  }

  void start()
  {
    mover->left.leapReset();
    mover->right.leapReset();
    ZMQHub::start();
  }

  bool rx(ZMQSubscribeSocket &socket) { 
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    memcpy(&message,msg.data(),sizeof(LeapMessage));
    for (int i=0; i<3; ++i) {
      message.left.at[i] = origin[i]+scale[i]*message.left.at[i];
      message.right.at[i] = origin[i]+scale[i]*message.right.at[i];
    }
    mover->left.leapAdjust(message.left);
    mover->right.leapAdjust(message.right);
    if (verbose) {
      cout << "leap adjusted" << endl;
    }
    return true;
  }

  bool tx(ZMQPublishSocket &socket) { return true; }
};

class SensorsRx : public ZMQHub
{
public:
  SensorsRx() {
    subscribers.push_back(cfg->str("sensors.subscribe"));
  }

  bool rx(ZMQSubscribeSocket &socket) {
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    memcpy(&sensors,msg.data(),sizeof(SensorsMessage));
    return true;
  }

  bool tx(ZMQPublishSocket &socket) { return true; }
};

class BodyController : public ZMQHub
{
public:
  std::map < std::string , Vec3d > vecs;
  std::map < std::string , Arc > arcs;

  list < string > replies;
  string last;
  map < string , string > saved;
  mutex repliesMutex;
  static int lab;
  //The following variables are used specifically for the ramp walk
  //Amin/max is the accelerometer value and the zmin/max is the change
  //in elevation on the ramp platforms
  float Amin;
  float Amax;
  float zmin;
  float zmax;
  int zoffset;

  void includeSavedCommands(string file="commands")
  {
    vector < vector < string > > csvSaved;
    if (CSVRead(file,"name,do",csvSaved)) {
      for (size_t i=0; i<csvSaved.size(); ++i) {
	saved[csvSaved[i][0]]=csvSaved[i][1];
      }
    }
  }

  void saveSavedCommands(string file="commands")
  {
    ofstream save(file);
    save << "name,do" << endl;
    for (map<string,string>::iterator i = saved.begin(); i != saved.end(); ++i) {
      save << i->first << ",\"" << i->second << "\"" << endl;
    }
  }

  void answer(const string &reply)
  {
    Lock lock(repliesMutex);
    replies.push_back(reply);
  }

  void answer(ostringstream &oss)
  {
    answer(oss.str());
  }

  LeapRx leapRx;

  SensorsRx sensorsRx;

  std::thread* neckThread;
  std::atomic < bool > neck_on;
  void subscribeToNeck() {	//Neck thread function
	float currentUpDown = 0;
	float currentLeftRight = 0;
	int rc;
	double t, t1 = 0.0, t2 = 0.0, timeOut = 2.0, timeOut2 = 0.5;
	int hwm = 1;
	int linger = 25;
	bool active = true;
	joystick jm;
	std::string address = cfg->str("body.commander.neckAddress");

	void* context = zmq_ctx_new();
	void* sub = zmq_socket(context, ZMQ_SUB);
	rc = zmq_setsockopt(sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	zmq_setsockopt(sub, ZMQ_LINGER, &linger, sizeof(linger));
	zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);

	if (zmq_connect(sub, address.c_str()) != 0) {
		printf("Error initializing 0mq NECK...\n");
		return;
	}
	
	t = now();
	while(neck_on.load()) {
		jm.clear();
		rc = zmq_recv(sub, &jm, sizeof(joystick), 0);
		if(rc == sizeof(joystick)) {
			t = now();
		}
		
		if(jm.button2) {
			t1 = now();
			if(t1 - t2 > timeOut2) {
				active = !active;
				t2 = now();
			}		
		}

		if(active) {
			currentUpDown = jm.y2;
			currentLeftRight = jm.x2;
			if(currentUpDown < -65) currentUpDown = -65;
			else if(currentUpDown > 65) currentUpDown = 65;
			if(currentLeftRight < -175) currentLeftRight = -175;
			else if(currentLeftRight > 175) currentLeftRight = 175;
			mover->neck.upDown.setup(currentUpDown);
			mover->neck.leftRight.setup(currentLeftRight);
		}

		if(now() - t > timeOut) {
			zmq_close(sub);
			sub = zmq_socket(context, ZMQ_SUB);
			zmq_setsockopt(sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
			zmq_setsockopt(sub, ZMQ_LINGER, &linger, sizeof(linger));
			zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
			if (zmq_connect(sub, address.c_str()) == 0) {
				t = now();
			}			
		}
		std::this_thread::sleep_for(std::chrono::microseconds(25));
	}  

	zmq_close(sub);
	zmq_ctx_destroy(context);
  }
  
  std::thread* handsThread;
  std::atomic < bool > hands_on;
  void subscribeToHands() {	//Hands thread function
	int hwm = 1;
	double timeOut = 2;
	int linger = 25;
	Hands manos;
	std::string address = cfg->str("body.commander.glovesAddress");
	
	void *context = zmq_ctx_new ();
	void *sub = zmq_socket(context, ZMQ_SUB);
	zmq_setsockopt(sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	zmq_setsockopt(sub, ZMQ_LINGER, &linger, sizeof(linger));
	zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
	
	if (zmq_connect(sub, address.c_str()) != 0) {
		printf("Error initializing 0mq HANDS...\n");
		return;
	}
	
	double t = now();	
	while(hands_on.load()) {
		manos.clear();
		int rc = zmq_recv(sub, &manos, sizeof(Hands), 0);
		if(rc == sizeof(Hands)) {
			t = now();
		}	

		mover->left.trigger.setup(manos.ltrigger);
		mover->left.middle.setup(manos.lmiddle);
		mover->left.ring.setup(manos.lring);
		mover->left.thumb.setup(manos.lthumb);
		mover->right.trigger.setup(manos.rtrigger);
		mover->right.middle.setup(manos.rmiddle);
		mover->right.ring.setup(manos.rring);
		mover->right.thumb.setup(manos.rthumb);

		if(now() - t > timeOut) {
			zmq_close(sub);
			sub = zmq_socket(context, ZMQ_SUB);
			zmq_setsockopt(sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
			zmq_setsockopt(sub, ZMQ_LINGER, &linger, sizeof(linger));
			zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
			if (zmq_connect(sub, address.c_str()) == 0) {
				t = now();
			}			
		}
		std::this_thread::sleep_for(std::chrono::microseconds(25));
	}		
	zmq_close(sub);
	zmq_ctx_destroy(context);
  }

  void setLIO(float angle) {
	mover->left.inOut.setup(angle);
//	mover->left.inOut.torque=0.75;
  }
  void setLUD(float angle) {
	mover->left.upDown.setup(angle);
//	mover->left.upDown.torque=0.75;
  }
  void setLBicep(float angle) {
	mover->left.bicep.setup(angle);
//	mover->left.bicep.torque=0.75;
  }
  void setLElbow(float angle) {
	mover->left.elbow.setup(angle);
//	mover->left.elbow.torque=0.75;
  }
  void setLForearm(float angle) {
	mover->left.forearm.setup(angle);
//	mover->left.elbow.torque=0.75;
  }
  void setRIO(float angle) {
	mover->right.inOut.setup(angle);
//	mover->right.inOut.torque=0.75;
  }
  void setRUD(float angle) {
	mover->right.upDown.setup(angle);
//	mover->right.upDown.torque=0.75;
  }
  void setRBicep(float angle) {
	mover->right.bicep.setup(angle);
//	mover->right.bicep.torque=0.75;
  }
  void setRElbow(float angle) {
	mover->right.elbow.setup(angle);
//	mover->right.elbow.torque=0.75;
  }
  void setRForearm(float angle) {
	mover->right.forearm.setup(angle);
//	mover->right.elbow.torque=0.75;
  }
  void goDHome() {
    load("doorhome.csv");
    setWaist(0);
    setLIO(-40);
    setLUD(-40);
    setLElbow(40);
    setLBicep(-20);
    setRIO(-40);
    setRUD(-40);
    setRElbow(40);
    setRBicep(-20); 
  }  
  void goHome() {
    load("home.csv");
    setWaist(0);
    setLIO(-40);
    setLUD(-40);
    setLElbow(40);
    setLBicep(-20);
    setRIO(-40);
    setRUD(-40);
    setRElbow(40);
    setRBicep(-20);    
  }
  
  void setPitch(float angle) {
    mover->neck.upDown.setup(angle);
    //    mover->neck.upDown.torque=0.5;
  }
  
  void setYaw(float angle) {
    mover->neck.leftRight.setup(angle);
    //    mover->neck.leftRight.torque=0.5;
  }

  void setWaist(float angle) {
    mover->waist.setup(angle);
  }
  
  bool load(const string &file)
  {
    return mover->load(file);
  }

  bool play(const string &file)
  {
    return mover->play(file);
  }

  void forward(int repeat=1)
  {
      mover->stepMove(4.0,15.0,15.0,-17.115+0.00,0.0,5.0,5.75+3.0,1.0,.72,false,repeat);
  }

  void forwardFast(int repeat=1)
  {
      mover->stepMove(5.0,15.0,15.0,-17.115+0.00,0.0,4.0,5.75+3.0,1.0,.72,false,repeat);
  }
  
  void smallForward()
  {
      mover->stepMove(4.0,15.0,15.0,-17.115+0.00,0.0,2.0,5.75+2.0,1.0,.72);
  }

  void lowForward()
  {
      mover->blended(4.0,15.0,15.0,-17.115+5.75,0.0,5.0,5.75+3.0,1.0,.72);
  }
  
  void balancedRampUp()
  {
      zoffset = ( ( (sensors.a[0] - Amin) * (Amin - Amax) ) / (zmin - zmax) ) + 8;
      WalkParameters wp(2.5,15.9,15.9,-15.,2.0,90.0,1.0);
      wp.repeat=4;
      wp.zOffset=zoffset;
      mover->bStep(wp);
  }
  
  void balancedRampDown()
  {
      zoffset = ( ( (sensors.a[0] - Amin) * (Amin - Amax) ) / (zmin - zmax) ) + 8;
      WalkParameters wp(2.5,15.9,15.9,-15.,2.0,90.0,1.0);
      wp.repeat=4;
      wp.zOffset=zoffset;
      mover->bStep(wp);
  }
  
  void stepIn() {
	double step=4.0;
    WalkParameters wp1(2.5,17.25,12.0,-15.,step,90.0,2.0);
    wp1.y3-=step;  // move back legs back 
    wp1.y4-=step;    
    WalkParameters wp2(2.5,12.0,17.25,-15.,step,90.0,2.0);
    wp2.y3-=step;  // move back legs back 
    wp2.y4-=step;
    mover->stepMerge(wp1,wp2);
  }
  
  void stepOut() {
	double step=4.0;
    WalkParameters wp1(2.5,12.0,17.25,-15.,step,90.0,2.0);
    wp1.y3-=step;  // move back legs back 
    wp1.y4-=step;    
    WalkParameters wp2(2.5,17.25,12.00,-15.,step,90.0,2.0);
    wp2.y3-=step;  // move back legs back 
    wp2.y4-=step;
    mover->stepMerge(wp1,wp2);
  }
  
  void doRaise(double step) {
    WalkParameters wp1(2.5,12.0,17.25,-15.,step,90.0,2.0);
    wp1.y3-=step;  // move back legs back 
    wp1.y4-=step;
    mover->changeZ(wp1,-20.0);    
  }
  
  void doSqwat(double step) {
    WalkParameters wp1(2.5,12.0,17.25,-20.,step,90.0,2.0);
    wp1.y3-=step;  // move back legs back 
    wp1.y4-=step;
    mover->changeZ(wp1,-15.0);
  }
  
  void balancedForward(int repeat=4)
  {
	double step=4.0;
    WalkParameters wp(2.5,12.0,17.25,-15.,step,90.0,2.0);
    wp.y3-=step;  // move back legs back 
    wp.y4-=step;    
    wp.repeat=repeat;
    mover->bStep(wp);
  }
  void balancedStrafeRight()
  {
	  double step=2.0;
      WalkParameters wp(2.5,12.0,17.25,-15.,step,0.0,1.0);    
      wp.y3-=step;  // move back legs back 
      wp.y4-=step;    
      mover->bStep(wp);
  }
  void balancedStrafeLeft()
  {
	  double step=2.0;
      WalkParameters wp(2.5,12.0,17.25,-15.,step,180.0,1.0);
      wp.y3-=step;  // move back legs back 
      wp.y4-=step;    
      mover->bStep(wp);
  }
  
  void balancedBackward(int repeat=4)
  {
	  double step=2.0;
      WalkParameters wp(2.5,12.0,17.25,-15.,step,270.0,1.0);
      wp.y3-=step;  // move back legs back 
      wp.y4-=step;    
      wp.repeat=repeat;
      mover->bStep(wp);
  }
  
  void balancedLeft()
  {
	  double step=4.0;
      WalkParameters wp(2.5,12.0,17.25,-15.,step,90.0-45.0,1.0);
      wp.y3-=step;  // move back legs back 
      wp.y4-=step;    
      wp.repeat=4;
      wp.rotation=-1.0;
      mover->bStep(wp);
  }
  
  void balancedRight()
  {
	  double step=4.0;
      WalkParameters wp(2.5,12.0,17.25,-15.,step,90.0-45.0,1.0);
      wp.y3-=step;  // move back legs back 
      wp.y4-=step;    
      wp.repeat=4;
      wp.rotation=1.0;
      mover->bStep(wp);
  }
  
  void balancedHighForward()
  {
	  double step=0.5;
	  WalkParameters wp(2.5,12.5,14.5,-20.00,step,90.0,12.0);
	  wp.repeat=4;
      mover->bStep(wp);
  }
  
  void balancedLowForward()
  {
	  double step=0.5;
	  WalkParameters wp(2.5,12.5,14.5,-10.00,step,90.0,0.0);
	  wp.repeat=4;
      mover->bStep(wp);
  }

  void tape(const std::string &tape)
  {
    for (int i=0; i<4; ++i) mover->legs.legMovers[i]->tape(tape);
  }

  bool setupBricks()
  {
    float saveSimSpeed=simSpeed;
    simSpeed=0;
    simTime=0;
    lastSimTime=0;
    tape("lf");
    lowForward();
    tape("sf");
    smallForward();
    tape("f");
    forward();
    tape("new");
    goHome();
    simSpeed=saveSimSpeed;
    return true;
  }

  void positionHand() {
    //Lift arm and place over handle
    setLElbow(-40);
    setRElbow(-40);    
    sleep(8);
    setLUD(35);
    setLBicep(0);
    setLIO(30);
    setLForearm(42);
    setRUD(35);
    setRBicep(0);
    setRIO(30);
    setRForearm(42);
    sleep(14);
    setLUD(10);
    setLElbow(-10);
    setRUD(10);
    setRElbow(-10);
  }
  
void leapHand() {
    //Lift arm and place over handle
    setLElbow(-40);
    setRElbow(-40);    
    sleep(8);
    setLUD(35);
    setLBicep(0);
    setLIO(23);
    setLForearm(42);
    setRUD(35);
    setRBicep(0);
    setRIO(23);
    setRForearm(42);
    sleep(14);
    setLUD(10);
    setLElbow(-10);
    setRUD(10);
    setRElbow(-10);
  }


  void handDownPush() {
    //Push handle down
    setLUD(3);
    setLElbow(0);
    sleep(5);
    //Push door open
    setLElbow(15);
    sleep(1);
    setLUD(20);
    sleep(5);
  }
  
  void handDownPull() {
    //Push handle down
    setLUD(3);
    setLElbow(-3);
    sleep(5);
    //Pull door open
    setLUD(-10);
    setLElbow(-20);
    sleep(5);
  }

  void retractArm() {
    //Position arm in to be ready to move from door
    setLUD(30);
    setLElbow(-35);
    setRUD(30);
    setRElbow(-35);
    sleep(4);
    //Put arm back near home
    setLIO(-40);
    setRIO(-40);
    sleep(6);
    setLUD(-40);
    setLElbow(0);
    setLForearm(0);
    setRUD(-40);
    setRElbow(0);
    setRForearm(0);
  }
  
  void shake() {
/*  Double check signs before we turn this back on
	setRBicep(-30);  
	sleep(1);
	setRElbow(44);
	sleep(1);
	setRIO(-40);
	setRUD(0);
	sleep(2);
	for (int i=0;i<3;i++) {
	  setRElbow(30);
	  sleep(1);
	  setRElbow(44);
	  sleep(1);
	}
	setRIO(44);
	setRBicep(0);
    setRElbow(10);
	setRUD(30); */
  }
  void yes()
  {
     setPitch(20);
     sleep(1);
     setPitch(-20);
     sleep(1);
     setPitch(20);
     sleep(1);
     setPitch(-20);
     sleep(1);
     setPitch(0);
  }
  void no()
  {
     setYaw(20);
     sleep(1);
     setYaw(-20);
     sleep(1);
     setYaw(20);
     sleep(1);
     setYaw(-20);
     sleep(1);
     setYaw(0);
  } 

  void straight() {
	setPitch(0);
	setYaw(0);
  }
  void sad() {
	setPitch(45);
	setWaist(10);
  }

  void sensorsOn()
  {
    sensorsRx.start();
  }

  void sensorsOff()
  {
    sensorsRx.stop();
    sensorsRx.join();
  }

  void neckOn()
  {
     if(neckThread == 0) {
       neck_on.store(true);
       neckThread = new std::thread(&BodyController::subscribeToNeck, this);
     }
  }

  void neckOff()
  {
     if (neckThread != 0) {
       neck_on.store(false);
       neckThread->join();
       delete neckThread;
       neckThread = 0;
     }
  }
  
  void handsOn()
  {
     if (handsThread == 0) {
       hands_on.store(true);
       handsThread = new std::thread(&BodyController::subscribeToHands, this);
     }
  }


  void handsOff()
  {
     if (handsThread != 0) {
       hands_on.store(false);
       handsThread->join();
       delete handsThread;
       handsThread = 0;
     }
  }

  void leapOn()
  {
    leapRx.start();
  }

   void leapOff()
  {
    leapRx.stop();
    leapRx.join();
  }


  void enable(string part, bool value)
  {
    float torque = (value) ? 0.75 : 0.0;
    bool ok = false;
    if (part == "left") {
      mover->left.torque(torque);
      ok=true;
    }
    if (part == "right") {
      mover->right.torque(torque);
      ok=true;
    }
    if (part == "leg1"){
      mover->legs.torque(torque,0);
      ok = true;
    }
    if (part == "leg2"){
      mover->legs.torque(torque,1);
      ok = true;
    }
    if (part == "leg3"){
      mover->legs.torque(torque,2);
      ok = true;
    }
    if (part == "leg4"){
      mover->legs.torque(torque,3);
      ok = true;
    }

    ostringstream oss;
    if (ok) {
      oss << "my " << part << " is " << (value ? "enabled" : "disabled");
      answer(oss);
    } else {
      oss << "what is " << part << "?";
      answer(oss);
    }
  }

  void act(string command)
  {
    if (command == "last") {
      command=last;
    } else {
      last=command;
    }

    { // just for "do" command...
      istringstream iss(command);
      ostringstream oss;

      string head;
      iss >> head;

      if (head == "do") {
	string name;
	iss >> name;

	map<string,string>::iterator i = saved.find(name);
	if (i != saved.end()) {
	  command = i->second;
	  cout << "doing "  << command << endl;
	} else {
	  oss << "don't know how to do " << name;
	  answer(oss.str());
	}
	return;
      }
    }

    istringstream iss(command);
    ostringstream oss;

    string head;
    iss >> head;



    if (head == "save") {
      string name;
      iss >> name;
      string instruction;
      string part;
      while (iss >> part) {
	if (instruction != "") {
	  instruction.append(" ");
	}
	instruction.append(part);
      }
      saved[name]=instruction;
      oss << "named '" << instruction << "' as " << name;
      answer(oss.str());
    }

    if (head == "vec") {
      string name;
      char eq;
      iss >> name;
      iss >> eq;
      iss >> vecs[name];
      oss << "vec " << name << "=" << vecs[name];
      answer(oss.str());
    }
    
    if (head == "lik") {
      LeftArmGeometry geometry;
      map<string,int> parameterMap;
      bool real=true;
      bool ok=true;

      geometry.forward();
      for (int i=0; i<ik_leftarmparameter_count; ++i) {
	parameterMap[ik_leftarmparameter_names[i]]=i;
      }

      string name;
      while (iss >> name) {
	if (name == "fake") { real=false; continue; }
	if (name == "real") { real=true;  continue; }
	if (name.rfind('=') != string::npos) {
	  size_t eq=name.rfind('=');
	  double value=atof(name.substr(eq+1).c_str());
	  name=name.substr(0,eq);
	  map<string,int>::iterator i = parameterMap.find(name);
	  if (i != parameterMap.end()) {
	    geometry.parameters.as_array[i->second]=value;
	  } else {
	    ok = false;
	    break;
	  }
	}
      }

      if (ok) {
	ok=geometry.inverse();
	if (ok && real) {
	  mover->left.inOut.setup(geometry.parameters.as_struct.shoulderio);
	  mover->left.upDown.setup(geometry.parameters.as_struct.shoulderud);
	  mover->left.bicep.setup(geometry.parameters.as_struct.bicep);
	  mover->left.elbow.setup(geometry.parameters.as_struct.elbow);
	  mover->left.forearm.setup(geometry.parameters.as_struct.forearm);
	}
      }
      
      if (!ok) {
	oss << "failed.";
      } else {
	if (!real) oss << "(fake)";
	for (map<string,int>::iterator i=parameterMap.begin(); i!=parameterMap.end(); ++i) {
	  oss << " " << i->first << "=" << geometry.parameters.as_array[i->second];
	}
      }
      answer(oss.str());
    }

#if USE_PY != 0
    if (head == "py") {
      StdCapture capture;
      capture.BeginCapture();
      if (command.length() > 2) py->run(command.substr(3));
      capture.EndCapture();
      answer(capture.GetCapture());
    }
#endif
    
    if (head == "enable") {
      string part;
      iss >> part;
      enable(part,true);
    }
    if (head == "disable") {
      string part;
      iss >> part;
      enable(part,false);
    }
    if (head == "sl") {
		lab -= 2;
		setLBicep(lab);
	}
	if (head == "sr") {
		lab += 2;
		setLBicep(lab);
	}
    if (head == "ph") {
	  lab = 0;
      positionHand();
      answer("Positioning hand...");
    }
    if (head == "lh") {
	  lab = 0;
      leapHand();
      answer("Leap hand...");
    }
    if (head == "hd1") {
      handDownPush();
      answer("Pushing handle down and push...");
    }
    if (head == "hd2") {
      handDownPull();
      answer("Pushing handle down and pull...");
    }
    if (head == "ra") {
	  lab = 0;
      retractArm();
      answer("Retracting arm...");
    }
    if (head == "HStep") {
      float radius=4,ystep=0,xstep=0,left=1.0,right=1.0;
      iss >> radius >> xstep >> ystep;
      if (fabs(radius)<6 && fabs(xstep)<6 && fabs(ystep)<6) {
        mover->stepMove(radius,13.9,13.9,-19.665,xstep,ystep,8,left,right);  // High Narrow Pose
        ostringstream oss;
        oss << "HStep r=" << radius << " xstep=" << xstep << " ystep=" << xstep << " :ok."; 
      } else {
		oss << "HStep r=" << radius << " xstep=" << xstep << " ystep=" << xstep << " :NOT ok"; 
	  }
      answer(oss.str());
    }
    if (head == "DStep") {
      float radius=4,ystep=0,xstep=0,left=1.0,right=1.0;
      iss >> radius >> xstep >> ystep >> left >> right;
      if (fabs(radius)<=6 && fabs(xstep)<=6 && fabs(ystep)<=6) {
        mover->stepMove(radius,12.0,17.25,-14.665,xstep,ystep,8,left,right);
        ostringstream oss;
        oss << "DStep r=" << radius << " xstep=" << xstep << " ystep=" << ystep << " :ok."; 
      } else {
		oss << "DStep r=" << radius << " xstep=" << xstep << " ystep=" << xstep << " :NOT ok"; 
	  }
      answer(oss.str());
    }
    if (head == "df") {
		//  Should be 2 inch step forward
        mover->stepMove(4,12.0,17.25,-14.665,0,4.0,6,1.0,1.0,true);
//        mover->stepMove(4,12,17.25,-15,0,4.0,6,1.0,1.0);
		oss << "DStep r=4 xstep=0 ystep=4 :ok"; 
      answer(oss.str());
    }
    if (head == "dsf") {
        mover->stepMove(4,12.0,17.25,-14.665,0,2.0,6,1.0,1.0,true);
//        mover->stepMove(4,12,17.25,-15,0,2.0,6,1.0,1.0);
		oss << "DStep r=2 xstep=0 ystep=4 :ok"; 
      answer(oss.str());
    }
    if (head == "dsb") {
        mover->stepMove(4,12,17.25,-15,0,-2.0,6,1.0,1.0,true);
		oss << "DStep r=2 xstep=0 ystep=4 :ok"; 
      answer(oss.str());
    }
    if (head == "dr") {
        mover->stepMove(4,12.0,17.25,-14.665,0,4,6,1.0,.1,true);
//        mover->stepMove(4,12,17.25,-15,0,2,6,1.0,-1.0);
		oss << "DStep r=4 xstep=0 ystep=4 :ok"; 
      answer(oss.str());
    }
    if (head == "dl") {
        mover->stepMove(4,12.0,17.25,-14.665,0,4,6,.1,1.0,true);
//        mover->stepMove(4,12,17.25,-15,0,2,6,-1.0,1.0);
		oss << "DStep r=4 xstep=0 ystep=4 :ok"; 
      answer(oss.str());
    }
    if (head == "Step") {
      float radius=4,ystep=0,xstep=0,left=1.0,right=1.0;
      float xpos=14.185,ypos=14.9;
      iss >> radius  >> xpos >> ypos>> xstep >> ystep;
      if (fabs(radius)<=6 && fabs(xstep)<=12 && fabs(ystep)<=12) {
        mover->stepMove(radius,xpos,ypos,-14.665,xstep,ystep,8,left,right);
        ostringstream oss;
        oss << "Step r=" << radius << " xstep=" << xstep << " ystep=" << ystep << " :ok."; 
      } else {
		oss << "Step r=" << radius << " xstep=" << xstep << " ystep=" << xstep << " :NOT ok"; 
	  }
      answer(oss.str());
    }
    if (head == "report") {
      ostringstream oss;
      body->report(oss);
      answer(oss);
    }
    if (head=="neck") {
      string value;
      iss >> value;
      if (value == "on") {
         neckOn();
	 answer("my neck is on.");
      } else if (value == "off") {
         neckOff();
	 answer("my neck is off.");
      }
    }
    if (head=="sensors") {
      string value;
      iss >> value;
      if (value == "on") {
	sensorsRx.start();
	answer("my sensors are on.");
      } else if (value == "off") {
	sensorsRx.stop();
	answer("my sensors are off.");
      }
    }
    if (head == "touch") {
      int legNumber;
      int touchPressure;
      iss >> legNumber >> touchPressure;
      if (1 <= legNumber && legNumber <=4 && 100 <= touchPressure && touchPressure <= 1024) {
      mover->legs.legMovers[legNumber-1]->touchPressure=touchPressure;
      oss << "set touch pressure to " << touchPressure << " on leg " << legNumber;
      answer(oss.str());
      }
    }
    if (head == "safe") {
      string flag;
      if (iss >> flag) {
	if (flag == "on") {
	  safety->safe(true);
	  answer("body safety is on");
	} else if (flag == "off") {
	  safety->safe(false);
	  answer("body safety is off");
	}
      } else {
	oss << "safe " << (safety->safe() ? "on" : "off")
	    << " and warn " << (safety->warn() ? "on" : "off");
	answer(oss.str());
      }
    }
    if (head == "at") {
      set<string> names = cfg->servoNames();
      bool any=false;
      string name;
      while (iss >> name) {
	any=true;
	if (name.rfind('=') != string::npos) {
	  size_t eq=name.rfind('=');
	  double value=atof(name.substr(eq+1).c_str());
	  name=name.substr(0,eq);
	  set<string>::iterator i = names.find(name);
	  if (i != names.end()) {
	    mover->getMover(name)->setup(value);
	    oss << " set " << name << "=" << value;
	  }
	} else {
	  set<string>::iterator i = names.find(name);
	  if (i != names.end()) {
	    oss << " " <<  name << "=" << mover->getMover(name)->angle();
	  }
	}
      }
      if (!any) {
	for (set<string>::iterator i = names.begin(); i != names.end(); ++i) {
	  oss << " " <<  *i << "=" << mover->getMover(*i)->angle();
	}
      }
      answer(oss.str());
    }

    if (head=="hands") {
      string value;
      iss >> value;
      if (value == "on") {
         handsOn();
	 answer("my hands are on.");
      } else if (value == "off") {
         handsOff();
	 answer("my hands are off.");
      }
    }

    if (head=="leap"){
      string value;
      iss >> value;
      if (value == "on"){
	leapOn();
	answer("leap controls arms.");
      } else if (value=="off") {
	leapOff();
	answer("leap no longer controls arms.");
      }
    }
    if (head=="shake") {
	  shake();
	  answer("Shake");
	}
    if (head == "yes") {
      yes();
      answer("Yes");
    }
    if (head == "no") {
      no();
      answer("No");
    }
    if (head=="straight") {
      straight();
    }
    if (head=="sad") {
      sad();
    }
    if (head == "home") {
	  goHome();
      ostringstream oss;
      oss << "played Home script and more"; 
      answer(oss.str());
    }
    if (head == "dhome") {
	  goDHome();
      ostringstream oss;
      oss << "played DoorHome script and more"; 
      answer(oss.str());
    }    if (head == "walk") {
	  load("Gait1_3.csv");
      ostringstream oss;
      oss << "played Gait1_3 script"; 
      answer(oss.str());
    }
    if (head== "backup") {
	  load("Gait3_0.csv");
      ostringstream oss;
      oss << "played Gait3_0m script"; 
      answer(oss.str());
	}
    if (head == "flu") {  // Forward Front Lift Up front for brick
      simSpeed = 0.0;
      simTime = 0.0;
	  lastSimTime = 0.0;
      mover->legs.legMovers[0]->tape("f");
      mover->legs.legMovers[1]->tape("f");
      mover->legs.legMovers[2]->tape("f");
      mover->legs.legMovers[3]->tape("f");
      mover->legs.legMovers[0]->state(LegMover::LEG_BRICKS);
      mover->legs.legMovers[1]->state(LegMover::LEG_BRICKS);
   	  mover->legs.legMovers[2]->state(LegMover::LEG_NORMAL);
   	  mover->legs.legMovers[3]->state(LegMover::LEG_NORMAL);
      simSpeed = 0.5;
      answer("Front stepping on brick");
    }
/*    if (head == "fob") {  // Forward Front On Brick
      load("fob.csv");
      answer("Front on brick");
    } */
    if (head == "fdd") {  // Forward Front Drop Down from brick
      simSpeed = 0.0;
      simTime = 0.0;
	  lastSimTime = 0.0;
      mover->legs.legMovers[0]->tape("f");
      mover->legs.legMovers[1]->tape("f");
      mover->legs.legMovers[2]->tape("f");
      mover->legs.legMovers[3]->tape("f");
   	  mover->legs.legMovers[0]->state(LegMover::LEG_NORMAL);
   	  mover->legs.legMovers[1]->state(LegMover::LEG_NORMAL);
   	  mover->legs.legMovers[2]->state(LegMover::LEG_NORMAL);
   	  mover->legs.legMovers[3]->state(LegMover::LEG_NORMAL);
      simSpeed = 0.5;
      answer("Front stepping off brick");
    }
    if (head == "blu") {  // Forward Back Lift Up front for brick
      simSpeed = 0.0;
      simTime = 0.0;
	  lastSimTime = 0.0;
      mover->legs.legMovers[0]->tape("f");
      mover->legs.legMovers[1]->tape("f");
      mover->legs.legMovers[2]->tape("f");
      mover->legs.legMovers[3]->tape("f");
   	  mover->legs.legMovers[0]->state(LegMover::LEG_NORMAL);
   	  mover->legs.legMovers[1]->state(LegMover::LEG_NORMAL);
      mover->legs.legMovers[2]->state(LegMover::LEG_BRICKS);
      mover->legs.legMovers[3]->state(LegMover::LEG_BRICKS);
      simSpeed = 0.5;
      answer("Back stepping on brick");
    }
/*    if (head == "bob") {  // Forward Back On Brick
      load("bob.csv");
      answer("Back on brick");
    } */
    if (head == "bdd") {  // Forward Back Drop Down from brick
      simSpeed = 0.0;
      simTime = 0.0;
	  lastSimTime = 0.0;
      mover->legs.legMovers[0]->tape("f");
      mover->legs.legMovers[1]->tape("f");
      mover->legs.legMovers[2]->tape("f");
      mover->legs.legMovers[3]->tape("f");
   	  mover->legs.legMovers[0]->state(LegMover::LEG_NORMAL);
   	  mover->legs.legMovers[1]->state(LegMover::LEG_NORMAL);
      mover->legs.legMovers[2]->state(LegMover::LEG_NORMAL);
      mover->legs.legMovers[3]->state(LegMover::LEG_NORMAL);
      simSpeed = 0.5;
      answer("Back stepping off brick");
    } 
    if (head == "setupBricks") {
      if (setupBricks()) {
	answer("setupBricks ok");
      }
    }
    if (head == "bricks") {
      bool any=false;
      int number;
      while (iss >> number) {
	if (1 <= number && number <= 4) {
	  mover->legs.legMovers[number-1]->state(LegMover::LEG_BRICKS);
	  if (!any) {
	    oss << "bricks on leg(s)";
	  }
	  oss << " " << mover->legs.legMovers[number-1]->number()+1;
	  any=true;
	}
      }
      if (!any) {
	mover->legs.state(LegMover::LEG_BRICKS);
	oss << "bricks on all legs.";
      }
      answer(oss.str());
    }
    if (head == "tape") {
      string part;
      while (iss >> part) {
	if (part.rfind('=') != string::npos) {
	  size_t eq = part.rfind('=');
	  string leg=part.substr(0,eq);
	  string tape=part.substr(eq+1);
	  if (leg == "leg1" || leg=="1") {
	    mover->legs.legMovers[LEG1]->tape(tape);
	    oss << " leg1=" << tape;
	  } else if (leg == "leg2" || leg=="2") {
	    mover->legs.legMovers[LEG2]->tape(tape);	    
	    oss << " leg2=" << tape;
	  } else if (leg == "leg3" || leg=="3") {
	    mover->legs.legMovers[LEG3]->tape(tape);
	    oss << " leg3=" << tape;
	  } else if (leg == "leg4" || leg=="4") {
	    mover->legs.legMovers[LEG4]->tape(tape);
	    oss << " leg4=" << tape;
	  } else if (leg == "legs") {
	    for (int i=0; i<4; ++i) {
	      oss << " leg" << (i+1) << "=" << tape;
	      mover->legs.legMovers[i]->tape(tape);
	    }
	  }
	} else {
	  string leg=part;
	  if (leg == "leg1" || leg=="1") {
	    oss << " leg1=" << mover->legs.legMovers[LEG1]->tape();
	  } else if (leg == "leg2" || leg=="2") {
	    oss << " leg2=" << mover->legs.legMovers[LEG2]->tape();
	  } else if (leg == "leg3" || leg=="3") {
	    oss << " leg3=" << mover->legs.legMovers[LEG3]->tape();
	  } else if (leg == "leg4" || leg=="4") {
	    oss << " leg4=" << mover->legs.legMovers[LEG4]->tape();
	  } else if (leg == "legs") {
	    for (int i=0; i<4; ++i) {
	      oss << " leg" << (i+1) << "=" 
		  << mover->legs.legMovers[i]->tape();
	    }
	  }
	}
      }
      answer(oss.str());
    }
    if (head == "cautious") {
      bool any=false;
      int number;
      while (iss >> number) {
	if (1 <= number && number <= 4) {
	  mover->legs.legMovers[number-1]->state(LegMover::LEG_CAUTIOUS);
	  if (!any) {
	    oss << "cautious on leg(s)";
	  }
	  oss << " " << number;
	  any=true;
	}
      }
      if (!any) {
	mover->legs.state(LegMover::LEG_CAUTIOUS);
	oss << "cautious on all legs.";
      }
      answer(oss.str());
    }
    if (head == "normal") {
      bool any=false;
      int number;
      while (iss >> number) {
	if (1 <= number && number <= 4) {
	  mover->legs.legMovers[number-1]->state(LegMover::LEG_NORMAL);
	  if (!any) {
	    oss << "normal on leg(s)";
	  }
	  oss << " " << number;
	  any=true;
	}
      }
      if (!any) {
	mover->legs.state(LegMover::LEG_NORMAL);
	oss << "normal on all legs.";
      }
      answer(oss.str());
    }
    if (head == "f") {  // forward
//      mover->stepMove(4.0,14.9,14.9,-15.665,0,3.,8.0,1.0,1.0);
//      mover->stepMove(5.0,15.0,12.0,-15.665,0.0,5.0,8.0,1.0,1.0);
      forward();
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=4 :ok."; 
      answer(oss.str());
    }
    if (head == "f4") {  // forward
//      mover->stepMove(4.0,14.9,14.9,-15.665,0,3.,8.0,1.0,1.0);
//      mover->stepMove(5.0,15.0,12.0,-15.665,0.0,5.0,8.0,1.0,1.0);
      forwardFast(4);
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=4 :ok."; 
      answer(oss.str());
    }
    if (head == "lf") {  // forward
//      mover->stepMove(4.0,14.9,14.9,-9.915,0,3.,5.0,1.0,1.0);
//      mover->stepMove(5.0,15.0,12.0,-9.915,0.0,5.0,5.0,1.0,1.0);
      lowForward();
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=4 :ok."; 
      answer(oss.str());
    }
    if (head=="raise") {
	  doRaise(4.0);
	  answer("Performing Raise");
	}
	if (head=="sqwat") {
	  doSqwat(4.0);
	  answer("Performing Sqwat");
	}
    if (head=="stepin") {
	  stepIn();
	  answer("Performing StepIn");
	}
	if (head=="stepout") {
	  stepOut();
	  answer("Performing StepOut");
	}
    if (head == "bf") {  // forward
      int repeat=4;
      iss >> repeat;
      balancedForward(repeat);
      ostringstream oss;
      oss << "Step Balanced r=2 zstep=12:ok."; 
      answer(oss.str());
    }
    if (head == "bb") {  // backward
      int repeat=4;
      iss >> repeat;
      balancedBackward(repeat);
      ostringstream oss;
      oss << "Step Balanced r=2 zstep=12:ok."; 
      answer(oss.str());
    }
    if (head == "brd") {  // ramp down
      balancedRampDown();
      answer("Balanced Ramp Down");
    }
    if (head == "bru") {  // ramp up
      balancedRampUp();
      answer("Balanced Ramp Up");
    }
    if (head == "bsr") {  // right
      balancedStrafeRight();
      ostringstream oss;
      oss << "Step strafe right"; 
      answer(oss.str());
    }
    if (head == "bsl") {  // right
      balancedStrafeLeft();
      ostringstream oss;
      oss << "Step strafe right"; 
      answer(oss.str());
    }
    if (head == "br") {  // right
      balancedRight();
      ostringstream oss;
      oss << "Step Balanced r=2 zstep=12:ok."; 
      answer(oss.str());
    }
    if (head == "bl") {  // left
      balancedLeft();
      ostringstream oss;
      oss << "Step Balanced r=2 zstep=12:ok."; 
      answer(oss.str());
    }
    if (head == "bhf") {  // forward
      balancedHighForward();
      ostringstream oss;
      oss << "Step Balanced r=2 zstep=12:ok."; 
      answer(oss.str());
    }
    if (head == "blf") {  // forward
      balancedLowForward();
      ostringstream oss;
      oss << "Step Balanced Low r=2 zStep=1 :ok."; 
      answer(oss.str());
    }
    if (head == "l") {  // left
      mover->stepMove(5.0,15,15,-15.115,0,3.,8.0,-.5,1.0);
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=4 l=0.25 r=1.0  :ok."; 
      answer(oss.str());
    }
    if (head == "r") {  // right
      mover->stepMove(5.0,15,15,-15.115,0,3.,8.0,1.0,-.5);
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=4 l=1.0 r=0.25 : ok."; 
      answer(oss.str());
    }
    if (head == "sf") {  // small forward
      smallForward();
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=2 :ok."; 
      answer(oss.str());
    }
    if (head == "sb") {  // small backup
      mover->stepMove(4.0,15,15,-15.115,0,-2.0,8.0,1.0,1.0);
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=-2 :ok."; 
      answer(oss.str());
    }
    if (head == "lwalk") {
	  load("Gait2_3.csv");
      ostringstream oss;
      oss << "played Gait2_3 script"; 
      answer(oss.str());
    }
    if (head == "LeftArmInOut" || head == "laio") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setLIO(angle);
      oss << "LeftArmInOut " << angle << " :ok."; 
      answer(oss.str());
    }    
    if (head == "LeftArmUpDown" || head == "laud") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setLUD(angle);
      oss << "LeftArmUpDown " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "LeftArmElbow" || head == "lae") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setLElbow(angle);
      oss << "LeftArmElbow " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "LeftArmBicep" || head == "lab") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setLBicep(angle);
      oss << "LeftArmBicep " << angle << " :ok."; 
      answer(oss.str());
    }    
    if (head == "LeftArmForearm" || head == "laf") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setLForearm(angle);
      oss << "LeftArmForearm " << angle << " :ok."; 
      answer(oss.str());
    }    
    if (head == "RightArmInOut" || head == "raio") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setRIO(angle);
      oss << "RightArmInOut " << angle << " :ok."; 
      answer(oss.str());
    }    
    if (head == "RightArmUpDown" || head == "raud") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setRUD(angle);
      oss << "RightArmUpDown " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "RightArmElbow" || head == "rae") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setRElbow(angle);
      oss << "RightArmElbow " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "RightArmBicep" || head == "rab") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setRBicep(angle);
      oss << "RightArmBicep " << angle << " :ok."; 
      answer(oss.str());
    }    
    if (head == "RightArmForearm" || head == "raf") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setRForearm(angle);
      oss << "RightArmForeArm " << angle << " :ok."; 
      answer(oss.str());
    }    
    if (head == "headPitch") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setPitch(angle);
      oss << "headPitch " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "headYaw" || head == "hy") {
      float angle;
      iss >> angle;
    ostringstream oss;
      setYaw(angle);
      oss << "headYaw " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "waist") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setWaist(angle);
      oss << "waist " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "play") {
      string file;
      iss >> file;
      ostringstream oss;
      oss << "play file '" << file << "' :" 
	  << (play(file) ? "ok" : "failed") << ".";
      answer(oss.str());
    }
    if (head == "load") {
      string file;
      iss >> file;
      ostringstream oss;
      oss << "load file '" << file << "' :" 
	  << (load(file) ? "ok" : "failed") << ".";
      answer(oss.str());
    }
    if (head == "time") {
      double value;
      if (iss >> value) {
	simTime = value;
	lastSimTime = value;
	oss << "set simTime/lastSimTime to " << value << ".";
      } else {
	oss << "simTime is " << setprecision(3) << fixed << simTime << " realTime is " << setprecision(3) << fixed << realTime << endl;
      }
      answer(oss.str());
    }
    if (head == "speed") {
      double value;
      if (iss >> value) {
	simSpeed = value;
	oss << "set speed to " << value << ".";
      } else {
	oss << "speed is " << simSpeed << endl;
      }
      answer(oss.str());
    }
    if (head == "clench") {
      string side;
      iss >> side;
      if (side == "LEFTARM" || side == "RIGHTARM") {
	HandTape hand(side);
	hand.opened(simTime);
	hand.grip(simTime+5.0);
	hand.gripped(simTime+10.0);
	hand.open(simTime+15.0);
	hand.write(*mover);
	answer(string("clenched ") + side);
      }
    }
    if (head == "arc") {
      string name;
      string arccommand;
      iss >> name >> arccommand;
      if (arccommand == "fromPoints") {
	Vec3d p1,p2,p3;
	while (isblank(iss.peek())) iss.get();
	if (iss.peek() == '[') {
	  iss >> p1;
	} else {
	  string p1name;
	  iss >> p1name;
	  p1 = vecs[p1name];
	}
	while (isblank(iss.peek())) iss.get();
	if (iss.peek() == '[') {
	  iss >> p2;
	} else {
	  string p2name;
	  iss >> p2name;
	  p2 = vecs[p2name];
	}
	while (isblank(iss.peek())) iss.get();
	if (iss.peek() == '[') {
	  iss >> p3;
	} else {
	  string p3name;
	  iss >> p3name;
	  p3 = vecs[p3name];
	}
	arcs[name].fromPoints(p1,p2,p3);
	oss << "arc " << name << ".fromPoints(p1=" << p1 << ",p2=" << p2 << ",p3=" << p3 <<")";
	answer(oss.str());
      }
    }


    if (head == "help") {
      oss << "laio|raio|laud|raud|lae|rae|lab|rab|laf|raf";
      answer(oss.str());
    }
    if (head == "temp"){
      ostringstream oss;
      string temp = "";
      iss >> temp;
      body->temp_report(oss, temp);
      answer("temp: " + oss.str());
    }
    if (head == "sense") {
      ostringstream out;
      out << " a=[" 
	  << sensors.a[0] << ","
	  << sensors.a[1] << ","
	  << sensors.a[2] << "]";
      
      out << " c=[" 
	  << sensors.c[0] << ","
	  << sensors.c[1] << ","
	  << sensors.c[2] << "]";
      
      out << " g=[" 
	  << sensors.g[0] << ","
	  << sensors.g[1] << ","
	  << sensors.g[2] << "]";
      
      out << " p=[" 
	  << sensors.p[0] << ","
	  << sensors.p[1] << ","
	  << sensors.p[2] << ","
	  << sensors.p[3] << "]";
      
      answer(out.str());
    }
  }


  void update()
  {
    int sleep_us = (1.0/cfg->num("body.servos.rate"))*1000000;

    realTime = now();
    lastRealTime=realTime;

    while (running) {
      usleep(sleep_us);
      realTime = now();
      simTime += simSpeed*(realTime-lastRealTime);
      mover->move(*body);
      lastRealTime = realTime;
      lastSimTime = simTime;

#if 0
      cout << " a=[" 
	  << sensors.a[0] << ","
	  << sensors.a[1] << ","
	  << sensors.a[2] << "]";
      
      cout << " c=[" 
	  << sensors.c[0] << ","
	  << sensors.c[1] << ","
	  << sensors.c[2] << "]";
      
      cout << " g=[" 
	  << sensors.g[0] << ","
	  << sensors.g[1] << ","
	  << sensors.g[2] << "]";
      
      cout << " p=[" 
	  << sensors.p[0] << ","
	  << sensors.p[1] << ","
	  << sensors.p[2] << ","
	  << sensors.p[3] << "]";

      cout << endl;
#endif
      
    }
  }

  bool rx(ZMQSubscribeSocket &socket)
  {
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    char *data = (char *)msg.data();
    string command((const char *)(data+1),data[0]);
    cout << "got: " << command << endl;
    act(command);
    return true;
  }

  bool tx(ZMQPublishSocket &socket)
  {
    Lock lock(repliesMutex);
    bool ok = true;

    while (!replies.empty()) {
      string &reply = *replies.begin();
      uint16_t size = (reply.size() < BODY_MESSAGE_MAXLEN) ? 
	reply.size() : BODY_MESSAGE_MAXLEN;

      ZMQMessage msg(size+2);
      char *data = (char *)msg.data();
      *((uint16_t*)data)=size;
      memcpy(data+2,&reply[0],size);
      if (msg.send(socket) == 0) ok = false;
      replies.pop_back();
    }
    return ok;
  }

  BodyController()
  {
    goUpdate = 0;
    mover = shared_ptr < BodyMover > (new BodyMover());
    hands_on = false;
    handsThread = 0;
    neckThread = 0;
    Amin = -40.0;
    Amax = 89.0;
    zmin = 8.0;
    zmax = -8.0;
    zoffset = 0;
    includeSavedCommands();
    goHome();
  }

  thread *goUpdate;

  void start()
  { 
    if (!running) {
      ZMQHub::start();
      goUpdate = new thread(&BodyController::update, this);
    }
    sensorsOn();
  }

  void join()
  {
    ZMQHub::join();
    if (goUpdate != 0) { 
      goUpdate->join(); 
      delete goUpdate; 
      goUpdate = 0; 
    }    
  }

  ~BodyController()
  {
    stop();
    BodyController::join();
    saveSavedCommands();

  }
};

int BodyController::lab = 0;

shared_ptr < BodyController > bodyController;

void SigIntHandler(int arg) {
  bodyController->stop();
}


void run()
{
  servoController = shared_ptr<ServoController>(CreateZMQServoController(cfg->str("body.servos.publish"),cfg->str("body.servos.subscribers"),cfg->num("body.servos.rate")));

  body = shared_ptr <Body> (new Body());
  body->init();

  bodyController = shared_ptr <BodyController> (new BodyController());
  bodyController->publish = cfg->str("body.commander.publish");
  bodyController->subscribers = cfg->list("body.commander.subscribers");
  bodyController->rxTimeout = 1e6;

  servoController->start();
  bodyController->start();
  safety=CreateSafetyClient(cfg->str("body.safety.publish"),cfg->str("safety.subscribe"),4);
  safety->safe(false);
  signal(SIGINT, SigIntHandler);
  signal(SIGTERM, SigIntHandler);
  signal(SIGQUIT, SigIntHandler);
  bodyController->join();
  bodyController.reset();
  servoController.reset();
  body.reset();
}

int main(int argc, char *argv[])
{
#if USE_PY != 0
  py = SPScript(new Script(argv[0]));
  py->import("__main__");
#endif

  simTime = 0;
  simSpeed = 1;
  realTime = now();

  cfg = shared_ptr < Configure > ( new Configure() );
  cfg->path("../../setup");
  cfg->args("body.",argv);
  if (argc == 1) cfg->load("config.csv");
  cfg->servos();
  verbose = cfg->flag("body.verbose",false);
  if (verbose) cfg->show();

  run();

  cout << "done" << endl;
  return 0;
}
