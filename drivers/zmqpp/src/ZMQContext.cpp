#include "zmq.h"
#include "ZMQContext.h"

ZMQContext::ZMQContext() { me = zmq_ctx_new(); }
ZMQContext::~ZMQContext() { zmq_ctx_destroy(me); }
