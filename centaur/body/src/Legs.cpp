#include "BodyGlobals.h"
#include "Legs.h"

Legs::Legs()
{
  legs[LEG1].number(LEG1);
  legs[LEG1].setPosition(-HIPAXISX,HIPAXISY);

  legs[LEG2].number(LEG2);
  legs[LEG2].setPosition(HIPAXISX,HIPAXISY);
  legs[LEG2].setHipOffset(-ANGLE90);

  legs[LEG3].number(LEG3);
  legs[LEG3].setPosition(HIPAXISX,-HIPAXISY);
  legs[LEG3].setHipOffset(-ANGLE180);

  legs[LEG4].number(LEG4);
  legs[LEG4].setPosition(-HIPAXISX,-HIPAXISY);
  legs[LEG4].setHipOffset(ANGLE90);
}

void Legs::init()
{
  for (int i=0; i<4; ++i) {
    legs[i].init();
  }
}
