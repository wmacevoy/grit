#include <ctime>
#include <csignal>
#include <zmq.h>
#include <map>
#include <thread>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#include "zmq.hpp"
#include "ZMQServo.h"
#include "ZMQServoMessage.h"
#include "CreateFakeServoController.h"
#include "CreateDynamixelServoController.h"
#include "FakeServo.h"
#include "Servo.h"

#define ZMQ_OK(CMD) { if ((CMD) != 0) { std::cout << "zmqerror: " << zmq_strerror(zmq_errno()) << endl; assert(false); } }

using namespace std;

FakeServo NO_SERVO;

struct Controllers
{
  shared_ptr<ServoController> fake;
  shared_ptr<ServoController> dynamixel;
  vector < shared_ptr<ServoController> > all;
  Controllers() :
    fake(CreateFakeServoController()),
    dynamixel(CreateDynamixelServoController())
  {
    all.push_back(fake);
    all.push_back(dynamixel);
  }
  void start()
  {
    for (size_t i=0; i<all.size(); ++i) all[i]->start();
  }
} ctrl;

const int TX_RATE=50;


const struct { ServoController *controller; int id; } SERVOS [] =
  {
  //{ controller, id },
#if 0
    { &*ctrl.dynamixel, 1  },
    { &*ctrl.fake, 2  },
    { &*ctrl.fake, 10 },
    { &*ctrl.fake, 20 },
#endif
#if 1
    { &*ctrl.fake, 11  },
    { &*ctrl.fake, 12  },
    { &*ctrl.fake, 13  },
    { &*ctrl.fake, 21  },
    { &*ctrl.fake, 22  },
    { &*ctrl.fake, 23  },
    { &*ctrl.fake, 31  },
    { &*ctrl.fake, 32  },
    { &*ctrl.fake, 33  },
    { &*ctrl.fake, 41  },
    { &*ctrl.fake, 42  },
    { &*ctrl.fake, 43  },
    { &*ctrl.fake, 91  },
    { &*ctrl.dynamixel, 93  },
    { &*ctrl.fake, 94  },
#endif
    { 0,    0} // end
  };

static const char * SUBSCRIBERS [] = 
  {
    "tcp://localhost:5501", /* TestZMQServerController */
    "tcp://192.168.2.135:5502", /* RevolutionTest */
    0 // end
  };

const char *PUBLISH = "tcp://*:5500";

bool running = true;
unsigned rxs = 0;
unsigned txs = 0;



class ZMQContext
{
public:
  void *me;
  ZMQContext() { me = zmq_ctx_new(); }
  ~ZMQContext() { zmq_ctx_destroy(me); }
};

class ZMQSocket 
{
public:
  void *me;
  ZMQSocket() { me = 0; }
  virtual ~ZMQSocket() { if (me != 0) ZMQ_OK(zmq_close(me)); }
  void highWaterMark(int value) {
    ZMQ_OK(zmq_setsockopt(me,ZMQ_SNDHWM,&value,sizeof(value)));
  }
};

class ZMQSubscribeSocket : public ZMQSocket
{
public:
  ZMQSubscribeSocket(ZMQContext &context, const std::string &address)
  {
    me=zmq_socket(context.me, ZMQ_SUB);
    assert(me != 0);
    assert(zmq_setsockopt(me, ZMQ_SUBSCRIBE, "", 0) == 0);
    assert(zmq_connect(me,address.c_str()) == 0);
  }
};

class ZMQPublishSocket : public ZMQSocket
{
public:
  ZMQPublishSocket(ZMQContext &context, const std::string &address)
  {
    me=zmq_socket(context.me, ZMQ_PUB);
    ZMQ_OK(zmq_bind(me,address.c_str()));
  }
};

class ZMQMsg
{
public:
  zmq_msg_t me;
  ZMQMsg() { ZMQ_OK(zmq_msg_init (&me)); }
  ZMQMsg(size_t size) { ZMQ_OK(zmq_msg_init_size(&me,size)); }
  ZMQMsg(const void *data, size_t size) { 
    ZMQ_OK(zmq_msg_init_size(&me,size)); 
    memcpy(zmq_msg_data(&me),data,size);
  }
  void *data() { return zmq_msg_data(&me); }
  int recv(ZMQSocket &socket, int arg=0) { 
    return zmq_recvmsg(socket.me,&me,arg); 
  }
  int recv_timeout(ZMQSocket &socket, int millisecondTimeout) { 
    zmq_pollitem_t items[1];
    items[0].socket = socket.me;
    items[0].events = ZMQ_POLLIN;
    if (zmq_poll (items, 1, millisecondTimeout) > 0) {
      return zmq_recvmsg(socket.me,&me,ZMQ_NOBLOCK);
    } else {
      return -1;
    }
  }

