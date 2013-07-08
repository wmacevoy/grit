#include <csignal>
#include <map>
#include <iostream>
#include <assert.h>
#include <signal.h>

#include "zmq.hpp"
#include "ZMQHub.h"
#include "ZMQServoMessage.h"
#include "CreateFakeServoController.h"
#include "CreateDynamixelServoController.h"
#include "Servo.h"


using namespace std;

const int DXL_DEVICE_INDEX = 1;
const int DXL_BAUD_NUM = 34;

struct ServoMap { const char *device; int id; };

const ServoMap TEST_SERVOS[] =
  {
    { "generic", 1  },
    { 0, -1} // end
  };

const ServoMap ROBOT_SERVOS[] =
  {
    { "generic", 11  },
    { "generic", 12  },
    { "generic", 13  },
    { "generic", 21  },
    { "generic", 22  },
    { "generic", 23  },
    { "generic", 31  },
    { "generic", 32  },
    { "generic", 33  },
    { "generic", 41  },
    { "generic", 42  },
    { "generic", 43  },
    { "generic", 91  },
    { "generic", 93  },
    { "generic", 94  },
    { 0, -1 } // end
  };


const int TX_RATE=20;

static const char * SUBSCRIBERS [] = 
  {
    "tcp://localhost:5501",
    "tcp://localhost:5502",
    0 // end
  };

const char *PUBLISH = "tcp://*:5500";

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
    deviceIndex = DXL_DEVICE_INDEX;
    baudNum = DXL_BAUD_NUM;
  }

  ServoController* create(const std::string &deviceName)
  {
    if (deviceName == "fake") {
      return CreateFakeServoController();
    }
    if (deviceName == "real") {
      return CreateDynamixelServoController(deviceIndex,baudNum);
    }
    cout << "unknown device " << deviceName << endl;
    assert(false);
  }

  ServoController *lookup(const std::string &deviceName)
  {
    string nonGenericDeviceName = (deviceName == "generic") ? genericName : deviceName;
    All::iterator i=all.find(nonGenericDeviceName);
    if (i != all.end()) return &*(i->second);
    ServoController *device = create(nonGenericDeviceName);
    all[nonGenericDeviceName]=shared_ptr<ServoController>(device);
    return device;
  }
  
  Servo* servo(const std::string &device, int id)
  {
    Servo *ans = lookup(device)->servo(id);
    if (verbose) {
      cout << "servo " << device << ":" << id << " at " << (void*) ans << endl;
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
  Servo *NO_SERVO;
  typedef std::map<int,Servo*> Servos;
  Servos servos;

  Servo* servo(ZMQServoMessage *message)
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
      cout << "rx msg id=" << data->messageId << " servo=" << data->servoId << " value=" << data->value << endl;
    }
    switch(data->messageId) {
    case ZMQServoMessage::SET_ANGLE: servo(data)->angle(data->value); break;
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
      cout << " angle[" << i->first << "]=" << i->second->angle();
    }
    cout << endl;
  }
};

ZMQServoServer *pserver=0;

void SigIntHandler(int arg) {
  pserver->stop();
}

void run(int argc, char **argv) {

  // basic objects
  Controllers controllers;
  ZMQServoServer server;

  // default configuration
  verbose = false;
  const ServoMap *servoMap = ROBOT_SERVOS;
  controllers.genericName = "real";

  server.NO_SERVO = controllers.servo("fake",999);
  server.rate = TX_RATE;

  for (int i=0; SUBSCRIBERS[i] != 0; ++i) {
    server.subscribers.push_back(SUBSCRIBERS[i]);
  }
  server.publish = PUBLISH;

  // parse arguments for run-time configuration

  for (int argi=1; argi<argc; ++argi) {
    if (strcmp(argv[argi],"--help") == 0) {
      cout << "usage: " << argv[0] << " options" << endl;
      cout << "\t --help (print help)"  << endl;
      cout << "\t --verbose (guess)" << endl;
      cout << "\t --fake (use fake generic servos)" << endl;
      cout << "\t --real (use real generic servos)" << endl;
      cout << "\t --robot (use robot servo map)" << endl;
      cout << "\t --test (use test servo map)" << endl;
      cout << "\t --deviceIndex [num] (use dynamixel deviceNum)" << endl;
      cout << "\t --baudNum [num] (use dynamixel baudNum)" << endl;
      cout << "\t --rate [num] (use give tx rate)" << endl;
      cout << "\t --publish [name] (zmq publish as this name)" << endl;
      cout << "\t --subscribers [names,...] (zmq subscribers)" << endl;
      cout << "\t --servos [ids,...] (servo ids)" << endl;
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
    if (strcmp(argv[argi],"--test") == 0) {
      servoMap = TEST_SERVOS;
      continue;
    }
    if (strcmp(argv[argi],"--robot") == 0) {
      servoMap = ROBOT_SERVOS;
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
      servoMap = 0;
      continue;
    }
    cout << "unkown arg '" << argv[argi] << "' ignored."  << endl;
  }

  if (servoMap != 0) {
    for (const ServoMap *i=servoMap; i->device != 0; ++i) {
      server.servos[i->id]=controllers.servo(i->device,i->id);
    }
  }

  pserver = &server;

  controllers.start();
  server.start();

  signal(SIGINT, SigIntHandler);

  server.join();
}

int main(int argc,char **argv) {
  run(argc,argv);
  cout << "done" << endl;
}
