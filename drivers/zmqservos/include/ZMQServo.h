#pragma once

#include "Servo.h"

class ZMQServo : public Servo
{
 public:
  float goalAngle;
  float presentAngle;
  float goalSpeed;
  float goalTorque;
  float goalRate;

  bool curveMode;
 private:
  uint8_t presentTemp;
  double presentTempTime;
 public:
  double t[2];
  float c0[3];
  float c1[3];
  virtual void curve(double t[2], float c0[3],float c1[3]);

  ZMQServo();
  float angle() const;
  void angle(float value);
  float speed() const;
  void speed(float value);
  float torque() const;
  void torque(float value);
  void rate(float value);
  uint8_t temp() const;
  void temp(uint8_t value);
};
