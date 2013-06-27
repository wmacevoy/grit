#pragma once

class Servo
{
public:
  virtual float angle() const=0;
  virtual void  angle(float value)=0;
  virtual ~Servo();
};
