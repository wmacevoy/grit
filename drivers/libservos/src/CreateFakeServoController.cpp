#include "CreateFakeServoController.h"
#include "now.h"

class FakeServo : public Servo
{
 public:
 mutable float currentAngle;
 mutable double t;

 float goalAngle;
 float minAngle;
 float maxAngle;
 float rate;
  
  FakeServo()
  {
    currentAngle = 0;
    goalAngle = 0;
    minAngle = -10*360;
    maxAngle =  10*360;
    rate = 90.0;
    t = now();
  }

  void update() const
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

  float angle() const
  {
    update();
    return currentAngle;
  }

  void angle(float value)
  {
    update();
    goalAngle = value;
  }
};

class FakeServoController : public ServoController
{
 public:
  Servo* servo(int id)
  { 
    return new FakeServo(); 
  }
};

ServoController* CreateFakeServoController()
{
  return new FakeServoController();
}
