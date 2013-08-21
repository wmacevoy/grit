#pragma once

#include <memory>
#include "Servo.h"

class Neck
{
 public:
  std::shared_ptr < Servo > upDown;
  std::shared_ptr < Servo > leftRight; 
  void init();
};
