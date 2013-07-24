#pragma once

#include "Servo.h"

class ZMQServo : public Servo
{
 public:
  float goalAngle;
  float currentAngle;
  float goalSpeed;
  float goalTorque;

#if SERVO_CURVE == 1
  bool curveMode;
  double t0;
  float c0[3];
  float c1[3];
  virtual void curve(double t0, float c0[3],float c1[3]);
#endif

  ZMQServo();
  float angle() const;
  void angle(float value);
  void speed(float value);
  void torque(float value);
};
