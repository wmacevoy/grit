#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "Vec3d.h"

using namespace std;

static Vec3d Vec3d_o(0,0,0);
static Vec3d Vec3d_ex(1,0,0);
static Vec3d Vec3d_ey(0,1,0);
static Vec3d Vec3d_ez(0,0,1);

const Vec3d& Vec3d::o=Vec3d_o;
const Vec3d& Vec3d::ex=Vec3d_ex;
const Vec3d& Vec3d::ey=Vec3d_ey;
const Vec3d& Vec3d::ez=Vec3d_ez;

Vec3d::Vec3d()
{
  memset(&data,0,sizeof(data));
}

Vec3d::Vec3d(float x, float y, float z)
{
  data[0]=x;
  data[1]=y;
  data[2]=z;
}

float dot(const Vec3d &a, const Vec3d &b)
{
  return a.x()*b.x()+a.y()*b.y()+a.z()*b.z();
}

Vec3d cross(const Vec3d &u, const Vec3d &v)
{
  return Vec3d(u.y()*v.z()-u.z()*v.y(),
	       u.z()*v.x()-u.x()*v.z(),
	       u.x()*v.y()-u.y()*v.x());
}

float length2(const Vec3d &a) { 
  return a.x()*a.x()+a.y()*a.y()+a.z()*a.z();
}

float length(const Vec3d &a) {
  return sqrt(length2(a));
}

Vec3d operator+(const Vec3d &a, const Vec3d &b)
{
  return Vec3d(a.x()+b.x(),a.y()+b.y(),a.z()+b.z());
}

Vec3d operator-(const Vec3d &a, const Vec3d &b)
{
  return Vec3d(a.x()-b.x(),a.y()-b.y(),a.z()-b.z());
}

Vec3d operator-(const Vec3d &a)
{
  return Vec3d(-a.x(),-a.y(),-a.z());
}

Vec3d operator*(float a, const Vec3d &b)
{
  return Vec3d(a*b.x(),a*b.y(),a*b.z());
}

Vec3d unit(const Vec3d &a)
{
  double l=length(a);
  return Vec3d(a.x()/l,a.y()/l,a.z()/l);
}

float dist(const Vec3d &a, const Vec3d &b)
{
  return length(b-a);
}

std::ostream& operator<< (std::ostream &out, const Vec3d &v)
{
  out << "[" << v(0) << "," << v(1) << "," << v(2) << "]";
  return out;
}

std::istream& operator>> (std::istream &in, Vec3d &v)
{
  while (isblank(in.peek())) in.get();
  if (in.peek() != '[') { in.setstate(istream::failbit); return in; }
  else in.get();
  if (!(in >> v.x())) return in;
  while (isblank(in.peek())) in.get();
  if (in.peek() != ',') { in.setstate(istream::failbit); return in; }
  else in.get();
  if (!(in >> v.y())) return in;  
  while (isblank(in.peek())) in.get();
  if (in.peek() != ',') { in.setstate(istream::failbit); return in; }
  else in.get();
  if (!(in >> v.z())) return in;  
  while (isblank(in.peek())) in.get();
  if (in.peek() != ']') { in.setstate(istream::failbit); return in; }
  else in.get();
  return in;
}
