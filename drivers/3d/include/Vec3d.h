#pragma once

#include <iostream>

class Vec3d 
{
 public:
  float data[3];
  Vec3d();
  Vec3d(float x, float y, float z);
  inline const float &x() const { return data[0]; }
  inline float& x() { return data[0]; }
  inline const float &y() const { return data[1]; }
  inline float& y() { return data[1]; }
  inline const float &z() const { return data[2]; }
  inline float& z() { return data[2]; }
  inline const float& operator()(int i) const { return data[i]; }
  inline float& operator()(int i) { return data[i]; }
  
  static const Vec3d &ex;
  static const Vec3d &ey;
  static const Vec3d &ez;
  static const Vec3d &o;
};

float dot(const Vec3d &a, const Vec3d &b);
Vec3d cross(const Vec3d &a, const Vec3d &b);
float length2(const Vec3d &a);
float length(const Vec3d &a);
Vec3d operator+(const Vec3d &a, const Vec3d &b);
Vec3d operator-(const Vec3d &a, const Vec3d &b);
Vec3d operator-(const Vec3d &a);
Vec3d operator*(float a, const Vec3d &b);

Vec3d unit(const Vec3d &a);

float dist(const Vec3d &a,const Vec3d &b);

std::ostream& operator<< (std::ostream &out, const Vec3d &v);
std::istream& operator>> (std::istream &in, Vec3d &v);
