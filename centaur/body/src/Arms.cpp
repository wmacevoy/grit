#include "Arms.h"
#include "BodyGlobals.h"

void Arm::goLimp()
{ 
  float t = 0.0;
  leftRight->torque(t);
  upDown->torque(t);
  bicep->torque(t);
  elbow->torque(t);
  forearm->torque(t);
  trigger->torque(t);
  middle->torque(t);
  ring->torque(t);
  thumb->torque(t);
  leftRight->torque(t);
}

void Arm::report(std::ostream &out) const
{
  out << ",";
  leftRight->report(out);
  out << ",";
  upDown->report(out);
  out << ",";
  bicep->report(out);
  out << ",";
  elbow->report(out);
  out << ",";
  forearm->report(out);
  out << ",";
  trigger->report(out);
  out << ",";
  middle->report(out);
  out << ",";
  ring->report(out);
  out << ",";
  thumb->report(out);
  out << ",";
  leftRight->report(out);	
}

void LeftArm::init()
{
  leftRight=servo("LEFTARM_SHOULDER_IO");
  upDown=servo("LEFTARM_SHOULDER_UD");
  bicep=servo("LEFTARM_BICEP_ROTATE");
  elbow=servo("LEFTARM_ELBOW");
  forearm=servo("LEFTARM_FOREARM_ROTATE");
  trigger=servo("LEFTARM_TRIGGER");  
  middle=servo("LEFTARM_MIDDLE");  
  ring=servo("LEFTARM_RING");
  thumb=servo("LEFTARM_THUMB");
  leftRight->angle(45.0);
//  upDown->angle(45.0);
}

void RightArm::init()
{
  leftRight=servo("RIGHTARM_SHOULDER_IO");
  upDown=servo("RIGHTARM_SHOULDER_UD");
  bicep=servo("RIGHTARM_BICEP_ROTATE");
  elbow=servo("RIGHTARM_ELBOW");
  forearm=servo("RIGHTARM_FOREARM_ROTATE");
  trigger=servo("RIGHTARM_TRIGGER");  
  middle=servo("RIGHTARM_MIDDLE");  
  ring=servo("RIGHTARM_RING");
  thumb=servo("RIGHTARM_THUMB");
}
