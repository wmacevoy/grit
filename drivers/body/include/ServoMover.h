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
  bool loop;
  float torque;

  float t0,T;

  void setLoop(bool doLoop);
  void move(Servo &servo);

  void setup(const std::map < float , float > &angles_);
  void setup(float angle);
  void wave(double t0,double T,double amin, double amax,int n=5);
  ServoMover();
};
