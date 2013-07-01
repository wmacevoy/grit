#pragma once

#include "ServoController.h"
#include "FakeServo.h"

class FakeServoController : public ServoController
{
 public:
  Servo* servo(int id);
};
