#include "zmq.h"
#include "ZMQ_OK.h"
#include "ZMQSocket.h"

ZMQSocket::ZMQSocket() 
{ 
  me = 0; 
}

ZMQSocket::~ZMQSocket() 
{ 
  if (me != 0) zmq_close(me); 
}

void ZMQSocket::highWaterMark(int value) 
{
  ZMQ_OK(zmq_setsockopt(me,ZMQ_SNDHWM,&value,sizeof(value)));
}
