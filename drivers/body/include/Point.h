#pragma once

class Point : public float[3]
{
  Point(float x,float y, float z);
  void shift(float s, const Point &p);
}

Point operator*(double a,const Point &p)
{
  return Point(c*p[0],c*p[1],c*p[2]);
}
