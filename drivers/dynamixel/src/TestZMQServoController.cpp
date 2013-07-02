#include <math.h>
#include <memory>
#include <iostream>
#include <iomanip>
#include <unistd.h>
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
    double theta = 2*M_PI/4.0*t;
    double s1=90*sin(theta);
    double s2=90*cos(theta);
    servo1->angle(s1);
    servo2->angle(s2);

    if (i % n ==  0) {
      cout << "client: servo1=" << servo1->angle() << " servo2=" << servo2->angle() << endl;
    }
  }
}
