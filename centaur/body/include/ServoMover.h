#pragma once

#include <map>
#include <mutex>
#include "Servo.h"

class ServoMover
{
public:
  std::mutex anglesMutex;
  typedef std::map < float , float > Angles;
  Angles angles;
  Angles::iterator at;
  double simTime0,simTime1;
  float torque;
  float linearCutoff;

  void move(Servo &servo);
  void setup(const std::map < float , float > &angles_,
	     double simTime0_=0, double simTime1_=1e99);
  void setup(float angle);
  ServoMover();
};
