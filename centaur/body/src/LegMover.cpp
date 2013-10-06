#include "LegMover.h"

using namespace std;

void LegMover::move(Leg &leg)  {
  kneeMover.move(*leg.knee);
  femurMover.move(*leg.femur);
  hipMover.move(*leg.hip);
}

void LegMover::setup(Leg &leg, const std::map < float , Point > &t2tips,
		     double simTime0, double simTime1) {
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
  kneeMover.setup(t2knee,simTime0,simTime1);
  femurMover.setup(t2femur,simTime0,simTime1);
  hipMover.setup(t2hip,simTime0,simTime1);
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

void LegMover::torque(float t)
{
  femurMover.torque=t;
  kneeMover.torque=t;
  hipMover.torque=t;
}
