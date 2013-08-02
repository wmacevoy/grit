#include "BodyGlobals.h"
#include "ServoMover.h"
#include "Lock.h"
#include "math.h"
#include "fit.h"

using namespace std;

void ServoMover::setLoop(bool doLoop) { 
  loop = doLoop;
}

void ServoMover::move(Servo &servo)
{
  Lock lock(anglesMutex);

  double ts[3];
  float p[3],c0[3],c1[3];

  if (angles.size() >= 2) {
    float s= (simTime-t0)/T;
    if (!loop && s>=1.0) s=1.0;
    s=s-floor(s);
    s=t0+T*s;

    while (at != angles.end() && s > at->first) ++at;
    if (at == angles.end()) at=angles.begin();
    while (at != angles.begin() && s < at->first) --at;
    float oldTime = at->first;
    float oldAngle = at->second;
    Angles::iterator after(at);
    if (++after == angles.end()) after=angles.begin();
    float newTime = after->first;
    if (newTime < oldTime) newTime += T;
    float newAngle = after->second; 
    if (++after == angles.end()) at=angles.begin();
    float newTime2 = after->first;
    if (newTime2 < oldTime) newTime2 += T;
    float newAngle2 = after->second;

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
    servo.speed(15);
    servo.torque(torque);
  } else {
    servo.angle(0);
    servo.speed(15);
    servo.torque(torque);
  }
}

void ServoMover::setup(const map < float , float > &angles_) {
  Lock lock(anglesMutex);
  angles=angles_;

  at=angles.begin();
  if (angles.size() == 0) {
    t0=0;
    T=1.0;
  } else {
    t0=angles.begin()->first;
    if (angles.size() > 1) {
      T=angles.rbegin()->first-t0;
    } else {
      T=1.0;
    }
  }
}

void ServoMover::setup(float angle)
{
  map < float , float > angles;
  angles[0]=angle;
  angles[0.5]=angle;
  angles[1]=angle;
  setup(angles);
}

void ServoMover::wave(double t0,double T,double amin, double amax,int n) 
{
  Lock lock(anglesMutex);
  map < float , float > angles;
  for (int i=0; i<=n; ++i) {
    double t=t0+T*double(i)/double(n);
    angles[t]=(amin+amax)/2+(amax-amin)/2*sin(2*M_PI*(t-t0)/T);
  }
  setup(angles);
}


ServoMover::ServoMover()
{
  loop = true;
  torque = 10;
  setup(0);
}
