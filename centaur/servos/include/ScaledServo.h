#pragma once

#include <memory>
#include "Servo.h"

//
// setting the scaled servo's angle to x sets the underlying unscaled
// servo to slope*x+offset
//
class ScaledServo : public Servo
{
 public:
  ScaledServo(std::shared_ptr < Servo > unscaledServo_, float slope_, float home_);
  std::shared_ptr < Servo > unscaledServo;
  float slope,offset;
  void init(std::shared_ptr < Servo > unscaledServo_, float slope_, float home_);
  virtual void curve(double t[2], float c0[3],float c1[3]);

  inline float angle() const { return (unscaledServo->angle()-offset)/slope; }
  inline void angle(float value) { unscaledServo->angle(slope*value+offset); }
  inline void speed(float value) { unscaledServo->speed(slope*value); }
  inline void torque(float value) { unscaledServo->torque(value/slope); }
  inline void rate(float value) { unscaledServo->rate(value); }
  inline uint8_t temp() const { return unscaledServo->temp(); }
};
