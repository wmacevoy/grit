#pragma once

#include "Servo.h"

class ZMQServo : public Servo
{
 public:
  float goalAngle;
  float currentAngle;
  ZMQServo();
  float angle() const;
  void angle(float value);
};
