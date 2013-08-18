#include "Arms.h"
#include "BodyGlobals.h"

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
