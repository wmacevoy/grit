#include "Neck.h"
#include "BodyGlobals.h"

void Neck::init()
{
  upDown=servo("NECKUD");
  leftRight=servo("NECKLR");
}
