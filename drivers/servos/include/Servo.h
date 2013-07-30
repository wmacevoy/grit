#pragma once

#ifndef SERVO_CURVE
#error config.h not yet included
#endif

class Servo
{
public:
  virtual void curve(double t[2], float c0[3],float c1[3])=0;
  virtual float angle() const=0; // degrees
  virtual void  angle(float value)=0;
  virtual void  speed(float value)=0; // degrees/sec
  virtual void  torque(float value)=0; // fraction of maximum
  virtual ~Servo();
};
