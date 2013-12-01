#pragma once

#include "Tape.h"

class HandTape : public Tape
{
 public:
  std::string side;
  HandTape(const std::string &side_); // LEFTARM or RIGHTARM
  void grip(double s);
  void open(double s);
};
