#include "CreateSafetyClient.h"
#include "SafetyMessage.h"
#include "ZMQHub.h"
#include "now.h"
#include "math.h"
#include <iostream>

using namespace std;

struct SafetyClient : ZMQHub, Safety
{
  bool getSafe;
  bool setSafe;
  bool getWarn;
  bool getStale;

  void safe(bool value)
  {
    setSafe = value;
  }

  bool safe() const
  {
    return getSafe;
  }

  bool stale() const
  {
    return getStale;
  }

  bool warn() const
  {
    return getWarn;
  }

  bool rx(ZMQSubscribeSocket &socket) 
  {
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    SafetyMessage *data = (SafetyMessage *)msg.data();
    switch(data->messageId) {
    case SafetyMessage::GET_SAFE: getSafe = data->value; break;
    case SafetyMessage::GET_WARN: getWarn = data->value; break;
    case SafetyMessage::GET_STALE: getStale = data->value; break;
    }
    return true;
  }

  bool tx(ZMQPublishSocket &socket) {
    bool ok = true;
    ZMQMessage msg(sizeof(SafetyMessage));
    SafetyMessage *data = (SafetyMessage*)msg.data();
    data->messageId = SafetyMessage::SET_SAFE;
    data->value = setSafe;
    if (msg.send(socket) == 0) ok = false;
    return ok;
  }

  SafetyClient(const std::string &me, const std::string &server, int rate_)
  { 
    setSafe = false;
    getSafe = false;
    getWarn = false;
    getStale = false;
    rate=rate_;
    publish = me;
    subscribers.push_back(server);
    start();
  }

  ~SafetyClient()
  {
    stop();
    join();
  }
};

SafetySP CreateSafetyClient(const std::string &me,const std::string &server, int rate)
{
  SafetyClient *p = new SafetyClient(me,server,rate);
  return SafetySP(p);
}
