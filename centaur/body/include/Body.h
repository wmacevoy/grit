#pragma once

#include <memory>
#include <iostream>

#include "Feet.h"
#include "Legs.h"
#include "Neck.h"
#include "Arms.h"
//
class Body
{
 public:
  Feet feet;
  Legs legs;
  std::shared_ptr < Servo > waist;
  Neck neck;
  LeftArm left;
  RightArm right;
  void init();
  void report(std::ostream &out) const;
  void temp_report(std::ostream &out, std::string part) const;
};

typedef std::shared_ptr < Body > SPBody;

