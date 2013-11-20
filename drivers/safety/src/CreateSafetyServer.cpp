#include <memory>
#include <thread>
#include <iostream>
#include <assert.h>
#include <string.h>

#include "zmq.h"

#include "CreateSafetyServer.h"
#include "SafetyMessage.h"
#include "ZMQHub.h"
#include "now.h"
#include "math.h"

using namespace std;

struct DelayedSafety : Safety
{
  bool m_safe;
  bool timerStarted;
  double timerValue;
  double delayOff;


  void safe(bool value) {
    if (!value) { // 
      if (m_safe) { // not already safe
	if (!timerStarted) { 
	  timerStarted = true;
	  timerValue = now()+delayOff;
	}
	if (now() >= timerValue) {
	  m_safe = false;
	  timerStarted=false;
	}
      }
    } else {
      m_safe = true;
      timerStarted = false;
    }
  }

  bool safe() const {
    return m_safe;
  }

  bool warn() const {
    return timerStarted;
  }

  DelayedSafety()
  {
    m_safe = false;
    timerStarted = false;
    timerValue  = 0;
    delayOff = 0;
  }
};

struct SafetyServer : DelayedSafety, ZMQHub
{
  std::vector < bool > safes;
  std::vector < double > timeouts;
  bool rx(ZMQSubscribeSocket &socket) 
  {
    double t=now();
    // who called
    size_t who = 0;
    while (who != rxSockets.size()) {
      if (&*rxSockets[who] == &socket) break;
      ++who;
    }
    if (who == rxSockets.size()) return false;

    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    SafetyMessage *data = (SafetyMessage *)msg.data();
    switch(data->messageId) {
    case SafetyMessage::SET_SAFE: 
      safes[who]=data->value; 
      timeouts[who]=t;
      break;
    }

    bool isSafe = true;
    for (size_t i = 0; i != rxSockets.size(); ++i) {
      if (!safes[i] || timeouts[i]+rxTimeout < t) isSafe = false;
    }
    safe(isSafe);
    return true;
  }

  bool tx(ZMQPublishSocket &socket) {
    bool ok = true;
    {
      ZMQMessage msg(sizeof(SafetyMessage));
      SafetyMessage *data = (SafetyMessage*)msg.data();
      data->messageId = SafetyMessage::GET_SAFE;
      data->value = safe();
      if (msg.send(socket) == 0) ok = false;
    }
    {
      ZMQMessage msg(sizeof(SafetyMessage));
      SafetyMessage *data = (SafetyMessage*)msg.data();
      data->messageId = SafetyMessage::GET_WARN;
      data->value = warn();
      if (msg.send(socket) == 0) ok = false;
    }
    return ok;
  }

  SafetyServer(const std::string &publish_, const std::vector < std::string > &subscribers_, float rate_, float delayOff_)
  { 
    rate=rate_;
    publish = publish_;
    subscribers=subscribers_;
    delayOff=delayOff_;

    txTimeout=2.0;
    rxTimeout=2.0;

    safes.clear();
    safes.resize(subscribers.size(),false);
    timeouts.clear();
    timeouts.resize(subscribers.size(),0.0);
    start();
  }

  ~SafetyServer()
  {
    stop();
    join();
  }
};

SafetySP CreateSafetyServer(const std::string &publish,const std::vector < std::string > &subscribers, float rate, float delayOff)
{
  SafetyServer *p = new SafetyServer(publish,subscribers,rate,delayOff);
  return SafetySP(p);
}
