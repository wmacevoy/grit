#include <vector>
#include <string>
#include <thread>
#include <memory>

#include "ZMQContext.h"
#include "ZMQSocket.h"
#include "ZMQSubscribeSocket.h"
#include "ZMQMessage.h"

class ZMQRx
{
 public:
  std::vector < std::string > subscribers;

  volatile bool running;
  int highWaterMark;
  int rxCount;
  float rxRate;

  std::thread *goRx;

  virtual void start();
  virtual void join();
  virtual void stop();
  virtual void rxLoop();

  ZMQRx();

  virtual void rx(ZMQSubscribeSocket &socket)=0;
  virtual ~ZMQRx();
};
