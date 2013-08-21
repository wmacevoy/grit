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
  float torque;
  double simTime0,simTime1;

  float t0,T;

  void move(Servo &servo);
  void setup(float T, const std::map < float , float > &angles_,
	     double simTime0_=0, double simTime1_=1e99);
  void setup(float angle);
  void wave(double t0,double T,double amin, double amax,
	    int n = 5,
	    double simTime0_=0, double simTime1_=1e99);
	    ;
  ServoMover();
};
