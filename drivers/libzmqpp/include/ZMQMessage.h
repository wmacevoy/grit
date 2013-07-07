#pragma once

#include "ZMQSocket.h"

class ZMQMessage
{
public:
  zmq_msg_t me;
  ZMQMessage();
  ZMQMessage(size_t size);
  ZMQMessage(const void *data, size_t size);
  void *data();
  int recv(ZMQSocket &socket, int arg=0);
  int recv_timeout(ZMQSocket &socket, int millisecondTimeout);
  int send(ZMQSocket &socket, int arg=0);
  ~ZMQMessage();
};

