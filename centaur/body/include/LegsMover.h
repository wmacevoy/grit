#pragma once

#include "LegMover.h"
#include "Legs.h"

class LegsMover
{
public:
  LegMover legMovers[4];

  void move(Legs &legs);
  void setup(Legs &legs, const std::map < float , Point > *t2tips,
	     double simTime0=0, double simTime1=1e99); 
  void torque(float t, int num);
};
