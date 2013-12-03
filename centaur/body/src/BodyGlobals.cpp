#include "BodyGlobals.h"

#include <string>
#include "string.h"

using namespace std;

bool verbose;

double simTime;
double lastSimTime;
double simSpeed;
double realTime;
double lastRealTime;

std::map < std::string , double > strs;
std::map < std::string , double > nums;
std::map < std::string , Vec3d >  vecs;
std::map < std::string , Mat3d >  mats;
std::map < std::string , Arc3d >  arcs;


std::shared_ptr < Configure > cfg;

SPServoController servoController;

SPBody body;
SPBodyMover mover;

SensorsMessage sensors;
SafetySP safety;

std::map < int , SPServo > servos;

SPServo servo(std::string name)
{
  int id = atoi(cfg->servo(name,"id").c_str());
  if (servos.find(id) != servos.end()) return servos[id];
  if (verbose) {
    cout << "servo " << name << " is " << id << endl;
  }
  SPServo ans = SPServo(servoController->servo(id));
  ans->id(id);
  ans->name(name);
  servos[id]=ans;
  return ans;
}
