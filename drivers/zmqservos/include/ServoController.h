#pragma once

#include "Servo.h"

#include <memory>

class ServoController
{
public:
  virtual Servo* servo(int id)=0;
  virtual void start();
  virtual ~ServoController();
};

typedef std::shared_ptr < ServoController > SPServoController;
