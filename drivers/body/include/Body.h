#pragma once

#include <memory>

#include "Legs.h"
#include "Neck.h"

class Body
{
 public:
  Legs legs;
  std::shared_ptr < Servo > waist;
  Neck neck;
  void init();
};
