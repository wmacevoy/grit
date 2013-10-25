#pragma once

#include <map>
#include <mutex>
#include "Servo.h"
#include "Curve.h"

class ServoMover
{
public:
  Curve curve;
  std::mutex curveMutex;

  double simTime0,simTime1;
  float torque;
  float sharpCutoff;

  void move(Servo &servo);
  void setup(const std::map < float , float > &angles_,
	     double simTime0_=0, double simTime1_=1e99);
  void setup(float angle);
  ServoMover();
};
