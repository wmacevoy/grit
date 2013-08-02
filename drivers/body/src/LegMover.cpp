#include "LegMover.h"

using namespace std;

void LegMover::setLoop(bool doLoop) { 
  kneeMover.setLoop(doLoop);
  femurMover.setLoop(doLoop);
  hipMover.setLoop(doLoop);
}

void LegMover::move(Leg &leg)  {
  kneeMover.move(*leg.knee);
  femurMover.move(*leg.femur);
  hipMover.move(*leg.hip);
}

void LegMover::setup(Leg &leg, const std::map < float , Point > &t2tips) {
  std::map < float , float > t2knee,t2femur,t2hip;
  for (map < float , Point > :: const_iterator i = t2tips.begin();
       i != t2tips.end();
       ++i) {
    float t=i->first;
    Point p=i->second;
    float knee;
    float femur;
    float hip;
    leg.compute3D(p.x,p.y,p.z,knee,femur,hip);
    t2knee[t]=knee;
    t2femur[t]=femur;
    t2hip[t]=hip;
  }
  kneeMover.setup(t2knee);
  femurMover.setup(t2femur);
  hipMover.setup(t2hip);
}

void LegMover::setup(Leg &leg, Point p)
{
  float knee;
  float femur;
  float hip;
  leg.compute3D(p.x,p.y,p.z,knee,femur,hip);

  kneeMover.setup(knee);
  femurMover.setup(femur);
  hipMover.setup(hip);
}
