#pragma once

#include "ServoMover.h"
#include "Neck.h"

class NeckMover
{
 public:
  ServoMover upDown;
  ServoMover leftRight;
  void move(Neck &neck);
};

