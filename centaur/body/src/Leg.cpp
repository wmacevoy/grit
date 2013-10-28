#include "BodyGlobals.h"
#include "Leg.h"
#include <sstream>

using namespace std;

void Leg::init() {
  ostringstream oss;
  oss << "LEG" << (number()+1);
  name=oss.str();
  knee = servo(name + "_KNEE");
  femur = servo(name + "_FEMUR");
  hip = servo(name + "_HIP");
}

void Leg::report() {
  cout << name << ":" << " knee=" << knee->angle() << " femur=" << femur->angle() << " hip=" << hip->angle() << endl;
}

void Leg::temp_report() {
    cout << name << ":" << " knee=" << knee->temp() << " femur=" << femur->temp() << " hip=" << hip->temp() << endl;
}
