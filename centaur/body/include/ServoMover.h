#pragma once

#include <map>
#include <mutex>
#include <memory>
#include "Servo.h"

#if USE_SERVO_LINEAR != 1
#include "Curve.h"
#endif

class ServoMover
{
public:
  std::mutex access;
#if USE_SERVO_LINEAR == 1
  typedef std::map < float , float > Angles;
  Angles angles;
  Angles::iterator at;
#else
  Curve angles;
#endif
  double simTime0,simTime1;
  float torque;
  float sharpCutoff;

  void curve(double t[2],float c0[3],float c1[3]);
  float angle();
  float speed();
  void at(float *angle, float *speed);
  void move(Servo &servo);
  void setup(const std::map < float , float > &angles_,
	     double simTime0_=0, double simTime1_=1e99);
  void setup(float angle);
  ServoMover();
};

typedef std::shared_ptr < ServoMover > ServoMoverSP;
