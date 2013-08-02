#pragma once

#include <string>
#include <iostream>

class LegGeometry {
  // Length of the members of the leg starting with hip
  float l0,l1,l2,zoffset,koffset,ktibia,kangle;
  float lcx,lcy;
 protected:
  float hipOffset;
  std::string name;
 public:
  void setPosition(float newlcx,float newlcy);
  void setHipOffset(float newHipOffset);
  LegGeometry();
  void setName(std::string newName);
  void outputName(std::ostream &out);
  float robustACos(float cosvalue);
  
  // Do not worry about the hip rotation
  void compute2D(float x,float z,float &knee,float &femur);
  void compute3D(float x,float y,float z,float &knee,float &femur,float &hip);
};
