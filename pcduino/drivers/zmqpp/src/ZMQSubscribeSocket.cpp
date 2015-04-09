#include <assert.h>
#include "zmq.h"
#include "ZMQSubscribeSocket.h"
#include "ZMQ_OK.h"

ZMQSubscribeSocket::ZMQSubscribeSocket(ZMQContext &context, const std::string &address)
{
  me=zmq_socket(context.me, ZMQ_SUB);
  assert(me != 0);
  assert(zmq_setsockopt(me, ZMQ_SUBSCRIBE, "", 0) == 0);
  assert(zmq_connect(me,address.c_str()) == 0);
}
