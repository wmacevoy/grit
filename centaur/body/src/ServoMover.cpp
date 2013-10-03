#include "BodyGlobals.h"
#include "ServoMover.h"
#include "Lock.h"
#include "math.h"
#include "fit.h"

using namespace std;

void ServoMover::move(Servo &servo)
{
  Lock lock(anglesMutex);

  double ts[3];
  float p[3],c0[3],c1[3];

  if (angles.size() >= 2) {
    double myTime;
    if (simTime < simTime0) myTime = 0;
    else if (simTime < simTime1) myTime = simTime-simTime0;
    else myTime= simTime1;

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
    fit(ts,p,c0,c1);
    servo.curve(ts+1,c0,c1);
    servo.torque(torque);
  } else if (angles.size() == 1) {
    servo.angle(angles.begin()->second);
    servo.speed(200);
    servo.torque(torque);
  } else {
    servo.angle(0);
    servo.speed(200);
    servo.torque(torque);
  }
}

void ServoMover::setup(float T_, const map < float , float > &angles_,
		       double simTime0_, double simTime1_) {
  for (Angles::const_iterator i=angles_.begin(); i!=angles_.end(); ++i) {
    cout << "ServoMover@" << (void*) this << "[" << i->first << "]=" << i->second << endl;
  }
  Lock lock(anglesMutex);
  angles=angles_;

  at=angles.begin();
  if (angles.size() == 0) {
    t0=0;
  } else {
    t0=angles.begin()->first;
  }
  T=T_;
  simTime0=simTime0_;
  simTime1=simTime1_;
}

void ServoMover::setup(float angle)
{
  map < float , float > angles;
  angles[0]=angle;
  angles[1.0/3.0]=angle;
  angles[2.0/3.0]=angle;
  setup(1.0,angles);
}

void ServoMover::wave(double t0,double T,double amin, double amax,int n,
		      double simTime0_,double simTime1_) 
{
  Lock lock(anglesMutex);
  map < float , float > angles;
  for (int i=0; i<=n; ++i) {
    double t=t0+T*double(i)/double(n);
    angles[t]=(amin+amax)/2+(amax-amin)/2*sin(2*M_PI*(t-t0)/T);
  }
  setup(T,angles,simTime0_,simTime1_);
}


ServoMover::ServoMover()
{
  torque = 10;
  setup(0);
}
