#include "BodyGlobals.h"

#include <string>
#include "string.h"

using namespace std;

bool verbose;

double simTime;
double simSpeed;
double realTime;

Configure cfg;

SPServoController servoController;

SPBody body;

SPServo servo(std::string name)
{
  int id = atoi(cfg.servo(name,"id").c_str());
  if (verbose) {
    cout << "servo " << name << " is " << id << endl;
  }
  return SPServo(servoController->servo(id));
}