  int send(ZMQSocket &socket, int arg=0) { return zmq_sendmsg(socket.me,&me,arg); }
  ~ZMQMsg() { ZMQ_OK(zmq_msg_close (&me)); }
};

typedef std::map<int,Servo*> Servos;
Servos servos;

Servo* servo(ZMQServoMessage *message)
{
  Servos::iterator i = servos.find(message->servoId);
  if (i != servos.end()) return i->second;
    
  cout << "zmqservoserver: !!! request for unknown servo id: " << message->servoId << endl;
  return &NO_SERVO;
}


void rx() {
  ZMQContext context;
  vector < shared_ptr <ZMQSubscribeSocket> > sockets;
  vector < zmq_pollitem_t > items;

  for (size_t i=0; SUBSCRIBERS[i] != 0; ++i) {
    ZMQSubscribeSocket *p = new ZMQSubscribeSocket(context,SUBSCRIBERS[i]);
    p->highWaterMark(100);
    sockets.push_back(shared_ptr<ZMQSubscribeSocket>(p));
  }
  
  items.resize(sockets.size());

  for (size_t i=0; i < items.size(); ++i) {
    items[i].socket = sockets[i]->me;
    items[i].events = ZMQ_POLLIN;
  }

  //  ZMQMsg msg(sizeof(ZMQServoMessage));
  //  ZMQServoMessage *data = (ZMQServoMessage *)msg.data();

  while (running) {
    ++rxs;
    if (zmq_poll(&items[0],items.size(),int(1.0*1000)) <= 0) continue;
    for (size_t i=0; i != sockets.size(); ++i) {
      if ((items[i].revents & ZMQ_POLLIN) != 0) {
	ZMQMsg msg;
	msg.recv(*sockets[i]);
	ZMQServoMessage *data = (ZMQServoMessage *)msg.data();
	//	cout << "msg id=" << data->messageId << " servo=" << data->servoId << " value=" << data->value << endl;
	switch(data->messageId) {
	case ZMQServoMessage::SET_ANGLE: servo(data)->angle(data->value); break;
	}
	items[i].revents = 0;
      }
    }
  }
}


void tx() {
  ZMQContext context;
  ZMQPublishSocket socket(context,PUBLISH);
  socket.highWaterMark(1);

  while (running) {
    ++txs;
    usleep(int(double(1.0/TX_RATE)*1000000));
    for (Servos::iterator i = servos.begin();
	 i != servos.end();
	 ++i) {

      ZMQMsg msg(sizeof(ZMQServoMessage));
      ZMQServoMessage *data = (ZMQServoMessage*)msg.data();

      data->messageId = ZMQServoMessage::GET_ANGLE;
      data->servoId = i->first;
      data->value = i->second->angle();
      msg.send(socket);
    }
  }
}


void report() {
  const double dt = 1.0;
  while (running) {
    usleep(int(dt*1000000));
    cout << "report: ";
    cout << " tx/s=" << txs/dt;
    cout << " rx/s=" << rxs/dt;
    for (Servos::iterator i = servos.begin();
	 i != servos.end();
	 ++i) {
      cout << " angle[" << i->first << "]=" << i->second->angle();
    }
    cout << endl;
    txs = 0;
    rxs = 0;
  }
}


void SigIntHandler(int arg) { 
  running = false; 
}


int main(int argc,char **argv) {
  signal(SIGINT, SigIntHandler);

  for (int i=0; SERVOS[i].id != 0; ++i) {
    servos[SERVOS[i].id]=SERVOS[i].controller->servo(SERVOS[i].id);
  }

  ctrl.start(); // start all controllers

  std::thread reportThread(report);
  std::thread txThread(tx);
  std::thread rxThread(rx);
  
  rxThread.join();
  txThread.join();
  reportThread.join();

  cout << "done" << endl;

  return 0;
}
