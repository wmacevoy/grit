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
