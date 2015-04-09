#include "zmq.h"
#include "ZMQPublishSocket.h"
#include "ZMQ_OK.h"
#include <string>
#include <iostream>

ZMQPublishSocket::ZMQPublishSocket(ZMQContext &context, const std::string &address)
{
  me=zmq_socket(context.me, ZMQ_PUB);
  ZMQ_OK(zmq_bind(me,address.c_str()));
}

