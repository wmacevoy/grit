#pragma once

#include <memory>

class Mover
{
 public:
  virtual void start()=0;
  virtual void stop()=0;
  ~Mover();
};

typedef std::shared_ptr < Mover> MoverSP;

