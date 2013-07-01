#include <ctime>

#include "FakeServo.h"
#include "now.h"

FakeServo::FakeServo()
{
  currentAngle = 0;
  goalAngle = 0;
  minAngle = -10*360;
  maxAngle =  10*360;
  rate = 90.0;
  t = now();
}

void FakeServo::update() const
{
  double delta = now()-t;
  t += delta;
  
  float dist = rate*delta;
  float effectiveGoalAngle = goalAngle;
  
  if (goalAngle < minAngle) effectiveGoalAngle = minAngle;
  if (goalAngle > maxAngle) effectiveGoalAngle = maxAngle;
  
  if (currentAngle < effectiveGoalAngle - dist) {
    currentAngle += dist;
  } else if (currentAngle < effectiveGoalAngle + dist) {
    currentAngle = effectiveGoalAngle;
  } else {
    currentAngle -= dist;
  }
}

float FakeServo::angle() const
{
  update();
  return currentAngle;
}

void FakeServo::angle(float value)
{
  update();
  goalAngle = value;
}
