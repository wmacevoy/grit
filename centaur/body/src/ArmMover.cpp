#include "ArmMover.h"
#include "ArmGeometry.h"

ArmMover::ArmMover(ArmGeometry *geometry_)
{
  geometry=geometry_;
  trigger.sharpCutoff=0;
  middle.sharpCutoff=0;
  ring.sharpCutoff=0;
  thumb.sharpCutoff=0;
}

void ArmMover::move(Arm &arm)
{
  inOut.move(*arm.inOut);
  upDown.move(*arm.upDown);
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
  inOut.torque=t;
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
  if (!inOut.done()) return false;
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

void ArmMover::leapReset()
{
  geometry->forward();
}

void ArmMover::leapAdjust(LeapHandMessage &hand)
{
  float _shoulderio,_shoulderud,_bicep,_elbow,_forearm;
  geometry->compute(hand.at[0],hand.at[1],hand.at[2],hand.point[0],hand.down[0],
		    _shoulderio,_shoulderud,_bicep,_elbow,_forearm);
  inOut.setup(_shoulderio);
  upDown.setup(_shoulderud);
  bicep.setup(_bicep);
  elbow.setup(_elbow);
  forearm.setup(_forearm);
}

ArmMover::~ArmMover() {}
