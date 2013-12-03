#pragma once

#include <iostream>
#include "Vec3d.h"

class Mat3d
{
 public:
  float data[3][4];
  Mat3d();
  Mat3d(const Vec3d &ex, const Vec3d &ey, const Vec3d &ez, const Vec3d &o);
  inline const float& operator()(int r, int c) const { return data[r][c]; }
  inline float& operator()(int r, int c) { return data[r][c]; }
  static Mat3d identity();
  static Mat3d zero();

  Vec3d ex() const;
  Vec3d ey() const;
  Vec3d ez() const;
  Vec3d o() const;
};

Mat3d translate(const Vec3d &d);
Mat3d rotate(const Vec3d &o, const Vec3d &u, float theta_deg);
Mat3d scale(const Vec3d &s);

Mat3d operator+(const Mat3d &a, const Mat3d &b);
Mat3d operator-(const Mat3d &a, const Mat3d &b);
Mat3d operator-(const Mat3d &a);
Mat3d operator*(const Mat3d &a, const Mat3d &b);
Mat3d operator*(float a, const Mat3d &b);
Vec3d operator*(const Mat3d &a, const Vec3d &b);

Mat3d inverse(const Mat3d &a);

//Mat3d inverse(const Mat3d &a);

// Ax=b, solve for x=inverse(A)*b
//Vec3d solve(const Mat3d &a, const Mat3d &b);

std::ostream& operator<< (std::ostream &out, const Mat3d &v);

