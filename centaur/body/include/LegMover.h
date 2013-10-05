#pragma once

#include <map>
#include "Point.h"
#include "ServoMover.h"
#include "Leg.h"


class LegMover
{
public:
  ServoMover kneeMover;
  ServoMover femurMover;
  ServoMover hipMover;

  void move(Leg &leg);
  void setup(Leg &leg, const std::map < float , Point > &t2tips,
	     double simTime0=0, double simTime1=1e99);
  void setup(Leg &leg, Point p);
};
