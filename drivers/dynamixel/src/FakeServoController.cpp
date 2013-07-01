#include "FakeServoController.h"

Servo* FakeServoController::servo(int id) { return new FakeServo(); }
