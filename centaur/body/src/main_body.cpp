#include <csignal>
#include <signal.h>
#include <iostream>
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
#include "Script.h"
#include "StdCapture.h"
#include "leapStruct.h"
#include "joystick.h"


SPScript py;

void subscribeH(void *zmq_sub, Hands* manos) 
{
	manos->clear();
	int rc = zmq_recv(zmq_sub, manos, sizeof(Hands), 0);
	manos->lthumb=manos->lthumb;
	manos->ltrigger=manos->ltrigger;
	manos->lmiddle=manos->lmiddle; 
	manos->lring=manos->lring;
	manos->rthumb=manos->rthumb;
	manos->rtrigger=manos->rtrigger; 
	manos->rmiddle=manos->rmiddle; 
	manos->rring=manos->rring;
}

float mapForearmAngle(float angle)
{
  angle *= -1.0;
  if (angle < -44.0) angle = -44.0;
  if (angle > 44.0) angle = 44.0;
  return angle;
}

void subscribeF(void *zmq_sub, leapData *leapItem)
{
  leapItem->clear();
  int rc = zmq_recv(zmq_sub, leapItem, sizeof(leapData), 0);
  leapItem->lroll = mapForearmAngle(leapItem->lroll);
  //leapItem->rroll = mapForearmAngle(leapItem->rroll);
  //leapItem->lpitch = mapForearmAngle(leapItem->lpitch);
  //leapItem->rpitch = mapForearmAngle(leapItem->rpitch);
  //NEED XYZ MAPPING STUFF
}

void subscribeN(void* zmq_sub, joystick* j)
{
	int rc = zmq_recv(zmq_sub, j, sizeof(joystick), ZMQ_DONTWAIT);
}

using namespace std;

class BodyController : public ZMQHub
{
public:
  list < string > replies;
  mutex repliesMutex;

  void answer(const string &reply)
  {
    Lock lock(repliesMutex);
    replies.push_back(reply);
  }

  void answer(ostringstream &oss)
  {
    answer(oss.str());
  }

