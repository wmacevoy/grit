#include "ZMQServo.h"
#include "now.h"

ZMQServo::ZMQServo() 
  : goalAngle(0), presentAngle(0), goalSpeed(0), goalTorque(0), 
    goalRate(1.0), curveMode(false), presentTemp(0), presentTempTime(0)
{
  curveMode = false;
  goalRate = 1.0;
  t[0] = 0.0;
  t[1] = 0.0;
  c0[0]=c0[1]=c0[2] = 0.0;
  c1[0]=c1[1]=c1[2] = 0.0;
}

void ZMQServo::curve(double t_[2], float c0_[3],float c1_[3])
{
  curveMode = true;
  t[0]=t_[0];
  t[1]=t_[1];
  c0[0]=c0_[0];
  c0[1]=c0_[1];
  c0[2]=c0_[2];
  c1[0]=c1_[0];
  c1[1]=c1_[1];
  c1[2]=c1_[2];
}

float ZMQServo::angle() const 
{ 
  return presentAngle; 
}

void ZMQServo::angle(float value) 
{ 
  curveMode = false;
  goalAngle = value; 
}

void ZMQServo::speed(float value) 
{ 
  goalSpeed = value; 
}

float ZMQServo::speed() const 
{ 
  return goalSpeed; 
}

void ZMQServo::torque(float value) 
{ 
  goalTorque = value; 
}

float ZMQServo::torque() const 
{ 
  return goalTorque; 
}

void ZMQServo::rate(float value)
{
  goalRate = value;
}

void ZMQServo::temp(uint8_t value)
{
  presentTempTime=now();
  presentTemp=value; 
}

uint8_t ZMQServo::temp() const
{
  if (presentTempTime < now() - 20.0) {
    return 1;
  } else {
   return presentTemp;
  }
}
