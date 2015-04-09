#pragma once
#include <iostream>

#include "Vec3d.h"
#include "Mat3d.h"

class Arc3d
{
 public:
  // ez->axis, ex->p1,o->center
  Mat3d frame;
  float radius;
  float theta0,theta1;
  Mat3d pose(float s) const;
  void fromPoints(const Vec3d &p1, const Vec3d &p2, const Vec3d &p3);
  Arc3d();
};

std::ostream& operator<<(std::ostream& out , const Arc3d &arc);


