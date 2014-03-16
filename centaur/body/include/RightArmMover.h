#pragma once

#include "ArmMover.h"
#include "RightArmGeometry.h"

class BodyMover;

class RightArmMover : public ArmMover
{
 public:
  RightArmGeometry rightArmGeometry;
  Mat3d pose();
  RightArmMover(BodyMover *bodyMover);
  RightArmMover();
};


