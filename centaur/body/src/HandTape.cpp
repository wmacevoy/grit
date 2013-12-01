#include "HandTape.h"

#include <assert.h>

HandTape::HandTape(const std::string &side_)
{
  side=side_;
  assert((side  == "LEFTARM") || (side == "RIGHTARM"));
  
  paths[side + "_THUMB"]=Path();
  paths[side + "_TRIGGER"]=Path();
  paths[side + "_MIDDLE"]=Path();
  paths[side + "_RING"]=Path();
}

void HandTape::opened(float s)
{
  paths[side + "_THUMB"][s]=0;
  paths[side + "_TRIGGER"][s]=0;
  paths[side + "_MIDDLE"][s]=0;
  paths[side + "_RING"][s]=0;
}

void HandTape::gripped(float s)
{
  paths[side + "_THUMB"][s]=360;
  paths[side + "_TRIGGER"][s]=360;
  paths[side + "_MIDDLE"][s]=360;
  paths[side + "_RING"][s]=360;
}

void HandTape::grip(float s)
{
  double s0=maxSimTime();
  double s1=(s0+s)/2.0;
  paths[side + "_THUMB"][s]=360;
  paths[side + "_TRIGGER"][s1]=360;
  paths[side + "_MIDDLE"][s1]=360;
  paths[side + "_RING"][s1]=360;
}

void HandTape::open(float s)
{
  paths[side + "_THUMB"][s]=0;
  paths[side + "_TRIGGER"][s]=0;
  paths[side + "_MIDDLE"][s]=0;
  paths[side + "_RING"][s]=0;
}
