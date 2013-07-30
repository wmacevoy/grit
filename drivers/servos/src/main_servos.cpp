#include <csignal>
#include <map>
#include <iostream>
#include <assert.h>
#include <signal.h>

#include "config.h"
#include "zmq.hpp"
#include "ZMQHub.h"
#include "ZMQServoMessage.h"
#include "CreateFakeServoController.h"
#include "CreateDynamixelServoController.h"
#include "Servo.h"
#include "ScaledServo.h"
#include "CSVRead.h"


using namespace std;

bool verbose;

// manage all servo controllers (real or fake)
struct Controllers
{
  typedef map < string , shared_ptr < ServoController > > All;
  All all;
  string genericName;
  int deviceIndex;
  int baudNum;

  Controllers() { 
    genericName="real";
    deviceIndex = 0;
    baudNum = 34;
  }

  shared_ptr<ServoController> create(const std::string &deviceName)
  {
    if (deviceName == "fake") {
      return shared_ptr<ServoController>(CreateFakeServoController());
    }
    if (deviceName == "real") {
      return shared_ptr<ServoController>(CreateDynamixelServoController(deviceIndex,baudNum));
    }
    cout << "unknown device " << deviceName << endl;
    assert(false);
  }

  shared_ptr<ServoController> lookup(const std::string &deviceName)
  {
    string nonGenericDeviceName = (deviceName == "generic") ? genericName : deviceName;
    All::iterator i=all.find(nonGenericDeviceName);
    if (i != all.end()) return (i->second);
    return all[nonGenericDeviceName]=create(nonGenericDeviceName);
  }
  
  shared_ptr < Servo >  servo(const std::string &device, int id)
  {
    shared_ptr < Servo > ans(lookup(device)->servo(id));
    if (verbose) {
      cout << "servo " << device << ":" << id << " at " << (void*) &*ans << endl;
    }
    return ans;
  }

  void start()
  {
    for (All::iterator i=all.begin(); i!=all.end(); ++i) {
      if (verbose) {
	cout << "controller " << i->first << " start." << endl;
      }
      i->second->start();
    }
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
  }

  void report()
  {
    cout << "report: ";
    cout << " tx/s=" << txRate;
    cout << " rx/s=" << rxRate;
    for (Servos::iterator i = servos.begin();
	 i != servos.end();
	 ++i) {
      cout << " angle[" << i->first << "]=" << i->second->angle() /*<< " speed " << i->second->speed()  */<< ",";
    }
    cout << endl;
  }

};

ZMQServoServer *pserver=0;

void SigIntHandler(int arg) {
  pserver->stop();
}

void configure(const string &config_csv,
	       Controllers &controllers,  ZMQServoServer &server) {

  vector < vector < string > > values;
  if (!CSVRead(config_csv,"name,value",values)) {
    cout << "Could not read configuration file '" << config_csv << "'." << endl;
    exit(1);
  }
  map<string,string> cfg;
  for (size_t i=0; i != values.size(); ++i) {
    cfg[values[i][0]]=values[i][1];
  }
  if (cfg.find("servos.verbose") != cfg.end()) {
    verbose=cfg["servos.verbose"] == "true";
  }
  if (cfg.find("servos.type") != cfg.end()) {
    controllers.genericName = cfg["servos.type"];
  }
  if (cfg.find("servos.deviceindex") != cfg.end()) {
    controllers.deviceIndex = atoi(cfg["servos.deviceindex"].c_str());
  }
  if (cfg.find("servos.baudnum") != cfg.end()) {
    controllers.baudNum = atoi(cfg["servos.baudnum"].c_str());
  }
  if (cfg.find("servos.rate") != cfg.end()) {
    server.rate = atof(cfg["servos.rate"].c_str());
  }
  if (cfg.find("servos.publish") != cfg.end()) {
    server.publish = cfg["servos.publish"];
  }
  if (cfg.find("servos.subscribers") != cfg.end()) {
    string arg=cfg["servos.subscribers"];
    server.subscribers.clear();
    while (arg.length() > 0) {
      size_t comma = arg.find(';');
      string subscriber = arg.substr(0,(comma != string::npos) ? comma : arg.length());
      server.subscribers.push_back(subscriber);
      arg=arg.substr((comma != string::npos) ? comma+1 : arg.length());
    }
  }
  
  if (cfg.find("servos.map") != cfg.end()) {
    vector < vector < string > > csvServoMap;
    if (CSVRead(cfg["servos.map"],"device,id,scale,offset",csvServoMap)) {
      for (size_t i=0; i<csvServoMap.size(); ++i) {
	string device=csvServoMap[i][0];
	int id=atoi(csvServoMap[i][1].c_str());
	double scale = atof(csvServoMap[i][2].c_str());
	double offset = atof(csvServoMap[i][3].c_str());
	
	shared_ptr < Servo > servo(controllers.servo(device,id));
	if (scale == 1.0 && offset == 0.0) {
	  if (verbose) {
	    cout << "servo device=" << device << " id=" << id << endl;
	  }
	  server.servos[id]=servo;
	} else {
	  if (verbose) {
	    cout << "servo device=" << device << " id=" << id 
		 << " scale=" << scale << " offset=" << offset << endl;
	  }
	  server.servos[id]=shared_ptr < Servo > (new ScaledServo(servo,scale,offset));
	}
      }
    } else {
      cout << "Could not read servo map in '" 
	   << cfg["servos.map"] << "'." << endl;
      exit(1);
    }
  }
}

