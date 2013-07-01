#pragma once

#include "Servo.h"

class FakeServo : public Servo
{
 public:
 mutable float currentAngle;
 mutable double t;

 float goalAngle;
 float minAngle;
 float maxAngle;
 float rate;
  
  FakeServo();
  void update() const;
  float angle() const;
  void angle(float value);
};
