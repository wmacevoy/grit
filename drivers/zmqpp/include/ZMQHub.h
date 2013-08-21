#include <vector>
#include <string>
#include <thread>
#include <memory>

#include "ZMQContext.h"
#include "ZMQSocket.h"
#include "ZMQPublishSocket.h"
#include "ZMQSubscribeSocket.h"
#include "ZMQMessage.h"

class ZMQHub
{
 public:
  std::vector < std::string > subscribers;
  std::string publish;

  int rate;
  volatile bool running;
  int highWaterMark;
  int rxCount,txCount;
  float rxRate,txRate;

  std::thread *goRx;
  std::thread *goTx;
  std::thread *goReport;

  virtual void start();
  virtual void join();
  virtual void stop();
  virtual void rxLoop();
  virtual void txWait();
  virtual void txLoop();
  virtual void reportLoop();

  ZMQHub();

  virtual void report();
  virtual void rx(ZMQSubscribeSocket &socket)=0;
  virtual void tx(ZMQPublishSocket &socket)=0;
  virtual ~ZMQHub();
};
