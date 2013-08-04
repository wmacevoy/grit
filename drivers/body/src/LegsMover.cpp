#include "LegsMover.h"

#include "BodyGlobals.h"

void LegsMover::move(Legs &legs)
{
  for (int i=0; i<4; ++i ) {
    legMovers[i].move(legs.legs[i]);
  }
}

void LegsMover::setup(float T, Legs &legs, const std::map < float , Point > *t2tips,
		      double simTime0, double simTime1) 
{
  for (int i=0; i<4; ++i) {
    legMovers[i].setup(T,legs.legs[i],t2tips[i],simTime0,simTime1);
  }
}
