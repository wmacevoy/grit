#include "LegsMover.h"

#include "BodyGlobals.h"

void LegsMover::move(Legs &legs)
{
  for (int i=0; i<4; ++i ) {
    legMovers[i].move(legs.legs[i]);
  }
}
void LegsMover::setLoop(bool doLoop) {
  for (int i=0; i<4; ++i ) legMovers[i].setLoop(doLoop);
}

void LegsMover::setup(Legs &legs, const std::map < float , Point > *t2tips) 
{
  for (int i=0; i<4; ++i) {
    legMovers[i].setup(legs.legs[i],t2tips[i]);
  }
}
