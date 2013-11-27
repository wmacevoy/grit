#include <ctime>
#include <csignal>
#include <map>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <signal.h>

#include <thread>

#include <vector>
#include <zmq.h>

#include "zmq.h"

#include "ZMQHub.h"

#include "ZMQContext.h"
#include "ZMQPublishSocket.h"
#include "ZMQSubscribeSocket.h"
#include "ZMQMessage.h"
#include "ZMQ_OK.h"
#include "now.h"

using namespace std;

ZMQHub::ZMQHub()
{
  rate = 10;
  highWaterMark = 1;
  running = false;
  goTx = 0;
  goRx = 0;
  goReport = 0;
  txCount=0;
  rxCount=0;
  txRate = 0.0/1.0;
  rxRate = 0.0/1.0;
  rxTimeout=4.0;
  rxPollTimeout=0.25;
  txTimeout=4.0;
}

void ZMQHub::rxClose()
{
  rxSockets.clear();
  rxPollItems.clear();
  rxContext.reset();
}

void ZMQHub::rxOpen()
{
  rxClose();

  cout << "ZMQHub::rxOpen(this=" << ((void*)this);
  for (size_t i=0; i<subscribers.size(); ++i) {
    cout << "," << subscribers[i];
  }
  cout << ")" << endl;

  rxContext = std::shared_ptr < ZMQContext > (new ZMQContext());

  for (size_t i=0; i<subscribers.size(); ++i) {
    ZMQSubscribeSocket *p = new ZMQSubscribeSocket(*rxContext,subscribers[i].c_str());
    p->highWaterMark(highWaterMark);
    rxSockets.push_back(shared_ptr<ZMQSubscribeSocket>(p));
  }
  
  rxPollItems.resize(rxSockets.size());
  
  for (size_t i=0; i < rxPollItems.size(); ++i) {
    rxPollItems[i].socket = rxSockets[i]->me;
    rxPollItems[i].events = ZMQ_POLLIN;
  }
  rxOk = now();
}

bool ZMQHub::rx(ZMQSubscribeSocket &socket, size_t who)
{
  return rx(socket);
}

void ZMQHub::rxLoop() 
{
  rxOpen();
  while (running) {
    if (rxOk+rxTimeout < now()) rxOpen();
    if (zmq_poll(&rxPollItems[0],rxPollItems.size(),int(rxPollTimeout*1000)) <= 0) continue;
    if (!running) break;
    bool ok = true;
    for (size_t i=0; i != rxSockets.size(); ++i) {
      if ((rxPollItems[i].revents & ZMQ_POLLIN) != 0) {
	++rxCount;
	if (!rx(*rxSockets[i],i)) ok = false;
	rxPollItems[i].revents = 0;
      }
    }
    if (ok) rxOk=now();
  }
  rxClose();
}

void ZMQHub::txWait()
{
	std::this_thread::sleep_for(std::chrono::microseconds(int((1.0/rate)*1000000)));
}

void ZMQHub::txClose()
{
  txSocket.reset();
  txContext.reset();
}

void ZMQHub::txOpen()
{
  txClose();
  cout << "ZMQHub::txOpen(this=" << ((void*)this) << "," << publish << ")" << endl;;

  txContext = std::shared_ptr < ZMQContext > ( new ZMQContext() );
  txSocket = std::shared_ptr < ZMQPublishSocket > ( new ZMQPublishSocket(*txContext,publish) );
  txSocket->highWaterMark(highWaterMark);
  txOk=now();
}

void ZMQHub::txLoop() 
{
  txOpen();
  while (running) {
    if (txOk+txTimeout < now()) txOpen();
    txWait();
    if (tx(*txSocket)) txOk=now();
    ++txCount;
  }
  txClose();
}

void ZMQHub::reportLoop()
{
  const float dt = 1.00;
  while (running) {

    std::this_thread::sleep_for(std::chrono::microseconds(int(dt*1000000)));
	rxRate = rxCount/dt;
    txRate = txCount/dt;
    report();
    rxCount = 0;
    txCount = 0;
  }
}

void ZMQHub::report()
{
}

void ZMQHub::start() 
{ 
  if (running == false) {
    running = true;
    if (publish != "") {
      goTx = new std::thread(&ZMQHub::txLoop,this);
    }
    if (!subscribers.empty()) {
      goRx = new std::thread(&ZMQHub::rxLoop,this);
    }
    if (publish != "" || !subscribers.empty()) {
      goReport = new std::thread(&ZMQHub::reportLoop,this);
    }
  }
}

void ZMQHub::join() 
{
  if (goTx) { goTx->join(); delete goTx; goTx=0; }
  if (goRx) { goRx->join(); delete goRx; goRx=0; }
  if (goReport) { goReport->join(); delete goReport; goReport=0; }
}

void ZMQHub::stop()
{
  running=false;
}

ZMQHub::~ZMQHub() { 
  ZMQHub::stop();
  ZMQHub::join();
}
