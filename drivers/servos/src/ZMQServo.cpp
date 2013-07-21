#include "ZMQServo.h"

ZMQServo::ZMQServo() 
  : goalAngle(0), currentAngle(0) 
{
}

float ZMQServo::angle() const 
{ 
  return currentAngle; 
}

void ZMQServo::angle(float value) 
{ 
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

