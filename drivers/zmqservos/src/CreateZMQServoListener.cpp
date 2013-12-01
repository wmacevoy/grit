#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <thread>
#include <assert.h>
#include <string.h>

#include "zmq.h"

#include "CreateZMQServoListener.h"
#include "ZMQServoMessage.h"
#include "ZMQHub.h"
#include "ZMQServo.h"
#include "now.h"
#include "math.h"

struct ZMQServoListener : ZMQHub, ServoController
{
  typedef std::map < int , ZMQServo* > Servos;
  Servos servos;

  void start()
  {
    ServoController::start();
    ZMQHub::start();
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
	j->second->presentAngle=data->value; break;
      case ZMQServoMessage::GET_TEMP:
	j->second->presentTemp=data->value; break;
      }
    }
    std::cout << "got id=" << data->messageId << " value=" << data->value << std::endl;
    return true;
  }

  bool tx(ZMQPublishSocket &socket) {
    return true;
  }

  ZMQServoListener(const std::string &server,int rate_) 
  { 
    rate=rate_;
    publish = "";
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

ServoController* CreateZMQServoListener(const std::string &server)
{
  return new ZMQServoListener(server,1);
}
