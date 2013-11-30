#include <csignal>
#include <map>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <thread>
#include <chrono>

#include "Configure.h"
#include "ZMQServoMessage.h"
#include "ZMQHub.h"
#include "now.h"
#include <map>
#include <memory>

Configure cfg;
bool verbose;

class ZMQProxyServo 
{
private:
  int m_id;

  bool m_angleSet;
  float m_angle;
  double m_angleTimeout;
  float m_angleRate;

  bool m_tempSet;
  float m_temp;
  double m_tempTimeout;
  float m_tempRate;

public:
  int id() const { return m_id; }
  void id(int value) { m_id=value; }

  float angleRate() const { return m_angleRate; }
  void angleRate(float value) { 
    m_angleRate = value;
    if (m_angleRate > 0) {
      m_angleTimeout = now() + 1.0/m_angleRate;
    } else {
      m_angleTimeout = now() + 1e9;
    }
  }

  float angle() const { return m_angle; }
  void angle(float value) {
    m_angle = value;
    m_angleSet = true;
  }

  double tempTimeout() const { return m_tempTimeout; }

  float tempRate() const { return m_tempRate; }
  void tempRate(float value) {
    m_tempRate = value;
    if (m_tempRate > 0) {
      m_tempTimeout = now() + 1.0/m_tempRate;
    } else {
      m_tempTimeout = now() + 1e9;
    }
  }

  float temp() const { return m_temp; }
  void temp(float value) {
    m_temp = value;
    m_tempSet = true;
  }

  ZMQProxyServo()
  {
    m_id=0;

    m_angle = 0;
    m_angleSet = false;
    m_angleRate = 0;
    m_angleTimeout = now() + 1e9;

    m_temp = 0;
    m_tempSet = false;
    m_tempRate = 0;
    m_tempTimeout = now() + 1e9;
  }

  bool txAngle(ZMQPublishSocket &socket)
  {
    bool ok=true;
    if (m_angleSet && (m_angleRate > 0 && (now() > m_angleTimeout))) {
      m_angleTimeout = now() + 1.0/m_angleRate;
      ZMQMessage msg(sizeof(ZMQServoMessage));
      ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
      data->messageId = ZMQServoMessage::GET_ANGLE;
      data->servoId = m_id;
      data->value = m_angle;
      if (msg.send(socket) == 0) ok = false;
      if (verbose) {
	std::cout << "tx id=" << m_id << " angle=" << m_angle << (ok?" ok":" fail")  << std::endl;
      }
    }
    return ok;
  }

  bool txTemp(ZMQPublishSocket &socket)
  {
    bool ok=true;
    if (m_tempSet && (m_tempRate > 0 && (now() > m_tempTimeout))) {
      m_tempTimeout = now() + 1.0/m_tempRate;
      ZMQMessage msg(sizeof(ZMQServoMessage));
      ZMQServoMessage *data = (ZMQServoMessage*)msg.data();
      data->messageId = ZMQServoMessage::GET_ANGLE;
      data->servoId = m_id;
      data->value = m_temp;
      if (msg.send(socket) == 0) ok = false;
      if (verbose) {
	std::cout << "tx temp=" << m_temp << (ok?" ok":" fail")  << std::endl;
      }
    }
    return ok;
  }

  bool tx(ZMQPublishSocket &socket)
  {
    bool ok = true;
    ok = txAngle(socket) && ok;
    ok = txTemp(socket)  && ok;
    return ok;
  }
};

class ProxyServoListener : public ZMQHub
{
public:
  typedef std::map < int , ZMQProxyServo > Servos;
  Servos servos;

  bool rx(ZMQSubscribeSocket &socket) 
  {
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    ZMQServoMessage *data = (ZMQServoMessage *)msg.data();

    Servos::iterator j = servos.find(data->servoId);
    if (j != servos.end()) {
      switch(data->messageId) {
      case ZMQServoMessage::GET_ANGLE: 
	j->second.angle(data->value); break;
      case ZMQServoMessage::GET_TEMP:
	j->second.temp(data->value); break;
      }
    }
    return true;
  }
    
  bool tx(ZMQPublishSocket &socket)
  {
    bool ok = true;
    for (Servos::iterator i=servos.begin(); i!=servos.end(); ++i) {
      if (!i->second.tx(socket)) ok = false;
    }
    return ok;
  }

  ProxyServoListener()
  {
    subscribers.push_back(cfg.str("servos.subscribe"));
    publish=cfg.str("proxyservos.publish");

    std::set < std::string > names = cfg.servoNames();
    for (std::set < std::string > :: iterator i = names.begin(); i != names.end(); ++i) {
      int id = atoi(cfg.servo(*i,"id").c_str());
      ZMQProxyServo &servo=servos[id];
      servo.id(id);
      servo.tempRate(cfg.num("proxyservos.temprate"));
      servo.angleRate(cfg.num("proxyservos.anglerate"));
    }
    start();
  }
};

typedef std::shared_ptr < ProxyServoListener > ProxyServoListenerSP;

ProxyServoListenerSP server;

void quit(int arg)
{
  server.reset();
}

int main(int argc, char *argv[])
{
  cfg.path("../../setup");
  cfg.args("proxyservos.",argv);
  if (argc == 1) cfg.load("config.csv");
  cfg.servos();
  verbose = cfg.flag("proxyservos.verbose",false);
  if (verbose) cfg.show();


  server = ProxyServoListenerSP(new ProxyServoListener());

  signal(SIGINT, quit);
  signal(SIGTERM, quit);
  signal(SIGQUIT, quit);

  while (server) {
    usleep(int(0.25*1e6));
  }

  std::cout << "done" << std::endl;
  return 0;
}
