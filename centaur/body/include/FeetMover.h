#pragma once

#include "ServoMover.h"
#include "Feet.h"

class BodyMover;

class FeetMover
{
 public:
  BodyMover *bodyMover;
  FeetMover(BodyMover *bodyMover_);
  double vx;
  double vy;
  double omega;
  void goals(double _vx, double _vy, double _omega);
  ServoMover foot[4];
  void move(Feet &feet);
  void torque(float t);
  bool done() const;
};

