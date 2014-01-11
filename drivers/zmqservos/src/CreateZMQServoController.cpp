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
#include "now.h"
#include "math.h"

struct ZMQServoController : ZMQHub, ServoController
{
  typedef std::map < int , ZMQServo* > Servos;
  Servos servos;
  double txTime;

  void start()
  {
    ServoController::start();
    ZMQHub::start();
    txTime = 0;
  }

  bool rx(ZMQSubscribeSocket &socket) 
  {
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    ZMQServoMessage *data = (ZMQServoMessage *)msg.data();
    Servos::iterator j = servos.find(data->servoId);
    if (j != servos.end()) {
      switch(data->messageId) {
      case ZMQServoMessage::GET_ANGLE: 
	j->second->angle(data->value); break;
      case ZMQServoMessage::GET_TEMP:
	j->second->temp(data->value); break;
      }
    }
    return true;
  }

  bool tx(ZMQPublishSocket &socket) {
    double lastTxTime = txTime;
    bool ok = true;
    txTime = now();
    for (Servos::iterator i = servos.begin(); i!=servos.end(); ++i) {
      if (i->second->curveMode) {
	{
	  ZMQMessage msg(sizeof(ZMQServoCurveMessage));
	  ZMQServoCurveMessage *data = (ZMQServoCurveMessage*)msg.data();
	
	  data->messageId = ZMQServoMessage::SET_CURVE;
	  data->servoId = i->first;
	  data->t[0] = i->second->t[0];
	  data->t[1] = i->second->t[1];
	  data->c0[0] = i->second->c0[0];
	  data->c0[1] = i->second->c0[1];
	  data->c0[2] = i->second->c0[2];
	  data->c1[0] = i->second->c1[0];
	  data->c1[1] = i->second->c1[1];
	  data->c1[2] = i->second->c1[2];
	  if (msg.send(socket) == 0) ok = false;
	}
      } else {
	{
	  ZMQMessage msg(sizeof(ZMQServoMessage));
	  ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	  
	  data->messageId = ZMQServoMessage::SET_ANGLE;
	  data->servoId = i->first;
	  data->value = i->second->goalAngle;
	  if (msg.send(socket) == 0) ok = false;
	}
	
	{
	  ZMQMessage msg(sizeof(ZMQServoMessage));
	  ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	  
	  data->messageId = ZMQServoMessage::SET_SPEED;
	  data->servoId = i->first;
	  data->value = i->second->goalSpeed;
	  if (msg.send(socket) == 0) ok = false;
	}
      }

      // send these messages only once per second
      if (txTime == 0 || floor(lastTxTime) != floor(txTime)) {
	{
	  {
	    ZMQMessage msg(sizeof(ZMQServoMessage));
	    ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	  
	    data->messageId = ZMQServoMessage::SET_TORQUE;
	    data->servoId = i->first;
	    data->value = i->second->goalTorque;
	    if (msg.send(socket) == 0) ok = false;
	  }
	  {
	    ZMQMessage msg(sizeof(ZMQServoMessage));
	    ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	  
	    data->messageId = ZMQServoMessage::SET_RATE;
	    data->servoId = i->first;
	    data->value = i->second->goalRate;
	    if (msg.send(socket) == 0) ok = false;
	  }
	}
      }
    }
    return ok;
  }

  ZMQServoController(const std::string &me, const std::string &server,int rate_) 
  { 
    rate=rate_;
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

ServoController* CreateZMQServoController(const std::string &me,const std::string &server, int rate)
{
  return new ZMQServoController(me,server,rate);
}
