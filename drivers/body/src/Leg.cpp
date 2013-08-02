#include "BodyGlobals.h"
#include "Leg.h"

using namespace std;

void Leg::init(string newName) {
  name=newName;
  knee = servo(name + "_KNEE");
  femur = servo(name + "_FEMUR");
  hip = servo(name + "_HIP");
}

void Leg::report() {
  cout << name << ":" << " knee=" << knee->angle() << " femur=" << femur->angle() << " hip=" << hip->angle() << endl;
}
