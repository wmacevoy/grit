#pragma once

#include "Tape.h"

class HandTape : public Tape
{
 public:
  std::string side;
  HandTape(const std::string &side_); // LEFTARM or RIGHTARM
  void gripped(float s);
  void opened(float s);
  void grip(float s);
  void open(float s);
};
