#include <csignal>
#include <map>
#include <iostream>
#include <assert.h>
#include <signal.h>
#include <unistd.h>

#include "Configure.h"
#include "zmq.hpp"
#include "ZMQHub.h"
#include "ZMQServoMessage.h"
#include "CreateFakeServoController.h"
#include "CreateDynamixelServoController.h"
#include "Servo.h"
#include "ScaledServo.h"
#include "CSVRead.h"


using namespace std;

Configure cfg;
bool verbose;

// manage all servo controllers (real or fake)

SPServoController fakeServoController;
SPServoController realServoController;

SPServo servo(string device, int id)
{
  SPServoController controller;
  if (device == "generic") {
    device = cfg.str("servos.generic","real");
  }
  if (device == "fake") {
    if (fakeServoController.get() == 0) {
      fakeServoController = SPServoController(CreateFakeServoController());
    }
    controller = fakeServoController;
  }
  if (device == "real") {
    if (realServoController.get() == 0) {
      realServoController = SPServoController(CreateDynamixelServoController(cfg,cfg.num("servos.deviceindex"),cfg.num("servos.baudnum")));
    }
    controller=realServoController;
  }
  
//  if (controller.get() == 0) throw out_of_range(device);
  
  SPServo ans = SPServo(controller->servo(id));
  return ans;
}

SPServo servo(string name)
{
  string device=cfg.servo(name,"device");
  int id=atoi(cfg.servo(name,"id").c_str());
  double scale = atof(cfg.servo(name,"scale").c_str());
  double offset = atof(cfg.servo(name,"offset").c_str());
  double torque = atof(cfg.servo(name,"torque").c_str());
  double speed = atof(cfg.servo(name,"speed").c_str());

  SPServo ans;

  if (scale == 1.0 && offset == 0.0) {
    ans = servo(device,id);
  } else {
    ans = SPServo(new ScaledServo(servo(device,id),scale,offset));
  }
  ans->torque(torque);
  ans->speed(speed);
  if (verbose) {
    cout << "servo device=" << device << " id=" << id 
	 << " scale=" << scale << " offset=" << offset 
	 << " speed=" << speed << " torque=" << torque << endl;
  }
  return ans;
}

class ZMQServoPlacebo : public ZMQHub
{
public:
  bool ready;
  ZMQServoPlacebo()
  {
    ready=false;
  }
  void rx(ZMQSubscribeSocket &socket) {
    ready = true;
  }

  void tx(ZMQPublishSocket &socket) {
  }
};

class ZMQServoServer : public ZMQHub
{
public:
  shared_ptr <Servo> NO_SERVO;
  typedef std::map<int,shared_ptr <Servo> > Servos;
  Servos servos;

  shared_ptr <Servo> servo(ZMQServoMessage *message)
  {
    Servos::iterator i = servos.find(message->servoId);
    if (i != servos.end()) return i->second;
    
    cout << "zmqservoserver: !!! request for unknown servo id: " << message->servoId << endl;
    return NO_SERVO;
  }

  void rx(ZMQSubscribeSocket &socket) {
    ZMQMessage msg;
    msg.recv(socket);
    ZMQServoMessage *data = (ZMQServoMessage *)msg.data();
    if (verbose) {
      if (data->messageId != ZMQServoMessage::SET_CURVE) {
	cout << "rx msg id=" << data->messageId << " servo=" << data->servoId << " value=" << data->value << endl;
      } else {
	ZMQServoCurveMessage *curveData = (ZMQServoCurveMessage *) data;
	cout << "rx msg id=" << curveData->messageId << " servo=" << curveData->servoId << " t=[" << curveData->t[0] << "," << curveData->t[1] << "] c0=[" << curveData->c0[0] << "," << curveData->c0[1] << "," << curveData->c0[2] << "]" << " c1=[" << curveData->c1[0] << "," << curveData->c1[1] << "," << curveData->c1[2] << "]"  << endl;
      }
    }
    switch(data->messageId) {
    case ZMQServoMessage::SET_ANGLE: servo(data)->angle(data->value); break;
    case ZMQServoMessage::SET_SPEED: servo(data)->speed(data->value); break;
    case ZMQServoMessage::SET_TORQUE: servo(data)->torque(data->value); break;
    case ZMQServoMessage::SET_CURVE: 
      ZMQServoCurveMessage *curveData = (ZMQServoCurveMessage*) data;
      servo(data)->curve(curveData->t,curveData->c0,curveData->c1);
      break;
    }
  }

