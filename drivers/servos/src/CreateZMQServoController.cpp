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
#if SERVO_CURVE == 1
      if (i->second->curveMode) {
	{
	  ZMQMessage msg(sizeof(ZMQServoCurveMessage));
	  ZMQServoCurveMessage *data = (ZMQServoCurveMessage*)msg.data();
	
	  data->messageId = ZMQServoMessage::SET_CURVE;
	  data->servoId = i->first;
	  data->t0 = i->second->t0;
	  data->c0 = i->second->c0;
	  data->c1 = i->second->c1;
	  msg.send(socket);
	}
	{
	  ZMQMessage msg(sizeof(ZMQServoMessage));
	  ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	  
	  data->messageId = ZMQServoMessage::SET_TORQUE;
	  data->servoId = i->first;
	  data->value = i->second->goalTorque;
	  msg.send(socket);
	}
      } else {
	{
	  ZMQMessage msg(sizeof(ZMQServoMessage));
	  ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	  
	  data->messageId = ZMQServoMessage::SET_ANGLE;
	  data->servoId = i->first;
	  data->value = i->second->goalAngle;
	  msg.send(socket);
	}
	
	{
	  ZMQMessage msg(sizeof(ZMQServoMessage));
	  ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	  
	  data->messageId = ZMQServoMessage::SET_SPEED;
	  data->servoId = i->first;
	  data->value = i->second->goalSpeed;
	  msg.send(socket);
	}
	
	{
	  ZMQMessage msg(sizeof(ZMQServoMessage));
	  ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	  
	  data->messageId = ZMQServoMessage::SET_TORQUE;
	  data->servoId = i->first;
	  data->value = i->second->goalTorque;
	  msg.send(socket);
	}
      }
#else
      {
	ZMQMessage msg(sizeof(ZMQServoMessage));
	ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	
	data->messageId = ZMQServoMessage::SET_ANGLE;
	data->servoId = i->first;
	data->value = i->second->goalAngle;
	msg.send(socket);
      }

      {
	ZMQMessage msg(sizeof(ZMQServoMessage));
	ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	
	data->messageId = ZMQServoMessage::SET_SPEED;
	data->servoId = i->first;
	data->value = i->second->goalSpeed;
	msg.send(socket);
      }

      {
	ZMQMessage msg(sizeof(ZMQServoMessage));
	ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	
	data->messageId = ZMQServoMessage::SET_TORQUE;
	data->servoId = i->first;
	data->value = i->second->goalTorque;
	msg.send(socket);
      }
#endif
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

  ~ZMQServoController()
  {
    stop();
    join();
  }
};

ServoController* CreateZMQServoController(const std::string &me,const std::string &server)
{
  return new ZMQServoController(me,server);
}
