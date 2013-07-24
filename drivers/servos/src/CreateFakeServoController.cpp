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

#if SERVO_CURVE == 1
  bool curveMode;
  double t0;
  float c0[3],c1[3];
#endif

  FakeServo()
  {
    currentAngle = 0;
    goalAngle = 0;
    minAngle = -10*360;
    maxAngle =  10*360;
    rate = 90.0;
    t = now();

#if SERVO_CURVE == 1
    curveMode = false;
#endif

  }

  void update() const
  {
    double delta = now()-t;
    t += delta;

#if SERVO_MODE == 1
    if (curveMode) {
      float dt=(t-t0);
      float dt2=dt*dt;
      float *c = dt <= 0 ? c0 : c1;
      goalAngle = c[0]+c[1]*dt+c[2]*dt2/2.0;
      goalSpeed = c[1]+c[2]*dt;
    }
#endif
    
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

#if SERVO_CURVE == 1
  void curve(double t0_, float c0_[3],float c1_[3])
  {
    curveMode = true;
    t0=t0_;
    c0[0]=c0_[0];
    c0[1]=c0_[1];
    c0[2]=c0_[2];
    c1[0]=c1_[0];
    c1[1]=c1_[1];
    c1[2]=c1_[2];
  }
#endif
  float angle() const
  {
#if CURVE_DATA == 1
    curveMode = false;
#endif
    update();
    return currentAngle;
  }

  void angle(float value)
  {
    update();
    goalAngle = value;
  }

  void speed(float value)
  {
    rate = value;
  }

  void torque(float value)
  {
    // ignored
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
