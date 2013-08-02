#pragma once

#include "LegGeometry.h"
#include "Servo.h"
#include <memory>
#include <string>

class Leg : public LegGeometry {
public:
  std::shared_ptr < Servo >  knee,femur,hip;
  std::string name;
  void init(std::string newName);
  void report();
};
