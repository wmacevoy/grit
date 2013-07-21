#pragma once

#include <string>
#include "ZMQContext.h"
#include "ZMQSocket.h"

class ZMQSubscribeSocket : public ZMQSocket
{
public:
  ZMQSubscribeSocket(ZMQContext &context, const std::string &address);
};
