#pragma once

class ZMQSocket 
{
public:
  void *me;
  ZMQSocket();
  virtual ~ZMQSocket();
  void highWaterMark(int value);
};


