#pragma once

class Servo
{
public:
  virtual float angle() const=0; // degrees
  virtual void  angle(float value)=0;
  virtual void  speed(float value)=0; // degrees/sec
  virtual void  torque(float value)=0; // fraction of maximum
  virtual ~Servo();
};
