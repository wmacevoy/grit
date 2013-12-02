#include <iostream>
#include <math.h>

#include "Vec3d.h"
#include "Arc.h"

using namespace std;



void test1()
{
  Arc arc;
  Vec3d p1(1,1,0);
  Vec3d p2(0,0,0);
  Vec3d p3(-1,1,0);

  arc.fromPoints(p1,p2,p3);

  int n=10;
  cout << "s,x,y,z,px,py,pz,ux,uy,uz" << endl;
  for (int i=0; i<=n; ++i) {
    float s=float(i)/float(n);
    Mat3d p=arc.pose(s);
    Vec3d at=p.o();
    Vec3d point=p.ey();
    Vec3d up=p.ez();
    cout << s 
	 << "," << at(0) << "," << at(1) << "," << at(2) 
	 << "," << point(0) << "," << point(1) << "," << point(2)
	 << "," << up(0) << "," << up(1) << "," << up(2)
	 << endl;
  }
}

void test2()
{
  Arc arc;
  Vec3d p1(5.6,24.3,16.3);
  Vec3d p2(3.95,19.3,17.5);
  Vec3d p3(4.9,18.3,14.5);

  arc.fromPoints(p1,p2,p3);

  int n=10;
  cout << "s,x,y,z,px,py,pz,ux,uy,uz" << endl;
  for (int i=0; i<=n; ++i) {
    float s=float(i)/float(n);
    Mat3d p=arc.pose(s);
    Vec3d at=p.o();
    Vec3d point=p.ey();
    Vec3d up=p.ez();
    cout << s 
	 << "," << at(0) << "," << at(1) << "," << at(2) 
	 << "," << point(0) << "," << point(1) << "," << point(2)
	 << "," << up(0) << "," << up(1) << "," << up(2)
	 << endl;
  }
}

void test3()
{
  Mat3d m=Mat3d::identity();

  m=m*translate(Vec3d(10,30,15));
  m=m*rotate(Vec3d::o,Vec3d::ex,90-15);
  
  cout << "m=" << m << endl;
  Vec3d p1=m*Vec3d(15*cos(M_PI/180*30),15*sin(M_PI/180*30),0);
  Vec3d p2=m*Vec3d(15*cos(M_PI/180*0),15*sin(M_PI/180*0),0);
  Vec3d p3=m*Vec3d(15*cos(M_PI/180*(-30)),15*sin(M_PI/180*(-30)),0);

  cout << "p1=" << p1 << " p2=" << p2 << " p3=" << p3 << endl;
  Arc arc;

  arc.fromPoints(p1,p2,p3);

  int n=10;
  cout << "s,x,y,z,px,py,pz,ux,uy,uz" << endl;
  for (int i=0; i<=n; ++i) {
    float s=float(i)/float(n);
    Mat3d p=arc.pose(s);
    Vec3d at=p.o();
    Vec3d point=p.ey();
    Vec3d up=p.ez();
    cout << s 
	 << "," << at(0) << "," << at(1) << "," << at(2) 
	 << "," << point(0) << "," << point(1) << "," << point(2)
	 << "," << up(0) << "," << up(1) << "," << up(2)
	 << endl;
  }
}

void test4()
{
  Mat3d m=Mat3d::identity();

  m=m*translate(Vec3d(10,30,15));
  m=m*rotate(Vec3d::o,Vec3d::ex,90-15);
  
  cout << "m=" << m << endl;
  Vec3d p1=m*Vec3d(15*cos(M_PI/180*30),15*sin(M_PI/180*(-30)),0);
  Vec3d p2=m*Vec3d(15*cos(M_PI/180*0),15*sin(M_PI/180*0),0);
  Vec3d p3=m*Vec3d(15*cos(M_PI/180*(-30)),15*sin(M_PI/180*(30)),0);

  cout << "p1=" << p1 << " p2=" << p2 << " p3=" << p3 << endl;
  Arc arc;

  arc.fromPoints(p1,p2,p3);

  int n=10;
  cout << "s,x,y,z,px,py,pz,ux,uy,uz" << endl;
  for (int i=0; i<=n; ++i) {
    float s=float(i)/float(n);
    Mat3d p=arc.pose(s);
    Vec3d at=p.o();
    Vec3d point=p.ey();
    Vec3d up=p.ez();
    cout << s 
	 << "," << at(0) << "," << at(1) << "," << at(2) 
	 << "," << point(0) << "," << point(1) << "," << point(2)
	 << "," << up(0) << "," << up(1) << "," << up(2)
	 << endl;
  }
}

int main()
{
  //  test1();
  //  test2();
  //  test3();
  test4();
  return 0;
}
