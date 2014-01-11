#pragma once

#include "ServoMover.h"
#include "Arms.h"
#include "LeapMessage.h"
#include "Mat3d.h"

class ArmGeometry;
class BodyMover;

class ArmMover
{
 public:
  BodyMover *bodyMover;
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

  double lastLeapTime;
  float lastLeapShoulderIO;
  float lastLeapShoulderUD;
  float lastLeapBicep;
  float lastLeapElbow;
  float lastLeapForearm;
  virtual Mat3d pose()=0;
  virtual void pose(const Mat3d &value);
  virtual void shift(float dx, float dy, float dz, float df);
  void leapAdjust(LeapHandMessage &leapHandMessage);

  virtual ~ArmMover();
  ArmMover(ArmGeometry *geometry_, BodyMover *bodyMover_);
};
