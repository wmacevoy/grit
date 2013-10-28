#include "BodyGlobals.h"
#include "LegMover.h"
#include "Lock.h"
#include <assert.h>
#include <sstream>

using namespace std;

void LegMover::tape(const std::string &m_tape_)
{
  Lock lock(tapeMutex);
  m_tape=m_tape_;
  if (kneeMovers.find(m_tape) == kneeMovers.end()) {
    cout << "new tape for knee." << endl;
    kneeMovers[m_tape]=ServoMoverSP(new ServoMover());
  }
  if (femurMovers.find(m_tape) == femurMovers.end()) {
    cout << "new tape for femur." << endl;
    femurMovers[m_tape]=ServoMoverSP(new ServoMover());
  }
  if (hipMovers.find(m_tape) == hipMovers.end()) {
    cout << "new tape for hip." << endl;
    hipMovers[m_tape]=ServoMoverSP(new ServoMover());
  }
  kneeMover=kneeMovers[m_tape];
  femurMover=femurMovers[m_tape];
  hipMover=hipMovers[m_tape];

  cout << "kneeMover@" << (void*) &*kneeMover << endl;
  cout << "femurMover@" << (void*) &*femurMover << endl;
  cout << "hipMover@" << (void*) &*hipMover << endl;
}

const std::string &LegMover::tape() const
{
  return m_tape;
}

void LegMover::state(int m_state_)
{
  m_state = m_state_;
}

int LegMover::state() const
{
  return m_state;
}

LegMover::LegMover(LegsMover *legs_, int number_)
{
  legs=legs_;
  number(number_);
  state(LEG_NORMAL);

  ostringstream oss;
  oss << "leg" << (number()+1) << ".touchpressure";
  touchPressure=cfg->num(oss.str());
  kneeMover = ServoMoverSP(new ServoMover());
  femurMover = ServoMoverSP(new ServoMover());
  hipMover = ServoMoverSP(new ServoMover());
}


void LegMover::cautious(Leg &leg)
{  
  {
    Lock lock(tapeMutex);
    if (simSpeed > 0 && femurMover->speed()/simSpeed < -5.0) {
      if (sensors.p[number()] < touchPressure) {
	simSpeed = 0;
	state(LEG_NORMAL);
      }
    }
  }
  normal(leg);
}

void LegMover::normal(Leg &leg)
{
  Lock lock(tapeMutex);
  kneeMover->move(*leg.knee);
  femurMover->move(*leg.femur);
  hipMover->move(*leg.hip);
}

void LegMover::move(Leg &leg)  {
  switch(m_state) {
  case LEG_CAUTIOUS: cautious(leg); break;
  default: normal(leg); break;
  }
}

void LegMover::setup(Leg &leg, const std::map < float , Point > &t2tips,
		     double simTime0, double simTime1) {
  Lock lock(tapeMutex);
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
    cout << leg.name << "," << t << "," << p.x << "," << p.y << "," << p.z << "," << knee << "," << femur << "," << hip << endl;
    t2knee[t]=knee;
    t2femur[t]=femur;
    t2hip[t]=hip;
  }
  kneeMover->setup(t2knee,simTime0,simTime1);
  femurMover->setup(t2femur,simTime0,simTime1);
  hipMover->setup(t2hip,simTime0,simTime1);

}

void LegMover::setup(Leg &leg, Point p)
{
  Lock lock(tapeMutex);

  float knee;
  float femur;
  float hip;
  leg.compute3D(p.x,p.y,p.z,knee,femur,hip);

  kneeMover->setup(knee);
  femurMover->setup(femur);
  hipMover->setup(hip);
}

void LegMover::torque(float t)
{
  Lock lock(tapeMutex);
  femurMover->torque=t;
  kneeMover->torque=t;
  hipMover->torque=t;
}
