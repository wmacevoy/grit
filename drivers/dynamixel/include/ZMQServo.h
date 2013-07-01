#pragma once

#include <mutex>
#include <thread>
#include "Servo.h"
#include "ServoController.h"

class ZMQServoClientState
{
 public:
  float currentAngle;
  float goalAngle;
};

class ZMQServo : public Servo
{
  ZMQServoClientState *state;
  float angle() const;
  void angle(float value);
};

class ZMQServoController : public ServoController
{
 private:
  std::map < int , ZMQServoClientState > states;
  std::thread thread;

  ZMQServoController();
  ~ZMQServoController();
  Servo *servo(const std::string &id);
};