  void tx(ZMQPublishSocket &socket) {
    for (Servos::iterator i=servos.begin(); i != servos.end(); ++i) {
      {
	ZMQMessage msg(sizeof(ZMQServoMessage));
	ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	
	data->messageId = ZMQServoMessage::GET_ANGLE;
	data->servoId = i->first;
	data->value = i->second->angle();
	
	if (verbose) {
	  cout << "tx msg id=" << data->messageId << " servo=" << data->servoId << " value=" << data->value << endl;
	}
	msg.send(socket);
      }
      {
	ZMQMessage msg(sizeof(ZMQServoMessage));
	ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	
	data->messageId = ZMQServoMessage::GET_TEMP;
	data->servoId = i->first;
	data->value = i->second->temp();
	
	if (verbose) {
	  cout << "tx msg id=" << data->messageId << " servo=" << data->servoId << " value=" << data->value << endl;
	}
	msg.send(socket);
      }
    }

  }

  void report()
  {
    cout << "report: ";
    cout << " tx/s=" << txRate;
    cout << " rx/s=" << rxRate;
    for (Servos::iterator i = servos.begin();
	 i != servos.end();
	 ++i) {
      cout << " angle[" << i->first << "]=" << i->second->angle() << " temp " << (int)i->second->temp()  << ",";
    }
    cout << endl;
  }

};

shared_ptr < ZMQServoPlacebo > placebo;

shared_ptr < ZMQServoServer > server;

void SigIntHandler(int arg) {
  server->stop();
}

void help()
{
  cout << "usage: servos options" << endl;
  cout << "\t --help (print help)"  << endl;
  cout << "\t --verbose (guess)" << endl;
  cout << "\t --generic [fake/real] (use these generic servos)" << endl;
  cout << "\t --deviceindex [num] (use dynamixel deviceNum)" << endl;
  cout << "\t --baudnum [num] (use dynamixel baudNum)" << endl;
  cout << "\t --rate [num] (use given tx rate)" << endl;
  cout << "\t --publish [name] (zmq publish as this name)" << endl;
  cout << "\t --subscribers [names,...] (zmq subscribers)" << endl;
  cout << "\t --servos [ids,...] (servo ids)" << endl;
  cout << "\t --configure [file] read csv configure file" << endl;
}



void args() 
{
  if (cfg.flag("servos.help",false)) help();
  server->rate=cfg.num("servos.rate");
  server->publish=cfg.str("servos.publish");
  server->subscribers=cfg.list("servos.subscribers");
  if (cfg.find("servos.servos")) {
    vector < string > servos = cfg.list("servos.servos");
    for (size_t i=0; i<servos.size(); ++i) {
      int id=atoi(servos[i].c_str());
      server->servos[id]=servo("generic",id);
    }
  } else {
    cfg.servos();
    set<string> names=cfg.servoNames();
    for (set<string>::iterator i=names.begin(); i != names.end(); ++i) {
      int id=atoi(cfg.servo(*i,"id").c_str());
      server->servos[id]=servo(*i);
    }
  }
}


void run() {
  server = shared_ptr < ZMQServoServer > (new ZMQServoServer());
  placebo = shared_ptr < ZMQServoPlacebo > ( new ZMQServoPlacebo() );
  server->NO_SERVO = servo("fake",999);

  args();

  placebo->rate=cfg.num("servos.rate");
  placebo->publish=cfg.str("servos.publish");
  placebo->subscribers=cfg.list("servos.subscribers");

  placebo->start();
  while (!placebo->ready) {
    cout << "waiting for body messages..." << endl;
    sleep(1);
  }
  placebo->stop();
  placebo->join();
  placebo = shared_ptr < ZMQServoPlacebo >();

  signal(SIGINT, SigIntHandler);
  signal(SIGTERM, SigIntHandler);

  if (fakeServoController) fakeServoController->start();
  if (realServoController) realServoController->start();
  server->start();

  server->join();

  server.reset();
  fakeServoController.reset();
  realServoController.reset();
}

int main(int argc,char **argv) {
  cfg.path("../../setup");
  cfg.args("servos.",argv);
  if (argc == 1) cfg.load("config.csv");
  verbose = cfg.flag("servos.verbose",false);
  if (verbose) cfg.show();
  run();

  cout << "done" << endl;
  return 0;
}
