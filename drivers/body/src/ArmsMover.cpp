#include "ArmsMover.h"

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
