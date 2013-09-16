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

    //    float s= (myTime-t0)/T;
    //    s=s-floor(s);
    //    s=t0+T*s;

    float s = myTime;

    Angles::iterator after(at);

    while (at != angles.end() && s > at->first) {
      after = at;
      ++after;
      if (after == angles.end()) break;
      at = after;
    }
    //    if (at == angles.end()) at=angles.begin();
//    while (at != angles.begin() && s < at->first) --at;
//    int count=0;
//    for (Angles::iterator it=angles.begin();it!=angles.end();it++) {
//      if (it==at) cout << "Step "<< count << " of " << angles.size() << endl;
//      count++;
//    }

    float oldTime = at->first;
    float oldAngle = at->second;

    float newTime,newTime2;
    float newAngle,newAngle2;

    after=at;

    ++after;
    if (after == angles.end()) {
//      cout << "wrapped newTime" << endl;
      after=angles.begin();
    }
    newTime = after->first;
    if (newTime < oldTime) {
//      cout << "shift newTime by T=" << T << endl;
      newTime += T;
    }
    newAngle = after->second; 
    ++after;
    if (after == angles.end()) {
//      cout << "wrapped newTime2" << endl;
      after=angles.begin();
    }
    newTime2 = after->first;
    if (newTime2 < oldTime) {
//      cout << "shift newTime2 by T=" << T << endl;
      newTime2 += T;
    }
    newAngle2 = after->second;

//    cout << "time oldTime=" << oldTime << " newTime=" << newTime << " newTime2=" << newTime2 << endl;
//    cout << "time oldAngle=" << oldAngle << " newAngle=" << newAngle << " newAngle2=" << newAngle2 << endl;
      
    double realTimeNow  = realTime;

    double lambda = (fabs(simSpeed) > 0.1) ? 1/simSpeed : 10.0;
    ts[0] = lambda*(oldTime-s) + realTimeNow;
    ts[1] = lambda*(newTime-s) + realTimeNow;
    ts[2] = lambda*(newTime2-s) + realTimeNow;

	
    p[0]=oldAngle;
    p[1]=newAngle;
    p[2]=newAngle2;
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
