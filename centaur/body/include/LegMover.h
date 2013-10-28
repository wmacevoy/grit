#pragma once

#include <map>
#include <memory>
#include <vector>
#include "Point.h"
#include "ServoMover.h"
#include "Leg.h"


class LegsMover;

class LegMover : public LegGeometry
{
 private:
  int m_state;
  std::string m_tape;
  std::map<std::string,ServoMoverSP> kneeMovers;
  std::map<std::string,ServoMoverSP> hipMovers;
  std::map<std::string,ServoMoverSP> femurMovers;
 public:
  int touchPressure;

  void state(int m_state_);
  int state() const;

  void tape(const std::string &m_tape_);
  const std::string &tape() const;

  LegsMover *legs;
  enum {LEG_NORMAL, LEG_CAUTIOUS};
  LegMover(LegsMover *_legs, int number_);
  ServoMoverSP kneeMover;
  ServoMoverSP femurMover;
  ServoMoverSP hipMover;

  void cautious(Leg &leg);
  void normal(Leg &leg);
  void move(Leg &leg);

  void setup(Leg &leg, const std::map < float , Point > &t2tips,
	     double simTime0=0, double simTime1=1e99);
  void setup(Leg &leg, Point p);
  void torque(float t);
};

typedef std::shared_ptr < LegMover > LegMoverSP;
