#include "model.h"

World world;
#define OSS(INSERTS) (((std::ostringstream&)(std::ostringstream() << INSERTS)).str())

void ik_leg(int id)
{
  Vec p0=
    world["body"]["legs"][OSS("leg" << id)]
    ["hip"]["femur"]["knee"]["foot"]["toe"].origin();
  Vec p1 = vec(var("px"),var("py"),var("pz"));
  
  eq.push_back(p1[0]-p0[0]);
  eq.push_back(p1[1]-p0[1]);
  eq.push_back(p1[2]-p0[2]);

  gsolve(OSS("leg"<<id),eq,x,world.parameters());
}

void sdl()
{
  world.sdl();
}
