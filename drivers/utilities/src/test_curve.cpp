#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "Curve.h"

using namespace std;

double simTime,simTime0,simTime1;
double realTime;
double simSpeed;

Curve curve;

double f(double x)
{
  return fabs(sin(2*M_PI*x/5.0));
}

void path(double t[2], float c0[3],float c1[3])
{
  float s;
  if (simTime < simTime0) s = 0;
  else if (simTime < simTime1) s = simTime-simTime0;
  else s= simTime1-simTime0;
  
  float s0,s1,c2[3],s2,s3;
  {
    curve.expand(s,s0,s1,c0);
    curve.expand(s1+0.001,s2,s3,c2);
  }
  
  float ds=s0-s2;
  c1[0]=c2[0]+c2[1]*ds+c2[2]*ds*ds/2.0;
  c1[1]=c2[1]+c2[2]*ds;
  c1[2]=c2[1];
  
  float lambda = (fabs(simSpeed) > 0.001) ? 1/simSpeed : 1000.0;
  
  c0[1] *= simSpeed;
  c0[2] *= simSpeed*simSpeed;
  c1[1] *= simSpeed;
  c1[2] *= simSpeed*simSpeed;

  t[0]=lambda*(s0-s)+realTime;
  t[1]=lambda*(s1-s)+realTime;
}

void test2()
{
  int n=20;
  double T = 10.0;
  map < float , float > p;
  simTime0=0;
  simTime1=T;

  for (int i=0; i<=n; ++i) {
    double t=T*float(i)/float(n);
    p[t]=f(t);
  }
  curve.setup(p);
  curve.sharpen();

  int m=8*n;
  cout << "t" << "," << "f" << "," << "t1-t" << "," 
       << "c0" << "," << "c1" << "," << "c2" << "," 
       << "a0" << "," << "v0" << ","
       << "d0" << "," << "d1" << "," << "d2" << "," 
       << "a1" << "," << "v1" << endl;

  for (int i=0; i<=m; ++i) {
    double t=T*float(i)/float(m);
    simTime = t;
    simSpeed = 1.0;
    realTime = 0.0+simTime/simSpeed;

    double ts[2];
    float  c0[3],c1[3];
    path(ts,c0,c1);

    double dt=realTime-ts[0];
    double a0=c0[0]+c0[1]*dt+c0[2]*pow(dt,2)/2.0;
    double v0=c0[1]+c0[2]*dt;
    double a1=c1[0]+c1[1]*dt+c1[2]*pow(dt,2)/2.0;
    double v1=c1[1]+c1[2]*dt;
    //    if (1 <= realTime && realTime <= 2) {
    cout << realTime << "," << f(simTime) << "," << realTime-ts[1] << "," 
	 << c0[0] << "," << c0[1] << "," << c0[2] << "," 
	 << a0 << "," << v0 << "," 
	 << c1[0] << "," << c1[1] << "," << c1[2] << "," 
	 << a1 << "," << v1 << endl;
    //    }
  }
}

void test1()
{
  map<float,float> p;
  
  float xmin=-3;
  float xmax=3;
  int n=10;

  for (int i=0; i<=n; ++i) {
    float t=float(i)/float(n);
    float s=float(n-i)/float(n);
    float x=s*xmin+t*xmax;
    p[x]=f(x);
  }

  curve.setup(p);
  curve.sharpen();

  cout << "x,y,f,x0,x1,c0,c1,c2" << endl;
  for (int i=-n; i<=12*n+n; ++i) {
    float x=xmin+(xmax-xmin)*float(i)/float(12*n);
    float x0,x1,c[3];
    curve.expand(x,x0,x1,c);
    float y=c[0]+c[1]*(x-x0)+c[2]*pow(x-x0,2)/2.0;

    cout << x << "," << y << "," << f(x) << "," 
	 << x0 << "," << x1 << "," 
	 << c[0] << ","<< c[1] << ","<< c[2] << endl;
  }
}

void test3()
{
  map<float,float> p;
  p[8.60142]=0;
  p[9.60142]=360;
  p[12.6014]=360;
  p[14.6014]=0;

  curve.setup(p);
  curve.sharpen();

  float xmin=p.begin()->first;
  float xmax=p.rbegin()->first;
  int n=20;

  cout << "x,y,f,x0,x1,c0,c1,c2" << endl;
  for (int i=-n; i<=12*n+n; ++i) {
    float x=xmin+(xmax-xmin)*float(i)/float(12*n);
    float x0,x1,c[3];
    curve.expand(x,x0,x1,c);
    float y=c[0]+c[1]*(x-x0)+c[2]*pow(x-x0,2)/2.0;

    cout << x << "," << y << "," << "0" << "," 
	 << x0 << "," << x1 << "," 
	 << c[0] << ","<< c[1] << ","<< c[2] << endl;
  }
}
  

int main(int argc, char *argv[])
{
  // test1();
  //test2();
  test3();
  return 0;
}
