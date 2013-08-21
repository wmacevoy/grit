#pragma once

#include <memory>
#include "Servo.h"

class ServoController
{
public:
  virtual Servo* servo(int id)=0;
  virtual void start();
  virtual ~ServoController();
};

typedef std::shared_ptr < ServoController > SPServoController;
