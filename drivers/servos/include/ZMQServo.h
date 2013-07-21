#pragma once

#include "Servo.h"

class ZMQServo : public Servo
{
 public:
  float goalAngle;
  float currentAngle;
  float goalSpeed;
  float goalTorque;
  ZMQServo();
  float angle() const;
  void angle(float value);
  void speed(float value);
  void torque(float value);
};
