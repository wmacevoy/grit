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
  bool m_stale;
  bool timerStarted;
  double timerValue;
  double delayOff;

  void stale(bool value) {
    m_stale = value;
  }

  bool stale() const {
    return m_stale;
  }

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
  Configure &cfg;
  bool verbose;
  std::vector < bool > safes;
  std::vector < double > timeouts;
  bool rx(ZMQSubscribeSocket &socket) { assert(false); return false; }

  bool rx(ZMQSubscribeSocket &socket, size_t who) 
  {
    double t=now();

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

  void report()
  {
    bool newStale = false;
    if (verbose) {
      string color;
      if (safe()) {
	if (warn()) {
	  color = "yellow";
	} else {
	  color = "green";
	}
      } else {
	color = "red";
      }
      double t = now();
      cout << "safety: time=" << t << " color=" << color;
      for (size_t i=0; i<subscribers.size(); ++i) {
	if (timeouts[i]+rxTimeout < t) {
	  cout << " " << subscribers[i] << " stale";
	  newStale = true;
	} else {
	  if (safes[i]) {
	    cout << " " << subscribers[i] << " safe";
	  } else {
	    cout << " " << subscribers[i] << " unsafe";
	  }
	}
      }
      cout << endl;
      stale(newStale);
    }
  }

  SafetyServer(Configure &cfg_)
    : cfg(cfg_)
  { 
    rate=cfg.num("safety.rate");
    publish=cfg.str("safety.publish");
    subscribers=cfg.list("safety.subscribers");
    delayOff=cfg.num("safety.delayoff");
    verbose=cfg.flag("safety.verbose",false);
    txTimeout=cfg.num("safety.txtimeout");
    rxTimeout=cfg.num("safety.rxtimeout");

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

SafetySP CreateSafetyServer(Configure &cfg)
{
  SafetyServer *p = new SafetyServer(cfg);
  return SafetySP(p);
}
