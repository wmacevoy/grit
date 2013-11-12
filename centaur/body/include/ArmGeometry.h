#pragma once

#include <string>
#include <iostream>

class ArmGeometry {
 protected:
  int m_side;
  std::string m_name;
 public:
  void name(const std::string &name);
  const std::string &name() const;
  virtual void forward()=0;
  virtual bool inverse()=0;
  virtual void compute(float x,float y,float z,float pointx, float downx, 
	       float &shoulderio,float &shoulderud,float &bicep, float &elbow, float &forearm)=0;
  virtual ~ArmGeometry();
};
