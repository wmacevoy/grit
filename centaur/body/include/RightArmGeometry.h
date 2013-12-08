#pragma once

#include "ArmGeometry.h"
#include "ik_rightarm.h"

class RightArmGeometry : public ArmGeometry
{
 public:
  ik_rightarmparameters parameters;
  ik_rightarmvariables variables;
  ik_rightarmglobals globals;

  RightArmGeometry();
  virtual void forward();
  virtual bool inverse();
  virtual void compute(float x,float y,float z,/* float pointx, */ float downx, 
	       float &shoulderio,float &shoulderud,float &bicep, float &elbow, float &forearm);
};
