#pragma once

#include "ArmGeometry.h"
#include "ik_leftarm.h"

class LeftArmGeometry : public ArmGeometry
{
 public:
  ik_leftarmparameters parameters;
  ik_leftarmvariables variables;
  ik_leftarmglobals globals;

  LeftArmGeometry();
  virtual void forward();
  virtual bool inverse();
  virtual void compute(float x,float y,float z,/* float pointx,*/ float downx, 
	       float &shoulderio,float &shoulderud,float &bicep, float &elbow, float &forearm);
};
