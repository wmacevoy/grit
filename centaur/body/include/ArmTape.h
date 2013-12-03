#pragma once

#include <memory>

#include "Tape.h"
#include "ArmGeometry.h"

class ArmArcTape : public Tape
{
 public:
  ArmTape(const std::string &side_); // LEFTARM or RIGHTARM

  double rgap;
  double zgap;

  Arc arc;
  std::shared_ptr < ArmGeometry > geometry;
  std::string side;

  void gripped(float s);
  void opened(float s);
  void grip(float s);
  void open(float s);
};
