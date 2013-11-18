#include "LegsMover.h"

#include "BodyGlobals.h"

void LegsMover::cutoff(float value)
{
  m_cutoff=value;
  for (int i=0; i<4; ++i) {
    if (legMovers[i]) legMovers[i]->cutoff(value);
  }
}

float LegsMover::cutoff() const
{
  return m_cutoff;
}
void LegsMover::state(int m_state_)
{
  m_state = m_state_;
  for (int i=0; i<4; ++i) {
    legMovers[i]->state(m_state);
  }
}

int LegsMover::state() const 
{
  return m_state;
}

LegsMover::LegsMover(BodyMover *bodyMover_)
{
  bodyMover=bodyMover_;
  legMovers[0]=LegMoverSP(new LegMover(this,LEG1));
  legMovers[1]=LegMoverSP(new LegMover(this,LEG2));
  legMovers[2]=LegMoverSP(new LegMover(this,LEG3));
  legMovers[3]=LegMoverSP(new LegMover(this,LEG4));

  state(LegMover::LEG_NORMAL);
  cutoff(cfg->num("legs.cutoff"));
}

void LegsMover::normal(Legs &legs)
{
  for (int i=0; i<4; ++i ) {
    legMovers[i]->move(legs.legs[i]);
  }
}

void LegsMover::move(Legs &legs)
{
  switch(m_state) {
  default: normal(legs); break;
  }
}

  

void LegsMover::setup(Legs &legs, const std::map < float , Point > *t2tips,
		      double simTime0, double simTime1) 
{
  for (int i=0; i<4; ++i) {
    legMovers[i]->setup(legs.legs[i],t2tips[i],simTime0,simTime1);
  }
}

void LegsMover::setup(Legs &legs, const std::map < float , std::pair < Point , int > > *t2tips,
		      double simTime0, double simTime1) 
{
  for (int i=0; i<4; ++i) {
    legMovers[i]->setup(legs.legs[i],t2tips[i],simTime0,simTime1);
  }
}

void LegsMover::torque(float t, int num)
{
  legMovers[num]->torque(t);
}

bool LegsMover::done() const
{
  for (int i=0; i<4; ++i) {
    if (!legMovers[i]->done()) return false;
  }
  return true;
}
