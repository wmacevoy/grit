#include <math.h>
#include <memory>

#include "FeetMover.h"
#include "LegMover.h"
#include "BodyMover.h"

FeetMover::FeetMover(BodyMover *bodyMover_)
  : bodyMover(bodyMover_)
{
  vx=0;
  vy=0;
  omega=0;
}

void FeetMover::goals(double _vx, double _vy, double _omega)
{
  vx=_vx;
  vy=_vy;
  omega=_omega;
}

void FeetMover::move(Feet &target)
{
  double angle;
  double speed;

  for (int i=0; i<4; ++i) {
    LegMover &legMover = *bodyMover->legs.legMovers[i];
    float x,y,z;
    float knee,femur,hip;
    knee=legMover.kneeMover->angle();
    femur=legMover.femurMover->angle();
    hip=legMover.hipMover->angle();
    legMover.forward3D(knee,femur,hip,x,y,z);

    double theta=(180/M_PI)*atan2(y,x);
    if ((vx != 0 || vy != 0) && omega == 0) {
      double goal = (180/M_PI)*atan2(vy,vx);
      angle = (goal-theta)+90;
      speed = sqrt(vx*vx+vy*vy);
      while (angle < 0) angle += 360;
      while (angle >= 360) angle -= 360;
      if (angle > 180) {
	angle -= 180;
	speed = -speed;
      } else {
	speed =  speed;
      }
      std::cout << "foot " << (i+1) << ": hip=" << hip << " x=" << x << " y=" << y << " goal=" << goal << " theta=" << theta << " angle=" << angle << " speed=" << speed << std::endl;
    } else if ((vx == 0 && vy == 0) && (omega != 0)) {
      if (omega > 0) {
	angle = 0;
	speed=omega;
      } else {
	angle = 180;
	speed=omega;
      }
    } else {
      angle=90;
      speed=0;
    }

    if (i == 2 || i == 3) {
      speed = -speed;
    }

    target.foot[i]->angle(angle);
    target.foot[i]->speed(speed);
  }
}

void FeetMover::torque(float t)
{
  for (int i=0; i<4; ++i) {
    foot[i].torque=t;
  }
}

bool FeetMover::done() const
{
  for (int i=0; i<4; ++i) {
    if (!foot[i].done()) return false;
  }
  return true;
}
