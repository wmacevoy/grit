#pragma once

#include <map>
#include <memory>
#include <vector>
#include <mutex>
#include "Point.h"
#include "ServoMover.h"
#include "Leg.h"


class LegsMover;

class LegMover : public LegGeometry
{
 private:
  CurveSP lifts;
  int m_state;
  enum { CHECK_NOTHING, CHECK_TAP, CHECK_PRESSURE };
  int check;
  std::string m_tape;
  std::map<std::string,ServoMoverSP> kneeMovers;
  std::map<std::string,ServoMoverSP> hipMovers;
  std::map<std::string,ServoMoverSP> femurMovers;
  std::map<std::string,CurveSP> liftsTapes;
  mutable std::mutex tapeMutex;
 public:
  int touchPressure;
  
  void state(int m_state_);
  int state() const;

  void tape(const std::string &m_tape_);
  const std::string &tape() const;

  float m_cutoff;
  void cutoff(float value);
  float cutoff() const;

  LegsMover *legs;
  enum {LEG_NORMAL, LEG_CAUTIOUS, LEG_BRICKS };
  LegMover(LegsMover *_legs, int number_);
  ServoMoverSP kneeMover;
  ServoMoverSP femurMover;
  ServoMoverSP hipMover;

  void tipping();
  void cautious(Leg &leg);
  void bricks(Leg &leg);
  void normal(Leg &leg);
  void move(Leg &leg);

  void setup(Leg &leg, const std::map < float , std::pair < Point, int > > &t2tips,
	     double simTime0=0, double simTime1=1e99);

  void setup(Leg &leg, const std::map < float , Point > &t2tips,
	     double simTime0=0, double simTime1=1e99);

  void setup(Leg &leg, Point p);
  void torque(float t);
  bool done() const;
};

typedef std::shared_ptr < LegMover > LegMoverSP;
