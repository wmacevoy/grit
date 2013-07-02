#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <thread>
#include <assert.h>
#include <string.h>
#include "zmq.h"

#include "CreateZMQServoController.h"
#include "ZMQServoMessage.h"


using namespace std;

const int TX_RATE = 50;

#define ZMQ_OK(CMD) { if ((CMD) != 0) { std::cout << "zmqerror: " << zmq_strerror(zmq_errno()) << endl; assert(false); } }

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


struct ZMQServo : Servo
{
  float goalAngle;
  float currentAngle;
  ZMQServo() : goalAngle(0), currentAngle(0) {}
  float angle() const { return currentAngle; }
  void angle(float value) { goalAngle = value; }
};

struct ZMQServoController : ServoController
{
  typedef std::map < int , ZMQServo* > Servos;
  Servos servos;

  std::thread *goRx;
  std::thread *goTx;

  std::string me;
  std::string server;

  bool running;

  void rx() {
    ZMQContext context;
    vector < shared_ptr <ZMQSubscribeSocket> > sockets;
    vector < zmq_pollitem_t > items;
    
    {
      ZMQSubscribeSocket *p = new ZMQSubscribeSocket(context,server);
      p->highWaterMark(100);
      sockets.push_back(shared_ptr<ZMQSubscribeSocket>(p));
    }
    
    items.resize(sockets.size());
    
    for (size_t i=0; i < items.size(); ++i) {
      items[i].socket = sockets[i]->me;
      items[i].events = ZMQ_POLLIN;
    }
    
    while (running) {

      if (zmq_poll(&items[0],items.size(),int(2.0*1000)) <= 0) continue;
      for (size_t i=0; i != sockets.size(); ++i) {
	if ((items[i].revents & ZMQ_POLLIN) != 0) {
	  ZMQMsg msg;
	  msg.recv(*sockets[i]);
	  ZMQServoMessage *data = (ZMQServoMessage *)msg.data();
	  Servos::iterator j = servos.find(data->servoId);
	  if (j != servos.end()) {
	    switch(data->messageId) {
	    case ZMQServoMessage::GET_ANGLE: 
	      j->second->currentAngle=data->value; break;
	    }
	  }
	  items[i].revents = 0;
	}
      }
    }
  }

  void tx() {
    ZMQContext context;
    ZMQPublishSocket socket(context,me);

    socket.highWaterMark(100);

    while (running) {
      usleep(int((1.0/TX_RATE)*1000000));
      size_t k=0, n=servos.size();
      for (Servos::iterator i = servos.begin(); i!=servos.end(); ++i) {
	ZMQMsg msg(sizeof(ZMQServoMessage));
	ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	
	data->messageId = ZMQServoMessage::SET_ANGLE;
	data->servoId = i->first;
	data->value = i->second->goalAngle;
	//	msg.send(txSocket,(++k < n) ? ZMQ_SNDMORE : 0);
	msg.send(socket);
      }
    }
  }

  ZMQServoController(const std::string &me_, const std::string &server_) 
    : me(me_), server(server_)
  { 
    running = false;
  }

  void start() { 
    if (running == false) {
      running = true;
      goTx = new std::thread(&ZMQServoController::tx,this);
      goRx = new std::thread(&ZMQServoController::rx,this);
    }
  }

  ~ZMQServoController() { 
    if (running) {
      running=false;
      goTx->join(); delete goTx;
      goRx->join(); delete goRx; 
    }
  }

  Servo* servo(int id)
  {
    Servos::iterator i = servos.find(id);
    if (i != servos.end())  return i->second;
    assert(running == false); // no new servos after start()
    return servos[id] = new ZMQServo();
  }
};

ServoController* CreateZMQServoController(const std::string &me,const std::string &server)
{
  return new ZMQServoController(me,server);
}
