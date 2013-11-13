#pragma once

#include "ServoMover.h"
#include "Arms.h"
#include "LeapMessage.h"

class ArmGeometry;

class ArmMover
{
 public:
  ArmGeometry *geometry;
  ServoMover inOut;
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
  bool done() const;
  void leapReset();
  void leapAdjust(LeapHandMessage &leapHandMessage);
  virtual ~ArmMover();
  ArmMover(ArmGeometry *geometry_);
};
