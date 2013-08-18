#pragma once

#include "Body.h"
#include "LegsMover.h"
#include "ServoMover.h"
#include "NeckMover.h"
#include "ArmsMover.h"

class BodyMover
{
 public:
  LegsMover legs;
  ServoMover waist;
  NeckMover neck;
  ArmMover left,right;
  void move(Body &body);
};
