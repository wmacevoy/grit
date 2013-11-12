#pragma once

#include "ArmMover.h"
#include "LeftArmGeometry.h"

class LeftArmMover : public ArmMover
{
 public:
  LeftArmGeometry leftArmGeometry;
  LeftArmMover();
};
