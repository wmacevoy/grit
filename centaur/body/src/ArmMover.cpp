#include "ArmMover.h"
#include "ArmGeometry.h"
#include "BodyGlobals.h"
#include <map>

using namespace std;

ArmMover::ArmMover(ArmGeometry *geometry_, BodyMover *bodyMover_)
{
  geometry=geometry_;
  bodyMover=bodyMover_;
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
  lastLeapTime=simTime;
  lastLeapShoulderIO=inOut.angle();
  lastLeapShoulderUD=upDown.angle();
  lastLeapElbow=elbow.angle();
  lastLeapForearm=forearm.angle();
}

void ArmMover::leapAdjust(LeapHandMessage &hand)
{
  map<float,float> shoulderios;
  map<float,float> shoulderuds;
  map<float,float> biceps;
  map<float,float> elbows;
  map<float,float> forearms;

  float _shoulderio,_shoulderud,_bicep,_elbow,_forearm;
  geometry->compute(hand.at[0],hand.at[1],hand.at[2],/* hand.point[0], */ hand.down[0],
		    _shoulderio,_shoulderud,_bicep,_elbow,_forearm);

  shoulderios[0]=_shoulderio;
  shoulderuds[0]=_shoulderud;
  biceps[0]=_bicep;
  elbows[0]=_elbow;
  forearms[0]=_forearm;
  
  if (simTime-0.5 < lastLeapTime && lastLeapTime < simTime-0.001) {
    double dt=lastLeapTime-simTime;
    shoulderios[dt]=lastLeapShoulderIO;
    shoulderuds[dt]=lastLeapShoulderUD;
    biceps[dt]=lastLeapBicep;
    elbows[dt]=lastLeapElbow;
    forearms[dt]=lastLeapForearm;
  }

  inOut.setup(shoulderios,simTime,simTime);
  upDown.setup(shoulderuds,simTime,simTime);
  bicep.setup(biceps,simTime,simTime);
  elbow.setup(elbows,simTime,simTime);
  forearm.setup(forearms,simTime,simTime);

  lastLeapTime = simTime;
  lastLeapShoulderIO=_shoulderio;
  lastLeapShoulderUD=_shoulderud;
  lastLeapBicep=_bicep;
  lastLeapElbow=_elbow;
  lastLeapForearm=_forearm;
}

void ArmMover::pose(const Mat3d &value)
{
  Vec3d at=value.o();
  Vec3d up=value.ez();
  Vec3d point=value.ey();

  if (up.z() < 0) { up = -up; }
  if (point.y() < 0) { point = -point; }

  float ioAngle,udAngle,bicepAngle,elbowAngle,forearmAngle;
  geometry->compute(at.x(),at.y(),at.z(),/* point.x(), */ -up.x(),
		    ioAngle,udAngle,bicepAngle,elbowAngle,forearmAngle);
  inOut.setup(ioAngle);
  upDown.setup(udAngle);
  bicep.setup(bicepAngle);
  elbow.setup(elbowAngle);
  forearm.setup(forearmAngle);
}

void ArmMover::shift(float dx, float dy, float dz, float df)
{
  Mat3d frame=pose();
  geometry->forward();
  float saved = forearm.angle();
  frame=translate(Vec3d(dx,dy,dz))*frame*rotate(Vec3d::o,Vec3d::ey,df);
  pose(frame);
  if (df == 0) forearm.setup(saved);
}

ArmMover::~ArmMover() {}
