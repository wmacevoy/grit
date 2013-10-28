#pragma once

#include <map>
#include <memory>
#include "Point.h"
#include "ServoMover.h"
#include "Leg.h"


class LegsMover;

class LegMover : public LegGeometry
{
 private:
  int m_state;
  float phase;
 public:

  void state(int m_state_);
  int state() const;
  LegsMover *legs;
  enum {LEG_NORMAL, LEG_CAUTIOUS};
  LegMover(LegsMover *_legs, int number_);
  ServoMover kneeMover;
  ServoMover femurMover;
  ServoMover hipMover;

  bool upward,downward,touch;
  double zfloor,zbrick;

  void cautious(Leg &leg);
  void normal(Leg &leg);
  void move(Leg &leg);

  void setup(Leg &leg, const std::map < float , Point > &t2tips,
	     double simTime0=0, double simTime1=1e99);
  void setup(Leg &leg, Point p);
  void torque(float t);
};

typedef std::shared_ptr < LegMover > LegMoverSP;
