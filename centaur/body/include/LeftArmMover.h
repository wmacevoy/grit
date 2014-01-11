#pragma once

#include "ArmMover.h"
#include "LeftArmGeometry.h"

class BodyMover;

class LeftArmMover : public ArmMover
{
 public:
  LeftArmGeometry leftArmGeometry;
  Mat3d pose();
  LeftArmMover(BodyMover *bodyMover_);
};
