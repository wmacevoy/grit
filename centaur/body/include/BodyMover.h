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
  bool circle(float r,float x,float y,float z);
  void move(Body &body);
  bool load(const std::string &file);
  bool play(const std::string &file);
  ServoMover* getMover(const std::string &name);
};

typedef std::shared_ptr < BodyMover > SPBodyMover;
