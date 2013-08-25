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

using namespace std;

class BodyController : public ZMQHub
{
public:
  shared_ptr < BodyMover > mover;

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
  
  void setLIO(float angle) {
	mover->left.leftRight.setup(angle);
	mover->left.leftRight.torque=0.75;
  }
  void setLUD(float angle) {
	mover->left.upDown.setup(angle);
	mover->left.upDown.torque=0.75;
  }
  void setLBicep(float angle) {
	mover->left.bicep.setup(angle);
	mover->left.bicep.torque=0.75;
  }
  void setLElbow(float angle) {
	mover->right.elbow.setup(angle);
	mover->right.elbow.torque=0.75;
  }
  void setRIO(float angle) {
	mover->right.leftRight.setup(angle);
	mover->right.leftRight.torque=0.75;
  }
  void setRUD(float angle) {
	mover->right.upDown.setup(angle);
	mover->right.upDown.torque=0.75;
  }
  void setRBicep(float angle) {
	mover->right.bicep.setup(angle);
	mover->right.bicep.torque=0.75;
  }
  void setRElbow(float angle) {
	mover->right.elbow.setup(angle);
	mover->right.elbow.torque=0.75;
  }
  void goHome() {
	load("home.csv");
    setWaist(0);
    setLIO(-45);
    setLUD(-45);
    setLElbow(-15);
    setLBicep(-20);
    setRIO(45);
    setRUD(40);
    setRElbow(-5);
    setRBicep(0);    
  }
  
  void setPitch(float angle) {
    mover->neck.upDown.setup(angle);
    mover->neck.upDown.torque=0.5;
  }
  
  void setYaw(float angle) {
    mover->neck.leftRight.setup(angle);
    mover->neck.leftRight.torque=0.5;
  }

  void setWaist(float angle) {
    mover->waist.setup(angle);
  }
  
  bool load(const string &file)
  {
    vector<vector<double>> data;
    //                             0  1  2  3  4  5  6  7  8  9 10 11 12 13
    string headers=    "Time (seconds),x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,waist";
    if (!CSVRead(file,headers,data)) {
	  cout << "CSVRead failed " << endl;
      return false;
    }

    cout << "read '" << file << "' ok." << endl;

    //    // add last row to finish cycle
    //data.push_back(data[0]);
    //int nr=data.size();
    //data[nr-1][0]=data[nr-2][0]+(data[nr-2][0]-data[nr-3][0]);

    int nr=data.size();

    // assume regular spacing of samples
    double T = data[nr-1][0]-data[0][0] + (data[1][0]-data[0][0]);

    map < float , Point > t2tips[4];
    map < float , float > t2waist;

    for (size_t r=0; r<data.size(); ++r) {
      for (int el=0; el<4; ++el) {
	float t=data[r][0];
	t2tips[el][t]=Point(data[r][1+3*el],
				     data[r][2+3*el],
				     data[r][3+3*el]);
	t2waist[t]=data[r][13];
      }
    }

    mover->legs.setup(T,body->legs,t2tips,simTime,simTime+T);
    mover->waist.setup(T,t2waist,simTime,simTime+T);
    return true;
  }
  
  void yes()
  {
    mover->neck.upDown.wave(0,2,-15,15);
    mover->neck.leftRight.setup(0);
  }

  void straight() {
	setPitch(0);
	setYaw(0);
  }
  void sad() {
	setPitch(45);
	setWaist(10);
  }

  void act(string &command)
  {
    istringstream iss(command);
    ostringstream oss;

    string head;
    iss >> head;
    if (head == "RightArmLimp") {
      mover->right.torque(0);
      oss << "my right arm is numb!";
      answer(oss);
    }
    if (head == "RightArmMove") {
      mover->right.torque(0.75);
      oss << "my right arm is not numb.";
      answer(oss);
    }
    if (head == "report") {
      ostringstream oss;
      body->report(oss);
      answer(oss);
    }
    if (head == "yes") {
      yes();
    }
#if 0
    if (head == "no") {
      no();
    }
#endif
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
    if (head == "walk") {
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
    if (head == "r") {
	  load("R15.csv");
      ostringstream oss;
      oss << "played Gait2_3 script"; 
      answer(oss.str());
    }	
    if (head == "f") {
	  load("Gait2_3.csv");
      ostringstream oss;
      oss << "played Gait2_3 script"; 
      answer(oss.str());
    }
    if (head == "lwalk") {
	  load("Gait2_3.csv");
      ostringstream oss;
      oss << "played Gait2_3 script"; 
      answer(oss.str());
    }
    if (head == "LeftArmInOut") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setLIO(angle);
      oss << "LeftArmInOut " << angle << " :ok."; 
      answer(oss.str());
    }    
    if (head == "LeftArmUpDown") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setLUD(angle);
      oss << "LeftArmUpDown " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "LeftArmElbow") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setLElbow(angle);
      oss << "LeftArmElbow " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "LeftArmBicep") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setLBicep(angle);
      oss << "LeftArmBicep " << angle << " :ok."; 
      answer(oss.str());
    }    
    if (head == "RightArmInOut") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setRIO(angle);
      oss << "RightArmInOut " << angle << " :ok."; 
      answer(oss.str());
    }    
    if (head == "RightArmUpDown") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setRUD(angle);
      oss << "RightArmUpDown " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "RightArmElbow") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setRElbow(angle);
      oss << "RightArmElbow " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "RightArmBicep") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setRBicep(angle);
      oss << "RightArmBicep " << angle << " :ok."; 
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
	cout << "delta=" << delta_bar << " sigma=" << sqrt(delta2_bar - delta_bar*delta_bar) << " max=" << max_delta << endl;
	max_delta=0;
      }
      realTime = now();
      lastRealTime = realTime;
      mover->move(*body);
      double delta = now()-realTime;
      delta_bar = (1-rho)*delta_bar + rho*delta;
      delta2_bar = (1-rho)*delta2_bar + rho*delta*delta;
      if (delta > max_delta) max_delta=delta;

      if (delta > delta_bar + 3*sqrt(delta2_bar - delta_bar*delta_bar)) {
	cout << "slow at simTime = " << simTime << " realTime=" << realTime << endl;
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
  }

  thread *goUpdate;

  void start()
  { 
    if (!running) {
      ZMQHub::start();
      goUpdate = new thread(&BodyController::update, this);
    }
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
  servoController = shared_ptr<ServoController>(CreateZMQServoController(cfg.str("body.servos.publish"),cfg.str("body.servos.subscribers")));

  body = shared_ptr <Body> (new Body());
  body->init();

  bodyController = shared_ptr <BodyController> (new BodyController());
  bodyController->publish = cfg.str("body.commander.publish");
  bodyController->subscribers = cfg.list("body.commander.subscribers");

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
  simTime = 0;
  simSpeed = 1;
  realTime = now();
  
  cfg.path("../../setup");
  cfg.args("body.",argv);
  if (argc == 1) cfg.load("config.csv");
  cfg.servos();
  verbose = cfg.flag("body.verbose",false);
  if (verbose) cfg.show();

  run();
  cout << "done" << endl;
  return 0;
}

