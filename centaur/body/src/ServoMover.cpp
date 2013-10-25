#include "BodyGlobals.h"
#include "ServoMover.h"
#include "Lock.h"
#include "math.h"
#include "fit.h"

using namespace std;


void ServoMover::move(Servo &servo)
{
  float s;
  if (simTime < simTime0) s = 0;
  else if (simTime < simTime1) s = simTime-simTime0;
  else s= simTime1-simTime0;

  float ds,s0,s1,s2,s3;
  float c0[3],c1[3];

  {
    Lock lock(curveMutex);

    curve.expand(s,s0,s1,c0);
    ds=s1-s0;
    c0[0] += (c0[1]+c0[2]/2.0*ds)*ds;
    c0[1] += c0[2]*ds;

    curve.expand(fmax(s1+0.001,simTime1-simTime0),s2,s3,c1);
    ds=s1-s2;
    c1[0] += (c1[1]+c1[2]/2.0*ds)*ds;
    c1[1] += c1[2]*ds;
  }
  
  float lambda = (fabs(simSpeed) > 0.001) ? 1/simSpeed : 1000.0;
  
  c0[1] *= simSpeed;
  c0[2] *= simSpeed*simSpeed;
  c1[1] *= simSpeed;
  c1[2] *= simSpeed*simSpeed;
  
  double t[2];
  t[0]=lambda*(s1-s)+realTime;
  t[1]=lambda*(s3-s)+realTime;

  servo.curve(t,c0,c1);
  servo.torque(torque);
}

void ServoMover::setup(const map < float , float > &angles_,
		       double simTime0_, double simTime1_) {
  if (verbose) {
    for (map<float,float>::const_iterator i=angles_.begin(); i!=angles_.end(); ++i) {
      cout << "ServoMover@" << (void*) this << "[" << i->first << "]=" << i->second << endl;
    }
  }
  Lock lock(curveMutex);
  curve.setup(angles_);
  curve.sharpen(sharpCutoff);
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
  sharpCutoff=1.0;
}
