#include "BodyGlobals.h"
#include "ServoMover.h"
#include "Lock.h"
#include "math.h"
#if USE_SERVO_LINEAR == 1
#include "fit.h"
#endif

using namespace std;

#if USE_SERVO_LINEAR == 1
void ServoMover::setup(const map < float , float > &angles_,
		       double simTime0_, double simTime1_) {
  if (verbose) {
    for (Angles::const_iterator i=angles_.begin(); i!=angles_.end(); ++i) {
      cout << "ServoMover@" << (void*) this << "[" << i->first << "]=" << i->second << endl;
    }
  }
  Lock lock(access);
  angles=angles_;
  at=angles.begin();
  simTime0=simTime0_;
  simTime1=simTime1_;
}

void ServoMover::curve(double t[2], float c0[3], float c1[3])
{
  Lock lock(access);

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
    fit(ts,p,c0,c1,sharpCutoff);
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
#else 
void ServoMover::setup(const map < float , float > &angles_,
		       double simTime0_, double simTime1_) {
  if (verbose) {
    for (map<float,float>::const_iterator i=angles_.begin(); i!=angles_.end(); ++i) {
      cout << "ServoMover@" << (void*) this << "[" << i->first << "]=" << i->second << endl;
    }
  }
  Lock lock(access);
  angles.setup(angles_);
  angles.sharpen(sharpCutoff);
  simTime0=simTime0_;
  simTime1=simTime1_;
}

void ServoMover::curve(double t[2], float c0[3], float c1[3])
{
  float s;
  if (simTime < simTime0) s = 0;
  else if (simTime < simTime1) s = simTime-simTime0;
  else s= simTime1-simTime0;

  float ds,s0,s1,s2,s3;

  {
    Lock lock(access);

    angles.expand(s,s0,s1,c0);
    ds=s1-s0;
    c0[0] += (c0[1]+c0[2]/2.0*ds)*ds;
    c0[1] += c0[2]*ds;

    angles.expand(fmax(s1+0.001,simTime1-simTime0),s2,s3,c1);
    ds=s1-s2;
    c1[0] += (c1[1]+c1[2]/2.0*ds)*ds;
    c1[1] += c1[2]*ds;
  }
  
  float lambda = (fabs(simSpeed) > 0.001) ? 1/simSpeed : 1000.0;
  
  c0[1] *= simSpeed;
  c0[2] *= simSpeed*simSpeed;
  c1[1] *= simSpeed;
  c1[2] *= simSpeed*simSpeed;
  
  t[0]=lambda*(s1-s)+realTime;
  t[1]=lambda*(s3-s)+realTime;
}
#endif

void ServoMover::move(Servo &servo)
{
  double t[2];
  float c0[3],c1[3];
  curve(t,c0,c1);
  servo.curve(t,c0,c1);
  servo.torque(torque);
}

void ServoMover::at(float *angle, float *speed)
{
  double t[2];
  float c0[3],c1[3];
  curve(t,c0,c1);
  double dt=realTime-t[0];
  double dt2=dt*dt;
  float *c = (dt <= 0) ? c0 : c1;
  if (angle != 0) *angle = c[0]+c[1]*dt+(c[2]/2.0)*dt2;
  if (speed != 0) *speed = c[1]+c[2]*dt;
}

float ServoMover::speed()
{
  float speed0;
  at(0,&speed0);
  return speed0;
}


float ServoMover::angle()
{
  float angle0;
  at(&angle0,0);
  return angle0;
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
  sharpCutoff=1.0;
}
