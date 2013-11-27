#include "Mat3d.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;

Mat3d::Mat3d()
{
  memset(&data,0,sizeof(data));
}

Mat3d::Mat3d(const Vec3d &ex, const Vec3d &ey, const Vec3d &ez, const Vec3d &o)
{
  data[0][0]=ex.x();
  data[1][0]=ex.y();
  data[2][0]=ex.z();

  data[0][1]=ey.x();
  data[1][1]=ey.y();
  data[2][1]=ey.z();

  data[0][2]=ez.x();
  data[1][2]=ez.y();
  data[2][2]=ez.z();

  data[0][3]=o.x();
  data[1][3]=o.y();
  data[2][3]=o.z();
}

Mat3d Mat3d::identity()
{
  return Mat3d(Vec3d::ex,Vec3d::ey,Vec3d::ez,Vec3d::o);
}

Mat3d Mat3d::zero()
{
  return Mat3d();
}

Vec3d Mat3d::ex() const
{
  return Vec3d(data[0][0],data[1][0],data[2][0]);
}

Vec3d Mat3d::ey() const
{
  return Vec3d(data[0][1],data[1][1],data[2][1]);
}

Vec3d Mat3d::ez() const
{
  return Vec3d(data[0][2],data[1][2],data[2][2]);
}

Vec3d Mat3d::o() const
{
  return Vec3d(data[0][3],data[1][3],data[2][3]);
}

Mat3d translate(const Vec3d &d)
{
  return Mat3d(Vec3d::ex,Vec3d::ey,Vec3d::ez,d);
}

Mat3d rotate(const Vec3d &p, const Vec3d &n, float theta_deg)
{
  double sinT=sin(theta_deg*M_PI/180.0);
  double cosT=cos(theta_deg*M_PI/180.0);

  double a=p(0);
  double b=p(1);
  double c=p(2);

  double lenn=length(n);
  double u=n(0)/lenn;
  double v=n(1)/lenn;
  double w=n(2)/lenn;

  double u2=u*u;
  double v2=v*v;
  double w2=w*w;

  double oneMinusCosT = 1-cosT;

  Mat3d m;

  m(0,0) = u2 + (v2 + w2) * cosT;
  m(0,1) = u*v * oneMinusCosT - w*sinT;
  m(0,2) = u*w * oneMinusCosT + v*sinT;
  m(0,3) = (a*(v2 + w2) - u*(b*v + c*w))*oneMinusCosT
                + (b*w - c*v)*sinT;

  m(1,0) = u*v * oneMinusCosT + w*sinT;
  m(1,1) = v2 + (u2 + w2) * cosT;
  m(1,2) = v*w * oneMinusCosT - u*sinT;
  m(1,3) = (b*(u2 + w2) - v*(a*u + c*w))*oneMinusCosT
                + (c*u - a*w)*sinT;

  m(2,0) = u*w * oneMinusCosT - v*sinT;
  m(2,1) = v*w * oneMinusCosT + u*sinT;
  m(2,2) = w2 + (u2 + v2) * cosT;
  m(2,3) = (c*(u2 + v2) - w*(a*u + b*v))*oneMinusCosT
                + (a*v - b*u)*sinT;

  return m;
}



// Mat3d rotate(const Vec3d &o, const Vec3d &u, float theta);

Mat3d scale(const Vec3d &s)
{
  return Mat3d(s.x()*Vec3d::ex,s.y()*Vec3d::ey,s.z()*Vec3d::ez,Vec3d::o);
}

Mat3d operator+(const Mat3d &a, const Mat3d &b)
{
  Mat3d ans;
  for (int r=0; r<3; ++r) {
    for (int c=0; c<4; ++c) {
      ans.data[r][c]=a.data[r][c]+b.data[r][c];
    }
  }
  return ans;
}

Mat3d operator*(const Mat3d &a, const Mat3d &b)
{
  Mat3d ans;
  for (int r=0; r<3; ++r) {
    for (int c=0; c<3; ++c) {
      double sum=0;
      for (int k=0; k<3; ++k) {
	sum += a.data[r][k]*b.data[k][c];
      }
      ans.data[r][c]=sum;
    }
  }
  for (int r = 0; r<3; ++r) {
    double sum=0;
    for (int k=0; k<3; ++k) {
      sum += a.data[r][k]*b.data[k][3];
    }
    ans.data[r][3]=sum+a.data[r][3];
  }

  return ans;
}

std::ostream& operator<< (std::ostream &out, const Mat3d &m)
{
  out << "[" << m.ex() << "," << m.ey() << "," << m.ez() << "," << m.o() << "]";
}


Vec3d operator*(const Mat3d &a, const Vec3d &b)
{
  return b.x()*a.ex()+b.y()*a.ey()+b.z()*a.ez()+a.o();
}
