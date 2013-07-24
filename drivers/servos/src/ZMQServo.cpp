#include "ZMQServo.h"

ZMQServo::ZMQServo() 
  : goalAngle(0), currentAngle(0), goalSpeed(0), goalTorque(0)
{
#if SERVO_CURVE == 1
  curveMode = false;
  t0 = 0.0;
  c0[0]=c0[1]=c0[2] = 0.0;
  c1[0]=c1[1]=c1[2] = 0.0;
#endif
}

#if SERVO_CURVE == 1
void ZMQServo::curve(double t0_, float c0_[3],float c1_[3])
{
  curveMode = true;
  t0=t0_;
  c0[0]=c0_[0];
  c0[1]=c0_[1];
  c0[2]=c0_[2];
  c1[0]=c1_[0];
  c1[1]=c1_[1];
  c1[2]=c1_[2];
}
#endif

float ZMQServo::angle() const 
{ 
  return currentAngle; 
}

void ZMQServo::angle(float value) 
{ 
#if SERVO_CURVE == 1
  curveMode = false;
#endif
  goalAngle = value; 
}

void ZMQServo::speed(float value) 
{ 
  goalSpeed = value; 
}

void ZMQServo::torque(float value) 
{ 
  goalTorque = value; 
}

