#pragma once

class Servo
{
public:
#if SERVO_CURVE == 1
  virtual void curve(double t0, float c0[3],float c1[3])=0;
#endif

  virtual float angle() const=0; // degrees
  virtual void  angle(float value)=0;
  virtual void  speed(float value)=0; // degrees/sec
  virtual void  torque(float value)=0; // fraction of maximum
  virtual ~Servo();
};
