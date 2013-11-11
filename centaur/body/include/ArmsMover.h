#pragma once

#include "ServoMover.h"
#include "Arms.h"

class ArmMover
{
 public:
  ServoMover leftRight;
  ServoMover upDown;
  ServoMover bicep;
  ServoMover elbow;
  ServoMover forearm;
  ServoMover trigger;
  ServoMover middle;
  ServoMover ring;
  ServoMover thumb;
  void move(Arm &arm);
  void torque(float t);
  ArmMover();
  bool done() const;
};
