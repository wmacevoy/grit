#include <csignal>
#include <map>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <map>
#include <memory>
#include <string.h>

#include "Configure.h"
#include "SensorsMessage.h"
#include "ZMQHub.h"
#include "ZMQMessage.h"
#include "now.h"
#include "Lock.h"

Configure cfg;
bool verbose;

class ProxySensors : public ZMQHub
{
private:
  mutable std::mutex access;
  bool m_messageSet;
  SensorsMessage m_message;
  double m_messageTimeout;
  float m_messageRate;
  
public:
  double messageTimeout() const { return m_messageTimeout; }
  float messageRate() const { return m_messageRate; }
  void messageRate(float value) {
    m_messageRate = value;
    if (m_messageRate > 0) {
      m_messageTimeout = now() + 1.0/m_messageRate;
    } else {
      m_messageTimeout = now() + 1e9;
    }
  }

  void messageTo(SensorsMessage &dest) const { 
    Lock lock(access);
    memcpy(&dest,&m_message,sizeof(SensorsMessage));
  }

  void messageFrom(const SensorsMessage &src) { 
    Lock lock(access);
    memcpy(&m_message,&src,sizeof(SensorsMessage));
    m_messageSet = true;
  }

  bool rx(ZMQSubscribeSocket &socket) 
  {
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    SensorsMessage *data = (SensorsMessage *)msg.data();
    messageFrom(*data);
    return true;
  }

  bool tx(ZMQPublishSocket &socket)
  {
    bool ok=true;
    if (m_messageSet && (m_messageRate > 0 && (now() > m_messageTimeout))) {
      m_messageTimeout = now() + 1.0/m_messageRate;
      ZMQMessage msg(sizeof(SensorsMessage));
      SensorsMessage *data = (SensorsMessage*)msg.data();
      messageTo(*data);
      if (msg.send(socket) == 0) ok = false;
    }
    return ok;
  }

  ProxySensors()
  {
    memset(&m_message,0,sizeof(SensorsMessage));
    m_messageSet = false;
    m_messageRate = 0;
    m_messageTimeout = now() + 1e9;

    subscribers.push_back(cfg.str("sensors.subscribe"));
    publish=cfg.str("proxysensors.publish");
    messageRate(cfg.num("proxysensors.rate"));

    start();
  }
};

typedef std::shared_ptr < ProxySensors > ProxySensorsSP;

ProxySensorsSP server;

void quit(int arg)
{
  server.reset();
}

int main(int argc, char *argv[])
{
  cfg.path("../../setup");
  cfg.args("proxysensors.",argv);
  if (argc == 1) cfg.load("config.csv");
  cfg.servos();
  verbose = cfg.flag("proxysensors.verbose",false);
  if (verbose) cfg.show();

  server = ProxySensorsSP(new ProxySensors());

  signal(SIGINT, quit);
  signal(SIGTERM, quit);
  signal(SIGQUIT, quit);

  while (server) {
    usleep(int(0.25*1e6));
  }

  std::cout << "done" << std::endl;
  return 0;
}
