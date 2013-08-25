#pragma once

#include <memory>
#include <iostream>

#include "Legs.h"
#include "Neck.h"
#include "Arms.h"

class Body
{
 public:
  Legs legs;
  std::shared_ptr < Servo > waist;
  Neck neck;
  LeftArm left;
  RightArm right;
  void init();
  void report(std::ostream &out) const;
};
