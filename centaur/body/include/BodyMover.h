#pragma once

#include <memory>

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
  bool load(const std::string &file);
};

typedef std::shared_ptr < BodyMover > SPBodyMover;
