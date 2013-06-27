// warren macevoy

#pragma once

#include <string>
#include "Servo.hpp"

class ServoConroller
{
public:
  virtual Servo* servo(const std::string &id)=0;
  virtual ~ServoController();
};
