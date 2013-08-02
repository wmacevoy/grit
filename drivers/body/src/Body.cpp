#include "Body.h"

#include "BodyGlobals.h"

void Body::init()
{
  legs.init();
  neck.init();
  waist = servo("WAIST");
}
