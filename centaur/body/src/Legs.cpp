#include "BodyGlobals.h"
#include "Legs.h"

Legs::Legs()
{

  legs[LEG1].setPosition(-HIPAXISX,HIPAXISY);
  legs[LEG2].setPosition(HIPAXISX,HIPAXISY);
  legs[LEG2].setHipOffset(-ANGLE90);
  legs[LEG3].setPosition(HIPAXISX,-HIPAXISY);
  legs[LEG3].setHipOffset(-ANGLE180);
  legs[LEG4].setPosition(-HIPAXISX,-HIPAXISY);
  legs[LEG4].setHipOffset(ANGLE90);
}

void Legs::init()
{
  legs[LEG1].init("LEG1");
  legs[LEG2].init("LEG2");
  legs[LEG3].init("LEG3");
  legs[LEG4].init("LEG4");
}
