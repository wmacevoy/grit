#include <ctime>
#include <csignal>
#include <zmq.hpp>
#include <map>
#include <thread>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "zmq.hpp"
#include "ZMQServo.h"
#include "ZMQServoMessage.h"
#include "FakeServoController.h"
#include "FakeServo.h"
#include "Servo.h"

#define ZMQ_OK(CMD) { if ((CMD) != 0) { std::cout << "zmqerror: " << zmq_strerror(zmq_errno()) << endl; assert(false); } }

using namespace std;

bool running = true;
unsigned rxs = 0;
unsigned txs = 0;


FakeServo NO_SERVO;

class ZMQSocket : public zmq::socket_t
{
public:
  ZMQSocket(zmq::context_t &context, int type) 
    :  zmq::socket_t(context,type) 
  {}

  void highWaterMark(int value) {
    setsockopt(ZMQ_SNDHWM,&value,sizeof(value));
  }
};

class ZMQSubscribeSocket : public ZMQSocket
{
public:
  ZMQSubscribeSocket(zmq::context_t &context) 
    : ZMQSocket(context,ZMQ_SUB)
  {
    setsockopt(ZMQ_SUBSCRIBE, "", 0);

  }
};

class ZMQPublishSocket : public ZMQSocket
{
public:
  ZMQPublishSocket(zmq::context_t &context, const std::string &address)
    : ZMQSocket(context, ZMQ_PUB)
  {
    bind(address.c_str());
  }
};

class ZMQMsg : public zmq::message_t
{
public:
  ZMQMsg(size_t size) : zmq::message_t(size) {}
  ZMQMsg(const void *source, size_t size) : zmq::message_t(size) {
    memcpy(data(),source,size);
  }
  int recv(zmq::socket_t &socket, int arg=0) { return socket.recv(this,arg); }
  int recv_timeout(ZMQSocket &socket, int millisecondTimeout) { 
    zmq_pollitem_t items[1];
    items[0].socket = socket;
    items[0].events = ZMQ_POLLIN;
    if (zmq::poll (items, 1, millisecondTimeout) > 0) {
      return socket.recv(this,ZMQ_NOBLOCK);
    } else {
      return -1;
    }
  }

  int send(ZMQSocket &socket, int arg=0) { return socket.send(this,arg); }
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
  zmq::context_t context;
  ZMQSubscribeSocket socket(context);
  socket.highWaterMark(100);

  while (running) {
    ++rxs;
    ZMQMsg msg;
    if (msg.recv_timeout(socket,int(1.0*1000)) != 0) continue; // ) != 0) { usleep(int(0.25*1000000)); continue; }
    ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
    switch(data->messageId) {
    case ZMQServoMessage::SET_ANGLE: servo(data)->angle(data->value); break;
    }
  }
}


void tx() {
  zmq::context_t context;
  ZMQPublishSocket socket(context,"tcp://*:5500");
  socket.highWaterMark(1);

  while (running) {
    ++txs;
    usleep(int(0.02*1000000));
    size_t k = 0, n = servos.size();
    for (Servos::iterator i = servos.begin();
	 i != servos.end();
	 ++i) {
      ZMQServoMessage data;
      data.messageId = ZMQServoMessage::GET_ANGLE;
      data.servoId = i->first;
      data.value = i->second->angle();

      ZMQMsg msg(&data,sizeof(data));
      msg.send(socket,(++k < n) ? ZMQ_SNDMORE : 0);
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

FakeServoController fake;

const struct { ServoController *controller; int id; } SERVO_MAP [] =
  {
  //{ controller, id },
    { &fake, 1  },
    { &fake, 10 },
    { &fake, 21 },
    { 0,    0} // end
  };

void SigIntHandler(int arg) { 
  running = false; 
}


int main(int argc,char **argv) {
  signal(SIGINT, SigIntHandler);

  for (int i=0; SERVO_MAP[i].id != 0; ++i) {
    servos[SERVO_MAP[i].id]=SERVO_MAP[i].controller->servo(SERVO_MAP[i].id);
  }

  std::thread reportThread(report);
  std::thread txThread(tx);
  std::thread rxThread(rx);
  
  rxThread.join();
  txThread.join();
  reportThread.join();

  cout << "done" << endl;

  return 0;
}
