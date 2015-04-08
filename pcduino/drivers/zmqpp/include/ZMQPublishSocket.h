#pragma once

#include <string>
#include "ZMQSocket.h"
#include "ZMQContext.h"

class ZMQPublishSocket : public ZMQSocket
{
public:
  ZMQPublishSocket(ZMQContext &context, const std::string &address);
};
