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
  double rxOk,txOk;
  float rxTimeout,txTimeout;
  float rxPollTimeout;
  typedef std::vector < std::shared_ptr < ZMQSubscribeSocket > > RxSockets;
  typedef std::vector < zmq_pollitem_t > RxPollItems;
  RxSockets rxSockets;
  RxPollItems rxPollItems;
  typedef std::shared_ptr < ZMQPublishSocket > TxSocket;
  TxSocket txSocket;
  std::shared_ptr < ZMQContext > rxContext;
  std::shared_ptr < ZMQContext > txContext;

  std::thread *goRx;
  std::thread *goTx;
  std::thread *goReport;

  virtual void start();
  virtual void join();
  virtual void stop();
  virtual void rxOpen();
  virtual void rxClose();

  virtual void rxLoop();
  virtual void txOpen();
  virtual void txClose();

  virtual void txWait();
  virtual void txLoop();
  virtual void reportLoop();

  ZMQHub();

  virtual void report();
  virtual bool rx(ZMQSubscribeSocket &socket)=0;
  virtual bool tx(ZMQPublishSocket &socket)=0;
  virtual ~ZMQHub();
};
