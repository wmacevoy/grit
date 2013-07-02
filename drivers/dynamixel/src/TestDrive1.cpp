#include <ctime>
#include <math.h>
#include "DynamixelDriver.h"

using namespace std;

int main()
{
  Servo servo;

  servo.init(1,"test");
  
  while (1) {
    double t = double(clock())/double(CLOCKS_PER_SEC);
    servo.joint(2048+cos(t)*2048.0);
  }
}

