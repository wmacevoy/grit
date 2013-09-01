#include "Neck.h"
#include "BodyGlobals.h"

void Neck::init()
{
  float t=768;
  upDown=servo("NECKUD");
  leftRight=servo("NECKLR");
  leftRight->torque(t);
  upDown->torque(t);
}