void args(int argc, char **argv, Controllers &controllers,  ZMQServoServer &server) {
  verbose = false;
  controllers.genericName = "real";

  server.rate = 1;
  server.NO_SERVO = controllers.servo("fake",999);

  for (int argi=1; argi<argc; ++argi) {
    if (strcmp(argv[argi],"--help") == 0) {
      cout << "usage: " << argv[0] << " options" << endl;
      cout << "\t --help (print help)"  << endl;
      cout << "\t --verbose (guess)" << endl;
      cout << "\t --fake (use fake generic servos)" << endl;
      cout << "\t --real (use real generic servos)" << endl;
      cout << "\t --deviceIndex [num] (use dynamixel deviceNum)" << endl;
      cout << "\t --baudNum [num] (use dynamixel baudNum)" << endl;
      cout << "\t --rate [num] (use given tx rate)" << endl;
      cout << "\t --publish [name] (zmq publish as this name)" << endl;
      cout << "\t --subscribers [names,...] (zmq subscribers)" << endl;
      cout << "\t --servos [ids,...] (servo ids)" << endl;
      cout << "\t --configure [file] read csv configure file" << endl;
      return;
    }
    if (strcmp(argv[argi],"--verbose") == 0) {
      verbose=true;
      continue;
    }
    if (strcmp(argv[argi],"--fake") == 0) {
      controllers.genericName = "fake";
      continue;
    }
    if (strcmp(argv[argi],"--real") == 0) {
      controllers.genericName = "real";
      continue;
    }
    if (strcmp(argv[argi],"--deviceIndex") == 0) {
      ++argi;
      controllers.deviceIndex = atoi(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"--baudNum") == 0) {
      ++argi;
      controllers.baudNum = atoi(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"--rate") == 0) {
      ++argi;
      server.rate = atoi(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"--publish") == 0) {
      ++argi;
      server.publish = argv[argi];
      continue;
    }
    if (strcmp(argv[argi],"--subscribers") == 0) {
      ++argi;
      string arg=argv[argi];
      server.subscribers.clear();
      while (arg.length() > 0) {
	size_t comma = arg.find(',');
	string subscriber = arg.substr(0,(comma != string::npos) ? comma : arg.length());
	server.subscribers.push_back(subscriber);
	arg=arg.substr((comma != string::npos) ? comma+1 : arg.length());
      }
      continue;
    }

    if (strcmp(argv[argi],"--servos") == 0) {
      ++argi;
      string arg=argv[argi];
      while (arg.length() > 0) {
	size_t comma = arg.find(',');
	int id=atoi(arg.substr(0,(comma != string::npos) ? comma : arg.length()).c_str());
	server.servos[id]=controllers.servo("generic",id);
	arg=arg.substr((comma != string::npos) ? comma+1 : arg.length());
      }
      continue;
    }

    if (strcmp(argv[argi],"--configure") == 0) {
      ++argi;
      configure(argv[argi],controllers,server);
      continue;
    }
    cout << "unkown arg '" << argv[argi] << "' ignored."  << endl;
  }

  if (argc == 1) {
    configure(CONFIG_CSV,controllers,server);
  }
}

void run(int argc, char **argv) {

  // basic objects
  Controllers controllers;
  ZMQServoServer server;

  // parse arguments for run-time configuration
  pserver = &server;

  args(argc,argv,controllers,server);


  controllers.start();
  server.start();

  signal(SIGINT, SigIntHandler);
  signal(SIGTERM, SigIntHandler);

  server.join();
}

int main(int argc,char **argv) {
  run(argc,argv);
  cout << "done" << endl;
  return 0;
}