  std::thread* sensorsThread;
  std::atomic < bool > sensors_on;
  void subscribeToSensors()
  {
    std::string address = cfg->str("sensors.requester.address");
    
    void* context = zmq_ctx_new();
    void* zmq_sub = zmq_socket(context, ZMQ_SUB);
    int hwm=1;
    int rc;
    rc = zmq_setsockopt(zmq_sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
    rc = zmq_setsockopt(zmq_sub, ZMQ_SUBSCRIBE, "", 0);

    if (zmq_connect(zmq_sub, address.c_str()) != 0) {
      printf("Error initializing 0mq...\n");
      return;
    }
	
    while(sensors_on.load()) {
      zmq_recv(zmq_sub, &sensors, sizeof(SensorsMessage), 0);
    }

    zmq_close(zmq_sub);
    zmq_ctx_destroy(context);
  }

  std::thread* neckThread;
  std::atomic < bool > neck_on;
  void subscribeToNeck() {	//Neck thread function
	float currentUpDown = 0;
	float currentLeftRight = 0;
	int rc;
	int hwm = 1;
	joystick jm;
	std::string address = cfg->str("body.commander.neckAddress", "tcp://192.168.2.113:5556");

	void* context = zmq_ctx_new();
	void* sub = zmq_socket(context, ZMQ_SUB);
	rc = zmq_setsockopt(sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);

	if (zmq_connect(sub, address.c_str()) != 0)
	{
		printf("Error initializing 0mq...\n");
		return;
	}
	
	double t = now();
	while(neck_on.load())
	{
		double deltat = now()-t;
		t=t+deltat;
		subscribeN(sub, &jm);
		currentUpDown = currentUpDown + (deltat)*(float(jm.y2)/32.0)*(15.0);
		currentLeftRight = currentLeftRight + (deltat)*(float(jm.x2)/32.0)*(15.0);
		if(currentUpDown < -33) currentUpDown = -33;
		else if(currentUpDown > 33) currentUpDown = 33;
		if(currentLeftRight < -33) currentLeftRight = -33;
		else if(currentLeftRight > 33) currentLeftRight = 33;
		mover->neck.upDown.setup(currentUpDown);
		mover->neck.leftRight.setup(currentLeftRight);
		std::this_thread::sleep_for(std::chrono::microseconds(25));
	}  

	zmq_close(sub);
	zmq_ctx_destroy(context);
  }
  
  std::thread* handsThread;
  std::atomic < bool > hands_on;
  void subscribeToHands() {	//Hands thread function
	int rc;
	Hands manos;
	std::string address = cfg->str("body.commander.glovesAddress", "tcp://192.168.2.113:6689");
	
	void *context = zmq_ctx_new ();
	void *sub = zmq_socket(context, ZMQ_SUB);
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
	
	if (zmq_connect(sub, address.c_str()) != 0)
	{
		printf("Error initializing 0mq...\n");
		return;
	}
	
//	cout << "starting hands control." << endl;
	while(hands_on.load())
	{
			subscribeH(sub, &manos);
			mover->left.trigger.setup(manos.ltrigger);
			mover->left.middle.setup(manos.lmiddle);
			mover->left.ring.setup(manos.lring);
			mover->left.thumb.setup(manos.lthumb);
			mover->right.trigger.setup(manos.rtrigger);
			mover->right.middle.setup(manos.rmiddle);
			mover->right.ring.setup(manos.rring);
			mover->right.thumb.setup(manos.rthumb);
//			cout << "adjusted hands (rthumb=" << manos.rthumb << ")." << endl;
			std::this_thread::sleep_for(std::chrono::microseconds(25));
	}	
//	cout << "ending hands control." << endl;	
	zmq_close(sub);
	zmq_ctx_destroy(context);
  }
  

  std::thread* forearmsThread;
  std::atomic < bool > forearms_on;
  void subscribeToForearms() {	//Forearms thread function
    int rc;
    leapData leap;
    std::string address = cfg->str("body.commander.leapAddress", "tcp://192.168.2.113:9990");	

	void *context = zmq_ctx_new ();
	void *sub = zmq_socket(context, ZMQ_SUB);
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
	if (zmq_connect(sub, address.c_str()) != 0)
	{
		printf("Error initializing 0mq ARMS...\n");
		return;
	}	

	while(forearms_on.load())
	{
			subscribeF(sub,&leap);
			mover->left.forearm.setup(leap.lroll);
			//mover->right.forearm.setup(leap.rroll);
			mover->left.elbow.setup(leap.lpitch);
			//mover->right.elbow.setup(leap.rpitch);
			std::this_thread::sleep_for(std::chrono::microseconds(25));
	}	
//	cout << "ending forearm control." << endl;	
	zmq_close(sub);
	zmq_ctx_destroy(context);
  }


  void setLIO(float angle) {
	mover->left.leftRight.setup(angle);
//	mover->left.leftRight.torque=0.75;
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
	mover->right.leftRight.setup(angle);
//	mover->right.leftRight.torque=0.75;
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
    setRIO(40);
    setRUD(40);
    setRElbow(-40);
    setRBicep(0);    
  }  
  void goHome() {
    load("home.csv");
    setWaist(0);
    setLIO(-40);
    setLUD(-40);
    setLElbow(40);
    setLBicep(-20);
    setRIO(40);
    setRUD(40);
    setRElbow(-40);
    setRBicep(0);    
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
  
  void shake() {
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
	setRUD(30);
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
     if(sensorsThread == 0) {
       sensors_on.store(true);
       sensorsThread = new std::thread(&BodyController::subscribeToSensors, this);
     }
  }

  void sensorsOff()
  {
     if (sensorsThread != 0) {
       sensors_on.store(false);
       sensorsThread->join();
       delete sensorsThread;
       sensorsThread = 0;
     }
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

  void forearmsOn()
  {
    if (forearmsThread == 0) {
      forearms_on.store(true);
      forearmsThread = new std::thread(&BodyController::subscribeToForearms, this);
    }
  }

   void forearmsOff()
  {
     if (forearmsThread != 0) {
       forearms_on.store(false);
       forearmsThread->join();
       delete forearmsThread;
       forearmsThread = 0;
     }
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

  void act(string &command)
  {
    istringstream iss(command);
    ostringstream oss;

    string head;
    iss >> head;
    if (head == "py") {
      StdCapture capture;
      capture.BeginCapture();
      if (command.length() > 2) py->run(command.substr(3));
      capture.EndCapture();
      answer(capture.GetCapture());
    }
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
        mover->stepMove(4,12.0,17.25,-14.665,0,3.0,6,1.0,1.0);
		oss << "DStep r=4 xstep=0 ystep=4 :ok"; 
      answer(oss.str());
    }
    if (head == "dsf") {
        mover->stepMove(4,12.0,17.25,-14.665,0,1.5,6,1.0,1.0);
		oss << "DStep r=2 xstep=0 ystep=4 :ok"; 
      answer(oss.str());
    }
    if (head == "dsb") {
        mover->stepMove(4,12.0,17.25,-14.665,0,-1.5,6,1.0,1.0);
		oss << "DStep r=2 xstep=0 ystep=4 :ok"; 
      answer(oss.str());
    }
    if (head == "dr") {
        mover->stepMove(4,12.0,17.25,-14.665,0,2,6,1.0,.1);
		oss << "DStep r=4 xstep=0 ystep=4 :ok"; 
      answer(oss.str());
    }
    if (head == "dl") {
        mover->stepMove(4,12.0,17.25,-14.665,0,2,6,.1,1.0);
		oss << "DStep r=4 xstep=0 ystep=4 :ok"; 
      answer(oss.str());
    }
    if (head == "Step") {
      float radius=4,ystep=0,xstep=0,left=1.0,right=1.0;
      iss >> radius >> xstep >> ystep;
      if (fabs(radius)<=6 && fabs(xstep)<=6 && fabs(ystep)<=6) {
        mover->stepMove(radius,14.9,14.9,-14.665,xstep,ystep,8,left,right);
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
         sensorsOn();
	 answer("my sensors are on.");
      } else if (value == "off") {
	sensorsOff();
	answer("my sensors are off.");
      }
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
    if (head=="forearms"){
      string value;
      iss >> value;
      if (value == "on"){
	forearmsOn();
	answer("my forearms are on.");
      } else if (value=="off") {
	forearmsOff();
	answer("my forearms are off.");
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
      load("FLiftUpFront.csv");
      answer("Front stepping on brick");
    }
    if (head == "fob") {  // Forward Front On Brick
      load("FOnBrickFront.csv");
      answer("Front on brick");
    }
    if (head == "fdd") {  // Forward Front Drop Down from brick
      load("FDropDownFront.csv");
      answer("Front stepping off brick");
    }
    if (head == "blu") {  // Forward Back Lift Up front for brick
      load("FLiftUpBack.csv");
      answer("Back stepping on brick");
    }
    if (head == "bob") {  // Forward Back On Brick
      load("FOnBrickBack.csv");
      answer("Back on brick");
    }
    if (head == "bdd") {  // Forward Back Drop Down from brick
      load("FDropDownBack.csv");
      answer("Back stepping off brick");
    }
    if (head == "f") {  // forward
      mover->stepMove(4.0,14.9,14.9,-15.665,0,3.,8.0,1.0,1.0);
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=4 :ok."; 
      answer(oss.str());
    }
    if (head == "lf") {  // forward
      mover->stepMove(4.0,14.9,14.9,-9.915,0,3.,5.0,1.0,1.0);
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=4 :ok."; 
      answer(oss.str());
    }
    if (head == "l") {  // left
      mover->stepMove(4.0,14.9,14.9,-15.665,0,3.,9.0,0.25,1.0);
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=4 l=0.25 r=1.0  :ok."; 
      answer(oss.str());
    }
    if (head == "r") {  // right
      mover->stepMove(4.0,14.9,14.9,-15.665,0,3.,9.0,1.0,0.25);
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=4 l=1.0 r=0.25 : ok."; 
      answer(oss.str());
    }
    if (head == "sf") {  // small forward
      mover->stepMove(4.0,14.9,14.9,-15.665,0,1.5,8.0,1.0,1.0);
      ostringstream oss;
      oss << "Step r=4 xstep=0 ystep=2 :ok."; 
      answer(oss.str());
    }
    if (head == "sb") {  // small backup
      mover->stepMove(4.0,14.9,14.9,-16.665,0,-1.5,9.0,1.0,1.0);
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
    if (head == "headYaw") {
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
      iss >> value;
      simTime = value;
      oss << "set time to " << value << ".";
      answer(oss.str());
    }
    if (head == "speed") {
      double value;
      iss >> value;
      simSpeed = value;
      oss << "set speed to " << value << ".";
      answer(oss.str());
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
    double rho=0.01;
    double lastRealTime=realTime;
    double delta_bar=0;
    double delta2_bar=0;
    double max_delta=0;
    while (running) {
      usleep(int((1.0/MOVE_RATE)*1000000));
      realTime = now();
      simTime += simSpeed*(realTime-lastRealTime);
      if (floor(realTime) != floor(lastRealTime)) {
//	cout << "delta=" << delta_bar << " sigma=" << sqrt(delta2_bar - delta_bar*delta_bar) << " max=" << max_delta << endl;
	max_delta=0;
	cout << "body: realTime=" << realTime << " delta=" << now()-realTime << endl;
      }
      lastRealTime = realTime;
      mover->move(*body);
      double delta = now()-realTime;
      delta_bar = (1-rho)*delta_bar + rho*delta;
      delta2_bar = (1-rho)*delta2_bar + rho*delta*delta;
      if (delta > max_delta) max_delta=delta;

      if (delta > delta_bar + 3*sqrt(delta2_bar - delta_bar*delta_bar)) {
	cout << "slow at simTime = " << simTime << " realTime=" << realTime << endl;
      }
      if (floor(realTime) != floor(lastRealTime)) {

      }
    }
  }

  void rx(ZMQSubscribeSocket &socket)
  {
    ZMQMessage msg;
    msg.recv(socket);
    char *data = (char *)msg.data();
    string command((const char *)(data+1),data[0]);
    cout << "got: " << command << endl;
    act(command);
  }

  void tx(ZMQPublishSocket &socket)
  {
    Lock lock(repliesMutex);

    while (!replies.empty()) {
      string &reply = *replies.begin();
      uint8_t size = (reply.size() < BODY_MESSAGE_MAXLEN) ? 
	reply.size() : BODY_MESSAGE_MAXLEN;

      ZMQMessage msg(size+1);
      char *data = (char *)msg.data();
      data[0]=size;
      memcpy(data+1,&reply[0],size);
      msg.send(socket);
      replies.pop_back();
    }
  }

  BodyController()
  {
    goUpdate = 0;
    mover = shared_ptr < BodyMover > (new BodyMover());
    hands_on = false;
    handsThread = 0;
    neckThread = 0;
    sensorsThread = 0;
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
  }

};

shared_ptr < BodyController > bodyController;

void SigIntHandler(int arg) {
  bodyController->stop();
}


void run()
{
  servoController = shared_ptr<ServoController>(CreateZMQServoController(cfg->str("body.servos.publish"),cfg->str("body.servos.subscribers")));

  body = shared_ptr <Body> (new Body());
  body->init();

  bodyController = shared_ptr <BodyController> (new BodyController());
  bodyController->publish = cfg->str("body.commander.publish");
  bodyController->subscribers = cfg->list("body.commander.subscribers");

  servoController->start();
  bodyController->start();
  signal(SIGINT, SigIntHandler);
  bodyController->join();
  bodyController.reset();
  servoController.reset();
  body.reset();
}

int main(int argc, char *argv[])
{
  py = SPScript(new Script(argv[0]));
  py->import("__main__");

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
