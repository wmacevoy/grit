#include "NeckMover.h"

void NeckMover::move(Neck &neck)
{
  upDown.move(*neck.upDown);
  leftRight.move(*neck.leftRight);
}

void NeckMover::torque(float t)
{
  upDown.torque=t;
  leftRight.torque=t;
}
