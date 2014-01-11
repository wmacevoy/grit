#include "LeftArmMover.h"
#include "BodyMover.h"
#include "fk_leftarm.h"

LeftArmMover::LeftArmMover(BodyMover *bodyMover_) : 
  ArmMover(&leftArmGeometry,bodyMover_) 
{
}

Mat3d LeftArmMover::pose()
{
  return fk_leftarm(bodyMover->waist.angle(),
		     inOut.angle(),upDown.angle(),bicep.angle(),elbow.angle(),
		     forearm.angle());
}

