#pragma once

#include <string>
#include <iostream>

class LegGeometry {
  // Length of the members of the leg starting with hip
  int m_number;
  float l0,l1,l2,zoffset,koffset,ktibia,kangle;
  float lcx,lcy;
 protected:
  float hipOffset;
  std::string m_name;
 public:
  LegGeometry();
  void number(int m_number);
  int number() const;
  void name(const std::string &m_name_);
  const std::string &name() const;
  void setPosition(float newlcx,float newlcy);
  void setHipOffset(float newHipOffset);
  float robustACos(float cosvalue);
  
  // Do not worry about the hip rotation
  void compute2D(float x,float z,float &knee,float &femur);
  void compute3D(float x,float y,float z,float &knee,float &femur,float &hip);
  void forward3D(float knee, float femur, float hip, float &x, float &y, float &z);
};
