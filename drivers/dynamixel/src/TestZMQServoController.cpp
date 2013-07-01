#include <math.h>
#include <memory>
#include <iostream>
#include <iomanip>
#include "CreateZMQServoController.h"
#include "now.h"

using namespace std;

int main()
{
  double t0 = now();
  string me = "tcp://*:5501";
  string server = "tcp://localhost:5500";
  shared_ptr<ServoController> controller(CreateZMQServoController(me,server));
  Servo *servo1 = controller->servo(1);
  Servo *servo2 = controller->servo(2);
  controller->start();

  size_t i = 0, n = 100;

  for (;;) {
    ++i;
    usleep(int((1.0/n)*1000000));

    double t = now()-t0;
    double s1=cos(t/10.0);
    double s2=sin(t/10.0);
    servo1->angle(s1);
    servo2->angle(s2);

    if (i % n ==  0) {
      cout << "client: servo1=" << servo1->angle() << " servo2=" << servo2->angle() << endl;
    }
  }
}
