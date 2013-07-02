#pragma once

#include "Servo.h"

class ServoController
{
public:
  virtual Servo* servo(int id)=0;
  virtual void start();
  virtual ~ServoController();
};
