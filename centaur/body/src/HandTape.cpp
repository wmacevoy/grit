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

void HandTape::grip(double s)
{
  double s0=maxSimTime();
  double s1=(s0+s)/2.0;
  paths[side + "_THUMB"][s1]=0;
  paths[side + "_TRIGGER"][s]=0;
  paths[side + "_MIDDLE"][s]=0;
  paths[side + "_RING"][s]=0;
}

void HandTape::open(double s)
{
  double s0=maxSimTime();
  double s1=(s0+s)/2.0;
  paths[side + "_THUMB"][s]=0;
  paths[side + "_TRIGGER"][s1]=0;
  paths[side + "_MIDDLE"][s1]=0;
  paths[side + "_RING"][s1]=0;
}
