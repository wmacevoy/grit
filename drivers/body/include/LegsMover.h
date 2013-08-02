#pragma once

#include "LegMover.h"
#include "Legs.h"

class LegsMover
{
public:
  LegMover legMovers[4];

  void move(Legs &legs);
  void setLoop(bool doLoop);
  void setup(Legs &legs, const std::map < float , Point > *t2tips); 
};
