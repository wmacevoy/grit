#pragma once

#include "zmq.h"
#include <iostream>
#include <assert.h>

#define ZMQ_OK(CMD) { if ((CMD) != 0) { std::cout << "zmqerror: " << zmq_strerror(zmq_errno()) << std::endl; assert(false); } }
