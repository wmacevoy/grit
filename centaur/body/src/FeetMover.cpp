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
  for (int i=0; i<4; ++i) {
    LegMover &legMover = *bodyMover->legs.legMovers[i];
    float x,y,z;
    float knee,femur,hip;
    knee=legMover.kneeMover->angle();
    femur=legMover.femurMover->angle();
    hip=legMover.hipMover->angle();
    legMover.forward3D(knee,femur,hip,x,y,z);

    double theta=(180/M_PI)*atan2(y,x);
    if (vx != 0 && vy == 0 && omega == 0) {
      double delta = (90-theta)-90;
      if (i == 0) {
	std::cout << "foot " << i << ": theta=" << theta << " delta=" << delta << std::endl;
      }
      target.foot[i]->angle(delta);
      target.foot[i]->speed(128+vx);
    } else {
      target.foot[i]->speed(0);
    }
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
