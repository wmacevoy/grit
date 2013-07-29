#pragma once

#ifndef SERVO_CURVE
#error config.h not yet included
#endif

class Servo
{
public:
#if SERVO_CURVE == 1
  virtual void curve(double t[2], float c0[3],float c1[3])=0;
#endif

  virtual float angle() const=0; // degrees
  virtual void  angle(float value)=0;
//  virtual float speed() const=0; // degrees
  virtual void  speed(float value)=0; // degrees/sec
//  virtual float torque() const=0; // degrees
  virtual void  torque(float value)=0; // fraction of maximum
  virtual ~Servo();
};
