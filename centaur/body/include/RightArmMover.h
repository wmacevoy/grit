#pragma once

#include "ArmMover.h"
#include "RightArmGeometry.h"

class RightArmMover : public ArmMover
{
 public:
  RightArmGeometry rightArmGeometry;
  RightArmMover();
};


