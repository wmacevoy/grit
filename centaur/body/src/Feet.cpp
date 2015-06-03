#include <sstream>

#include "Feet.h"
#include "BodyGlobals.h"
#include "BodyMover.h"

Feet::Feet()
{
}


void Feet::init()
{
  for (int i=0; i<4; ++i) {
    std::ostringstream oss;
    oss << "LEG" << (i+1) << "_FOOT";
    foot[i]=servo(oss.str());
  }
}
