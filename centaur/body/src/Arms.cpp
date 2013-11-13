#include "Arms.h"
#include "BodyGlobals.h"

void Arm::goLimp()
{ 
  float t = 0.0;
  inOut->torque(t);
  upDown->torque(t);
  bicep->torque(t);
  elbow->torque(t);
  forearm->torque(t);
  trigger->torque(t);
  middle->torque(t);
  ring->torque(t);
  thumb->torque(t);
  inOut->torque(t);
}

void Arm::report(std::ostream &out) const
{
  inOut->report(out);
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
  inOut->report(out);	
}

void Arm::temp_report(std::ostream &out) const
{
  inOut->temp_report(out);
  out << ",";
  upDown->temp_report(out);
  out << ",";
  bicep->temp_report(out);
  out << ",";
  elbow->temp_report(out);
  out << ",";
  forearm->temp_report(out);
  out << ",";
  trigger->temp_report(out);
  out << ",";
  middle->temp_report(out);
  out << ",";
  ring->temp_report(out);
  out << ",";
  thumb->temp_report(out);
}


void LeftArm::init()
{ 
  float t=768;
  inOut=servo("LEFTARM_SHOULDER_IO");
  upDown=servo("LEFTARM_SHOULDER_UD");
  bicep=servo("LEFTARM_BICEP_ROTATE");
  elbow=servo("LEFTARM_ELBOW");
  forearm=servo("LEFTARM_FOREARM_ROTATE");
  trigger=servo("LEFTARM_TRIGGER");  
  middle=servo("LEFTARM_MIDDLE");  
  ring=servo("LEFTARM_RING");
  thumb=servo("LEFTARM_THUMB");
  inOut->torque(t);
  upDown->torque(t);
  bicep->torque(t);
  elbow->torque(t);
  forearm->torque(t);
  trigger->torque(t);
  middle->torque(t);
  ring->torque(t);
  thumb->torque(t);
  inOut->torque(t);
}

void RightArm::init()
{
  float t=10.0;
  inOut=servo("RIGHTARM_SHOULDER_IO");
  upDown=servo("RIGHTARM_SHOULDER_UD");
  bicep=servo("RIGHTARM_BICEP_ROTATE");
  elbow=servo("RIGHTARM_ELBOW");
  forearm=servo("RIGHTARM_FOREARM_ROTATE");
  trigger=servo("RIGHTARM_TRIGGER");  
  middle=servo("RIGHTARM_MIDDLE");  
  ring=servo("RIGHTARM_RING");
  thumb=servo("RIGHTARM_THUMB");
  inOut->torque(t);
  upDown->torque(t);
  bicep->torque(t);
  elbow->torque(t);
  forearm->torque(t);
  trigger->torque(t);
  middle->torque(t);
  ring->torque(t);
  thumb->torque(t);
  inOut->torque(t);
}
