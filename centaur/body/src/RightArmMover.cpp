#include "RightArmMover.h"
#include "BodyMover.h"
#include "fk_rightarm.h"

RightArmMover::RightArmMover(BodyMover *bodyMover_) 
  : ArmMover(&rightArmGeometry, bodyMover_) 
{
}

Mat3d RightArmMover::pose()
{
  return fk_rightarm(bodyMover->waist.angle(),
		     inOut.angle(),upDown.angle(),bicep.angle(),elbow.angle(),
		     forearm.angle());
}
