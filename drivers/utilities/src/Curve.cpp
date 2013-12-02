#include <math.h>
#include <sys/types.h>
#include <assert.h>
#include <iostream>
#include "Curve.h"

using namespace std;

void Curve::clear()
{
  n=0;
  knots.clear();
  at.clear();
}

void Curve::ats()
{
  at.resize(knots.size());
  size_t k = 0;
  float xmin = knots[0].x;
  float xmax = knots[n-1].x;

  for (size_t i=0; i<n; ++i) {
    double t = double(i)/double(n);
    float x = xmin*(1-t)+xmax*t;
    while (k+1 < n && knots[k+1].x <= x) ++k;
    at[i]=k;
  }
}

void Curve::setup(const std::map < float , float > &points)
{
  size_t k;

  n=points.size();
  knots.resize(n);
  at.resize(n);

  if (n == 0) {
    return;
  }

  k=0;
  for (std::map < float , float > :: const_iterator i
	 = points.begin();
       i != points.end();
       ++i) {
    knots[k].x=i->first;
    knots[k].y=i->second;
    knots[k].sharp = false;
    ++k;
  }
  knots[0].sharp=true;
  knots[n-1].sharp=true;
  ats();
}

void Curve::setup(const std::map < float , std::pair < float , bool > > &points)
{
  size_t k;

  n=points.size();
  knots.resize(n);
  at.resize(n);

  if (n == 0) {
    return;
  }

  k=0;
  for (std::map < float , std::pair < float, bool>  > :: const_iterator i
	 = points.begin();
       i != points.end();
       ++i) {
    knots[k].x=i->first;
    knots[k].y=i->second.first;
    knots[k].sharp = i->second.second;
    ++k;
  }
  knots[0].sharp=true;
  knots[n-1].sharp=true;

  ats();
}

void Curve::sharpen(float cutoff)
{
  knots[0].sharp=true;
  knots[n-1].sharp=true;
  float dmax=0;
  for (size_t k = 1; k < n; ++k) {
    dmax=fmax(dmax,fabs((knots[k].y-knots[k-1].y)/(knots[k].x-knots[k-1].x)));
  }
  for (size_t k = 1; k < n-1; ++k) {
    size_t k0=k-1;
    float c[3];

    float dx0=knots[k0+1].x-knots[k0+0].x;
    float dx1=knots[k0+2].x-knots[k0+1].x;
    float dx2=knots[k0+2].x-knots[k0+0].x;

    float dy0=(knots[k0+1].y-knots[k0+0].y);
    float dy1=(knots[k0+2].y-knots[k0+1].y);
    
    float d0=dy0/dx0;
    float d1=dy1/dx1;

    float cut =fabs(d0-d1)/dmax;

    knots[k].sharp = (cutoff == 0 || cut > cutoff);
    
  }
}

void Curve::interval(float x, size_t &k0, size_t &k1)
{
  if (n <= 1) {
    k0=0;
    k1=0;
  } else {
    float xmin=knots[0].x;
    float xmax=knots[n-1].x;
    double t=(x-xmin)/(xmax-xmin);
    ssize_t i = t*n;
    
    if (t <= 0) {
      k0=0;
      k1=0;
    } else if (t >= 1) {
      k0=n-1;
      k1=n-1;
    } else {
      k0=at[i];
      while (k0+1 < n && knots[k0+1].x < x) ++k0;
      k1 = (k0+1 < n) ? k0+1 : k0;
    }
  }
}

float Curve::minX() const {
  return (n > 0) ? knots[0].x : 0;
}

float Curve::maxX() const {
  return (n > 0) ? knots[n-1].x : 0;
}

void Curve::expand(float x, float &x0, float &x1, float c[3])
{
  if (n == 0) {
    c[0]=0;
    c[1]=0;
    c[2]=0;
    x0=x;
    x1=x;
    return;
  }
  if (n == 1) {
    c[0]=knots[0].y;
    c[1]=0;
    c[2]=0;
    x0=x;
    x1=x;
    return;
  }
  
  size_t k0,k1;
  interval(x,k0,k1);
  while (k1-k0+1<3 && !knots[k1].sharp) ++k1;
  while (k1-k0+1<3 && !knots[k0].sharp) --k0;

  //  cout << " x=" << x << " k0=" << k0 << " k1=" << k1 << endl;

  if (k1==k0+2) {
    float dx0=knots[k0+1].x-knots[k0+0].x;
    float dx1=knots[k0+2].x-knots[k0+1].x;
    float dx2=knots[k0+2].x-knots[k0+0].x;

    float dy0=knots[k0+1].y-knots[k0+0].y;
    float dy1=knots[k0+2].y-knots[k0+1].y;

    c[0]=knots[k0+1].y;
    c[2]=-2.0*(dy0/dx0-dy1/dx1)/(dx2);
    c[1]=(dy1/dx1)-c[2]*(dx1)/2.0;

    x0=knots[k0+1].x;
    x1=knots[k0+2].x;
  } else if (k1==k0+1) {
    float dx0=knots[k1].x-knots[k0+0].x;
    float dy0=knots[k1].y-knots[k0+0].y;

    c[0]=knots[k1].y;
    c[1]=dy0/dx0;
    c[2]=0;

    x0=knots[k1].x;
    x1=x0;
  } else {
    c[0]=knots[k1].y;
    c[1]=0;
    c[2]=0;
    x0=knots[k1].x;
    x1=x0;
  }
}
