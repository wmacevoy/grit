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
  inline float angle() const { return (unscaledServo->angle()-offset)/slope; }
  inline void angle(float value) { unscaledServo->angle(slope*value+offset); }
  inline void speed(float value) { unscaledServo->speed(slope*value); }
  inline void torque(float value) { unscaledServo->torque(value/slope); }
};
