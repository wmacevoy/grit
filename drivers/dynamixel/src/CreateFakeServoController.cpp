#include "CreateFakeServoController.h"

#include "FakeServoController.h"

ServoController* CreateFakeServoController()
{
  return new FakeServoController();
}
