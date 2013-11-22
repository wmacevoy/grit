#include <iostream>

#include "Vec3d.h"
#include "Arc.h"

using namespace std;

int main()
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

  return 0;
}
