#include <iostream>
#include <math.h>
using namespace std;
#include "ServoMover.h"
#include "CreateFakeServoController.h"
#include "now.h"

#include "BodyGlobals.h"

class MyFakeServo : public Servo
{
 public:
 mutable float currentAngle;
 mutable double t;

 mutable float goalAngle;
 mutable float goalSpeed;

 float minAngle;
 float maxAngle;

  bool curveMode;
  double ct[2];
  float c0[3],c1[3];
  MyFakeServo()
  {
    currentAngle = 0;
    goalAngle = 0;
    minAngle = -10*360;
    maxAngle =  10*360;
    goalSpeed = 90.0;
    t = realTime;

    curveMode = false;

  }
  
  float speed() const { return 0.0; }
  float torque() const { return 0.0; }
  
  uint8_t temp() const
  {
    return 0;
  }

  void update() const
  {
    double delta = realTime-t;
    t += delta;

    if (curveMode) {
      float dt=(t < ct[1]) ? (t-ct[0]) : (ct[1]-ct[0]);
      float dt2=dt*dt;
      const float *c = dt <= 0 ? c0 : c1;
      goalAngle = c[0]+c[1]*dt+c[2]*dt2/2.0;
      goalSpeed = c[1]+c[2]*dt;
    }

    float effectiveGoalAngle = goalAngle;
    if (goalAngle < minAngle) effectiveGoalAngle = minAngle;
    if (goalAngle > maxAngle) effectiveGoalAngle = maxAngle;
    
    float dist = goalSpeed*delta;
    if (currentAngle < effectiveGoalAngle) {
      currentAngle += fabs(dist);
      if (currentAngle > effectiveGoalAngle) currentAngle = effectiveGoalAngle;
    } else {
      currentAngle -= fabs(dist);
      if (currentAngle < effectiveGoalAngle) currentAngle = effectiveGoalAngle;
    }
  }

  void curve(double t_[2], float c0_[3],float c1_[3])
  {
    curveMode = true;
    ct[0]=t_[0];
    ct[1]=t_[1];
    c0[0]=c0_[0];
    c0[1]=c0_[1];
    c0[2]=c0_[2];
    c1[0]=c1_[0];
    c1[1]=c1_[1];
    c1[2]=c1_[2];
  }

  float angle() const
  {
    update();
    return currentAngle;
  }

  void angle(float value)
  {
    curveMode = false;
    update();
    goalAngle = value;
  }

  void speed(float value)
  {
    goalSpeed = value;
  }

  void torque(float value)
  {
    // ignored
  }

  void rate(float value)
  {
    // ignored
  }
};

double f(double t)
{
  return 100*sin(2*M_PI*t/5.0);
}

int main()
{
  double t0=0;

  ServoMover servoMover;
  ServoController *controller = CreateFakeServoController();
  MyFakeServo *servo = new MyFakeServo();
  //Servo *servo = controller->servo(1);
  servo->speed(1e9);
  int n=20;
  double T = 10.0;
  map < float , float > path;

  for (int i=0; i<=n; ++i) {
    double t=T*float(i)/float(n);
    path[t]=f(t);
  }

  simTime = 0;
  realTime = 0;
  simSpeed = 1.0;
  servoMover.setup(T,path,0,T);

  int m=8*n;
  cout << "t" << "," << "a" << "," << "t1" << "," << "c0" << "," << "c1" << "," << "c2" << "," << "g" << endl;
  for (int i=0; i<=m; ++i) {
    double t=T*float(i)/float(m);
    simTime = t;
    realTime = t;
    simSpeed = 1.0;
    //    if (i == m/3) {
    //      cout << "sample." << endl;
    //cout << realTime << "," << f(realTime) << "," << servo->angle() << "," << servo->goalAngle << endl;
    //}
    
    servoMover.move(*servo);
    servo->update();
    //    if (1 <= realTime && realTime <= 2) {
      cout << realTime << "," << f(realTime) << "," << servo->ct[0] << "," << servo->c0[0] << "," << servo->c0[1] << "," << servo->c0[2] << "," << servo->goalAngle << endl;
      //    }
  }

}
