#pragma once

#include "LegMover.h"
#include "Legs.h"

class BodyMover;

class LegsMover
{
 private:
  int m_state;
  float m_cutoff;

public:
  float cutoff() const;
  void cutoff(float value);
  
  LegMoverSP legMovers[4];

  void state(int m_state);
  int state() const;

  BodyMover *bodyMover;

  LegsMover(BodyMover *bodyMover_);


  void normal(Legs &legs);
  void move(Legs &legs);
  void setup(Legs &legs, const std::map < float , Point > *t2tips,
	     double simTime0=0, double simTime1=1e99); 
  void setup(Legs &legs, const std::map < float , std::pair < Point, int > > *t2tips,
	     double simTime0=0, double simTime1=1e99); 
  void torque(float t, int num);
  bool done() const;
};
