#include "Foot.h"
#include "now.h"
#include <stdlib.h>

void testFoot()
{
  AnalogIn p2(2);
  Foot foot(3,8,2,4,3,5);
  foot.start();
  while (true) {
    foot.setGoalHeading(now()/120);
    foot.setGoalSpeed(0);
    usleep(10000);
  }
}

int main(int argc, char** argv)
{
  system("sudo modprobe adc");
  testFoot();
  return 0;
}
