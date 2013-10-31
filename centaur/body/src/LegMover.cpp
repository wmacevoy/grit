#include "BodyGlobals.h"
#include "LegMover.h"
#include "Lock.h"
#include <assert.h>
#include <sstream>
#include "math.h"

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
  if (liftsTapes.find(m_tape) == liftsTapes.end()) {
    cout << "new tape for lifts." << endl;
    liftsTapes[m_tape]=CurveSP(new Curve());
  }

  kneeMover=kneeMovers[m_tape];
  femurMover=femurMovers[m_tape];
  hipMover=hipMovers[m_tape];
  lifts=liftsTapes[m_tape];

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
  if (m_state != m_state_ && m_state_ == LEG_BRICKS) {
    check=CHECK_NOTHING;
  }
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
  lifts = CurveSP(new Curve());
}


void LegMover::bricks(Leg &leg)
{  
  float femurAngle,femurSpeed;
  int k0,k1;
  float lift0,lift1;

  {
    Lock lock(tapeMutex);
    femurMover->at(&femurAngle,&femurSpeed);
    lifts->interval(simTime,k0,k1);
    lift0=lifts->knots[k0].y;
    lift1=lifts->knots[k1].y;
  }
    
  femurSpeed=(fabs(simSpeed) > 0.001) ? femurSpeed/simSpeed : 0.0;
  
  
  if (k0 != k1) {
    if (lift0 == 1 && lift1 == -1) {
      check = CHECK_TAP;
//      if (tape() != "f") { tape("f"); }
    }
  }

  if (floor(realTime) != floor(lastRealTime)) {
    cout << "leg " << number()+1 << " check state " << check << " femurAngle=" << femurAngle << " femurSpeed=" << femurSpeed << " pressure=" << sensors.p[number()] << " lift=" << lift1 << " k1=" << k1 << endl;
  }

  switch(check) {
  case CHECK_NOTHING:
    break;
  case CHECK_TAP:
    if (lift1 > 0) {
      check = CHECK_NOTHING;
    } else if (sensors.p[number()] < touchPressure) {
      cout << "leg " << number()+1 << " is tapped angle = " << femurAngle << endl;
      if (tape() != "lf") tape("lf");
      check = CHECK_NOTHING;
    }
     if (k0 != k1) {
      if (lift0 == -2) {
	   check=CHECK_PRESSURE;
       std::cout << "Checking Pressure leg: " << number() + 1 << std::endl; 
      }
    }
    break;
   case CHECK_PRESSURE: {
	   if (sensors.p[number()] < touchPressure) {
	     check=CHECK_NOTHING;
	     break;
	   }
	   if (lift0 == 0 && lift1==0) {
          if (tape() != "f") { 
			  tape("f");
			  simSpeed=0;    
              std::cout << "Stepped Off Bricks leg: " << number() + 1 << std::endl; 
	      } 
       }
    } 
  }

  normal(leg);
}
  
void LegMover::cautious(Leg &leg)
{  
  float femurSpeed;
  {
    Lock lock(tapeMutex);
    femurSpeed=(fabs(simSpeed) > 0.001) 
      ? femurMover->speed()/simSpeed : 0.0;
  }

  if (simSpeed > 0 && femurSpeed < -5.0) {
    if (sensors.p[number()] < touchPressure) {
      simSpeed = 0;
      state(LEG_NORMAL);
    }
  }
  normal(leg);

}

void LegMover::tipping()
{
#if 0
  float g=fabs(sensors.g[0]);
  g=fmax(g,fabs(sensors.g[1]));
  g=fmax(g,fabs(sensors.g[2]));
  if (g > 2500.0) {
    femurMover->torque = 0.2;
    kneeMover->torque = 0.2;
    hipMover->torque = 0.2;
  } else {
    femurMover->torque = 10.0;
    kneeMover->torque = 10.0;
    hipMover->torque = 10.0;
  }
#endif
}

void LegMover::normal(Leg &leg)
{
  //  tipping();
  Lock lock(tapeMutex);
  kneeMover->move(*leg.knee);
  femurMover->move(*leg.femur);
  hipMover->move(*leg.hip);
}

void LegMover::move(Leg &leg)  {
  switch(m_state) {
  case LEG_CAUTIOUS: cautious(leg); break;
  case LEG_BRICKS: bricks(leg); break;
  default: normal(leg); break;
  }
}

void LegMover::setup(Leg &leg, const std::map < float , std::pair<Point,int> > &t2tips,
		     double simTime0, double simTime1) {
  Lock lock(tapeMutex);
  std::map < float , float > t2knee,t2femur,t2hip,t2lifts;

  for (map < float , std::pair < Point, int> > :: const_iterator i = t2tips.begin();
       i != t2tips.end();
       ++i) {
    float t=i->first;
    Point p=i->second.first;
    float knee;
    float femur;
    float hip;
    leg.compute3D(p.x,p.y,p.z,knee,femur,hip);
    cout << leg.name << "," << t << "," << p.x << "," << p.y << "," << p.z << "," << knee << "," << femur << "," << hip << "," << i->second.second << endl;
    t2knee[t]=knee;
    t2femur[t]=femur;
    t2hip[t]=hip;
    t2lifts[t]=i->second.second;
  }
  lifts->setup(t2lifts);
  kneeMover->setup(t2knee,simTime0,simTime1);
  femurMover->setup(t2femur,simTime0,simTime1);
  hipMover->setup(t2hip,simTime0,simTime1);
}

void LegMover::setup(Leg &leg, const std::map < float , Point > &t2tips,
		     double simTime0, double simTime1) {

  Lock lock(tapeMutex);
  std::map < float , float > t2knee,t2femur,t2hip,t2lifts;
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
    t2lifts[t]=0;
  }
  lifts->setup(t2lifts);
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
