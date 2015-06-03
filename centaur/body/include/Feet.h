#pragma once

#include "Servo.h"

class Feet
{
public:
  std::shared_ptr < Servo > foot[4];
  Feet();
  void init();
};
