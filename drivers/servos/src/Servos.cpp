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

struct Controllers
{
  shared_ptr<ServoController> fake;
  shared_ptr<ServoController> dynamixel;
  vector < shared_ptr<ServoController> > all;
  Controllers() :
    fake(CreateFakeServoController()),
    dynamixel(CreateDynamixelServoController(DXL_DEVICE_INDEX,DXL_BAUD_NUM))
  {
    all.push_back(fake);
    all.push_back(dynamixel);
  }
  void start()
  {
    for (size_t i=0; i<all.size(); ++i) all[i]->start();
  }
} controllers;

const struct { ServoController *controller; int id; } SERVOS [] =
  {
    //{ controller, id },
#if 0
    { &*controllers.dynamixel, 1  },
    { &*controllers.fake, 2  },
    { &*controllers.fake, 10 },
    { &*controllers.fake, 20 },
#endif
#if 1
    { &*controllers.dynamixel, 11  },
    { &*controllers.dynamixel, 12  },
    { &*controllers.dynamixel, 13  },
    { &*controllers.dynamixel, 21  },
    { &*controllers.dynamixel, 22  },
    { &*controllers.dynamixel, 23  },
    { &*controllers.dynamixel, 31  },
    { &*controllers.dynamixel, 32  },
    { &*controllers.dynamixel, 33  },
    { &*controllers.dynamixel, 41  },
    { &*controllers.dynamixel, 42  },
    { &*controllers.dynamixel, 43  },
    { &*controllers.dynamixel, 91  },
    { &*controllers.dynamixel, 93  },
    { &*controllers.dynamixel, 94  },
#endif
    { 0,    0} // end
  };

const int TX_RATE=20;

static const char * SUBSCRIBERS [] = 
  {
    "tcp://localhost:5501",
    "tcp://localhost:5502",
    0 // end
  };

const char *PUBLISH = "tcp://*:5500";


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
    //	cout << "msg id=" << data->messageId << " servo=" << data->servoId << " value=" << data->value << endl;
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
  cout << "sigint caught" << endl;
  pserver->stop();
}

int main(int argc,char **argv) {
  {
    ZMQServoServer server;
    server.NO_SERVO = controllers.fake->servo(999); 

    server.rate = TX_RATE;

    for (int i=0; SERVOS[i].id != 0; ++i) {
      server.servos[SERVOS[i].id]=SERVOS[i].controller->servo(SERVOS[i].id);
    }
    
    for (int i=0; SUBSCRIBERS[i] != 0; ++i) {
      server.subscribers.push_back(SUBSCRIBERS[i]);
    }
    
    server.publish = PUBLISH;

    for (int i=0; SERVOS[i].id != 0; ++i) {
      server.servos[SERVOS[i].id]=SERVOS[i].controller->servo(SERVOS[i].id);
    }

    pserver = &server;

    controllers.start();
    server.start();

    signal(SIGINT, SigIntHandler);

    server.join();
  }

  cout << "done" << endl;

  return 0;
}
