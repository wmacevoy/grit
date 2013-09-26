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

struct ZMQServoController : ServoController, ZMQHub
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
    double lastTxTime = txTime;
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
	    msg.send(socket);
	  }
	  {
	    ZMQMessage msg(sizeof(ZMQServoMessage));
	    ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
	  
	    data->messageId = ZMQServoMessage::SET_RATE;
	    data->servoId = i->first;
	    data->value = i->second->goalRate;
	    msg.send(socket);
	  }
	}
      }
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
