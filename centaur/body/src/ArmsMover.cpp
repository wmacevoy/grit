#include "ArmsMover.h"

ArmMover::ArmMover()
{
  trigger.sharpCutoff=0;
  middle.sharpCutoff=0;
  ring.sharpCutoff=0;
  thumb.sharpCutoff=0;
}

void ArmMover::move(Arm &arm)
{
  upDown.move(*arm.upDown);
  leftRight.move(*arm.leftRight);
  bicep.move(*arm.bicep);
  elbow.move(*arm.elbow);
  forearm.move(*arm.forearm);
  trigger.move(*arm.trigger);
  middle.move(*arm.middle);
  ring.move(*arm.ring);
  thumb.move(*arm.thumb);
}

void ArmMover::torque(float t)
{
  upDown.torque=t;
  leftRight.torque=t;
  bicep.torque=t;
  elbow.torque=t;
  forearm.torque=t;
  trigger.torque=t;
  middle.torque=t;
  ring.torque=t;
  thumb.torque=t;
}

bool ArmMover::done() const
{
  if (!leftRight.done()) return false;
  if (!upDown.done()) return false;
  if (!bicep.done()) return false;
  if (!elbow.done()) return false;
  if (!forearm.done()) return false;
  if (!trigger.done()) return false;
  if (!middle.done()) return false;
  if (!ring.done()) return false;
  if (!thumb.done()) return false;
  return true;
}
