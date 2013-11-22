#include "Arc.h"
#include <math.h>
#include "Vec3d.h"
#include "Mat3d.h"
#include <assert.h>

using namespace std;

Mat3d Arc::pose(float s) const
{
  float t=(theta0*(1-s)+theta1*s);

  Mat3d m(frame);
  
  m = m*rotate(Vec3d::o,Vec3d::ez,t);
  m = m*translate(Vec3d(radius,0,0));
  m = m*rotate(Vec3d::o,Vec3d::ez,90);
  m = m*rotate(Vec3d::o,Vec3d::ex,90);
  //  cout << "pose(" << s << ")=" << m << endl;
  
  return m;
}

void Arc::fromPoints(const Vec3d &p1, const Vec3d &p2, const Vec3d &p3)
{
  Vec3d t = p2-p1;
  Vec3d u = p3-p1;
  Vec3d v = p3-p2;

  Vec3d w = cross(t,u);
  double wsl2 = length2(w);

  double iwsl2 = 1.0 / (2.0*wsl2);
  double tt = dot(t,t);
  double uu = dot(u,u);

  Vec3d o = p1 + iwsl2*(tt*(dot(u,v))*u - uu*(dot(t,v))*t);
  radius = sqrt(tt * uu * (dot(v,v)) * iwsl2*0.5);
  Vec3d ez = unit(w);
  Vec3d ex = unit(p1-o);
  Vec3d ey = cross(ez,ex);

  float t1=(180/M_PI)*atan2(dot(ey,p1-o),dot(ex,p1-o));
  float t2=(180/M_PI)*atan2(dot(ey,p2-o),dot(ex,p2-o));
  float t3=(180/M_PI)*atan2(dot(ey,p3-o),dot(ex,p3-o));

  //
  // vvt3c
  // pwt3c
  // 
  double d12=t2-t1;
  if (d12 < 0) d12 += 360;
  double d23=t3-t2;
  if (d23 < 0) d23 += 360;
  if (d12+d23 < 360) {
    t2=t1+d12;
    t3=t2+d23;
  } else {
    d12=-(t2-t1);
    if (d12 < 0) d12 += 360;
    d23=-(t3-t2);
    if (d23 < 0) d23 += 360;
    t2=t1-d12;
    t3=t2-d23;
  }

  assert((t1 < t2 && t2 < t3) || (t1 > t2 && t2 > t3));
  assert(dist(o+cos((M_PI/180)*t1)*ex+sin((M_PI/180)*t1)*ey,p1) < 1e-3);
  assert(dist(o+cos((M_PI/180)*t2)*ex+sin((M_PI/180)*t2)*ey,p2) < 1e-3);
  assert(dist(o+cos((M_PI/180)*t3)*ex+sin((M_PI/180)*t3)*ey,p3) < 1e-3);

  theta0=t1;
  theta1=t3;

  frame=Mat3d(ex,ey,ez,o);
}

