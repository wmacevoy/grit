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
#include "ZMQHub.h"
#include "ZMQServo.h"

struct ZMQServoController : ServoController, ZMQHub
{
  typedef std::map < int , ZMQServo* > Servos;
  Servos servos;

  void start()
  {
    ServoController::start();
    ZMQHub::start();
  }

  void rx(ZMQSubscribeSocket &socket) 
  {
    ZMQMessage msg;
    msg.recv(socket);
    ZMQServoMessage *data = (ZMQServoMessage *)msg.data();
    Servos::iterator j = servos.find(data->servoId);
    if (j != servos.end()) {
      switch(data->messageId) {
      case ZMQServoMessage::GET_ANGLE: 
	j->second->currentAngle=data->value; break;
      }
    }
  }

  void tx(ZMQPublishSocket &socket) {
    for (Servos::iterator i = servos.begin(); i!=servos.end(); ++i) {
      ZMQMessage msg(sizeof(ZMQServoMessage));
      ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
      
      data->messageId = ZMQServoMessage::SET_ANGLE;
      data->servoId = i->first;
      data->value = i->second->goalAngle;
      msg.send(socket);
    }
  }

  ZMQServoController(const std::string &me, const std::string &server) 
  { 
    publish = me;
    subscribers.push_back(server);
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
