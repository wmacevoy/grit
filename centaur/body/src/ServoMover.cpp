#include "BodyGlobals.h"
#include "ServoMover.h"
#include "Lock.h"
#include "math.h"
#include "fit.h"

using namespace std;

void ServoMover::curve(double t[2], float c0[3], float c1[3])
{
  Lock lock(anglesMutex);

  double ts[3];
  float p[3];

  if (angles.size() >= 2) {
    double myTime;
    if (simTime < simTime0) myTime = 0;
    else if (simTime < simTime1) myTime = simTime-simTime0;
    else myTime= simTime1-simTime0;

    if (at == angles.end() || at->first > myTime) {
      at = angles.begin();
    }
    Angles::iterator samples[4];
    samples[0]=at;
    samples[1]=samples[0]; if (samples[1] != angles.end()) ++samples[1];
    samples[2]=samples[1]; if (samples[2] != angles.end()) ++samples[2];
    samples[3]=samples[2]; if (samples[3] != angles.end()) ++samples[3];

    while (samples[3] != angles.end() && (myTime > samples[1]->first)) {
      samples[0]=samples[1];
      samples[1]=samples[2];
      samples[2]=samples[3];
      if (samples[3] != angles.end()) ++samples[3];
    }
    at=samples[0];
      
    double lambda = (fabs(simSpeed) > 0.1) ? 1/simSpeed : 10.0;
    ts[0] = lambda*(samples[0]->first-myTime) + realTime;
    ts[1] = lambda*(samples[1]->first-myTime) + realTime;
    ts[2] = lambda*(samples[2]->first-myTime) + realTime;

    //    cout << "t=" << simTime << " ts=[" << ts[0] << "," << ts[1] << "," << ts[2]  << "]" << endl;
	
    p[0]=samples[0]->second;
    p[1]=samples[1]->second;
    p[2]=samples[2]->second;
    fit(ts,p,c0,c1,linearCutoff);
    t[0]=ts[1];
    t[1]=ts[2];
  } else if (angles.size() == 2) {
    Angles::iterator i[2];
    i[0]=angles.begin();
    i[1]=i[0]; i[1]++;

    t[0]=i[0]->first;
    t[1]=i[1]->first;
    c0[0]=c1[0]=i[1]->second;
    c0[1]=c1[1]=(i[1]->second-i[0]->second)/(t[1]-t[0]);
    c0[2]=c1[2]=0;
  } else if (angles.size() == 1) {
    Angles::iterator i[1];
    i[0]=angles.begin();
    t[0]=i[0]->first;
    t[1]=t[0]+1e6;
    c0[0]=c1[0]=i[0]->second;
    c0[1]=c1[1]=0;
    c0[2]=c1[2]=0;
  } else {
    t[0]=realTime;
    t[1]=realTime+1e6;
    c0[0]=c1[0]=0;
    c0[1]=c1[1]=0;
    c0[2]=c1[2]=0;
  }
}

void ServoMover::move(Servo &servo)
{
  double t[2];
  float c0[3],c1[3];
  curve(t,c0,c1);
  servo.curve(t,c0,c1);
  servo.torque(torque);
}

float ServoMover::speed()
{
  double t[2];
  float c0[3],c1[3];
  curve(t,c0,c1);
  double t0=realTime;
  float *c = (t0 <= t[0] ? c1 : c0);
  return c[1]+(t0-t[0])*c[2];
}

float ServoMover::angle()
{
  double t[2];
  float c0[3],c1[3];
  curve(t,c0,c1);
  double t0=realTime;
  float *c = (t0 <= t[0] ? c1 : c0);
  return c[0]+c[1]*(t0-t[0])+(c[2]/2.0)*pow(t0-t[0],2.0);
}

void ServoMover::setup(const map < float , float > &angles_,
		       double simTime0_, double simTime1_) {
  if (verbose) {
    for (Angles::const_iterator i=angles_.begin(); i!=angles_.end(); ++i) {
      cout << "ServoMover@" << (void*) this << "[" << i->first << "]=" << i->second << endl;
    }
  }
  Lock lock(anglesMutex);
  angles=angles_;
  at=angles.begin();
  simTime0=simTime0_;
  simTime1=simTime1_;
}

void ServoMover::setup(float angle)
{
  map < float , float > angles;
  
  angles[simTime]=angle;
  angles[simTime+1e8]=angle;
  angles[simTime+2e8]=angle;
  setup(angles,simTime,simTime+2e8);
}

ServoMover::ServoMover()
{
  setup(0);
  torque=10;
  linearCutoff=1.0;
}
