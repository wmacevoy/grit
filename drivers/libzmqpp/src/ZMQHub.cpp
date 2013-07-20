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
#include <pthread.h>
#include <vector>
#include <zmq.h>

#include "zmq.h"

#include "ZMQHub.h"

#include "ZMQContext.h"
#include "ZMQPublishSocket.h"
#include "ZMQSubscribeSocket.h"
#include "ZMQMessage.h"
#include "ZMQ_OK.h"

using namespace std;

ZMQHub::ZMQHub()
{
  rate = 20;
  highWaterMark = 1;
  running = false;
  goTx = 0;
  goRx = 0;
  goReport = 0;
  txCount=0;
  rxCount=0;
  txRate = 0.0/1.0;
  rxRate = 0.0/1.0;
}

void ZMQHub::rxLoop() 
{
  ZMQContext context;
  vector < shared_ptr < ZMQSubscribeSocket > > sockets;

  vector < zmq_pollitem_t > items;

  for (size_t i=0; i<subscribers.size(); ++i) {
    ZMQSubscribeSocket *p = new ZMQSubscribeSocket(context,subscribers[i].c_str());
    p->highWaterMark(highWaterMark);
    sockets.push_back(shared_ptr<ZMQSubscribeSocket>(p));
  }
  
  items.resize(sockets.size());
  
  for (size_t i=0; i < items.size(); ++i) {
    items[i].socket = sockets[i]->me;
    items[i].events = ZMQ_POLLIN;
  }
  
  while (running) {
    if (zmq_poll(&items[0],items.size(),int(1.0*1000)) <= 0) continue;
    for (size_t i=0; i != sockets.size(); ++i) {
      if ((items[i].revents & ZMQ_POLLIN) != 0) {
	++rxCount;
	rx(*sockets[i]);
	items[i].revents = 0;
      }
    }
  }
}

void ZMQHub::txLoop() 
{
  ZMQContext context;
  ZMQPublishSocket socket(context, publish);
  socket.highWaterMark(highWaterMark);

  while (running) {
    usleep(int((1.0/rate)*1000000));
    ++txCount;
    tx(socket);
  }
}

void ZMQHub::reportLoop()
{
  const float dt = 1.00;
  while (running) {
    usleep(int(dt*1000000));
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
    goTx = new std::thread(&ZMQHub::txLoop,this);
    goRx = new std::thread(&ZMQHub::rxLoop,this);
    goReport = new std::thread(&ZMQHub::reportLoop,this);
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
