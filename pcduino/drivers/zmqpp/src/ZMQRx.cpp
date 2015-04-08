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

#include "ZMQRx.h"

#include "ZMQContext.h"
#include "ZMQPublishSocket.h"
#include "ZMQSubscribeSocket.h"
#include "ZMQMessage.h"
#include "ZMQ_OK.h"

using namespace std;

ZMQRx::ZMQRx()
{
  highWaterMark = 1;
  running = false;
  goRx = 0;
  rxCount=0;
  rxRate = 0.0/1.0;
}

void ZMQRx::rxLoop() 
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
    if (zmq_poll(&items[0],items.size(),int(0.25*1000)) <= 0) continue;
    if (!running) break;
    for (size_t i=0; i != sockets.size(); ++i) {
      if ((items[i].revents & ZMQ_POLLIN) != 0) {
	++rxCount;
	rx(*sockets[i]);
	items[i].revents = 0;
      }
    }
  }
}


void ZMQRx::start() 
{ 
  if (running == false) {
    running = true;
    goRx = new std::thread(&ZMQRx::rxLoop,this);
  }
}

void ZMQRx::join() 
{
  if (goRx) { goRx->join(); delete goRx; goRx=0; }
}

void ZMQRx::stop()
{
  running=false;
}

ZMQRx::~ZMQRx() { 
  ZMQRx::stop();
  ZMQRx::join();
}
