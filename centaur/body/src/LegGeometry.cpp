#include "LegGeometry.h"
#include "BodyGlobals.h"
#include "math.h"

using namespace std;

void LegGeometry::setPosition(float newlcx,float newlcy) {
  lcx=newlcx;
  lcy=newlcy;
}
void LegGeometry::setHipOffset(float newHipOffset) {
  hipOffset=newHipOffset;
}
LegGeometry::LegGeometry() {
  hipOffset=0.0;
  koffset=2.801;
  ktibia=16.50;
  l0=3.051; //inches
  l1=8.25;
  l2=sqrt(ktibia*ktibia+koffset*koffset);
  kangle=atan(koffset/ktibia)*ANGLE180/M_PI;
  zoffset=1.335;
  lcx=5.707; // distance from center of chassis to hip axis
  lcy=5.707; // distance from center of chassis to hip axis
}
void LegGeometry::setName(string newName) {
  name=newName;
}
void LegGeometry::outputName(ostream &out) {
  out << name << endl;
}
float LegGeometry::robustACos(float cosvalue) {
  if (cosvalue >= 1.0) {
    return 0;
  } else if (cosvalue <= -1.0) {
    return M_PI;
  } else {
    return acos(cosvalue);
  }

#if 0
  float retval=0.0;
//		cout <<"CosValue:"<<cosvalue<<endl;
  while (cosvalue > 1.0) {
    cosvalue-=2.0;
    retval+=M_PI/2.0;
  }
  while (cosvalue<-1.0) {
    cosvalue+=2.0;
    retval-=M_PI/2.0;
  }
  retval+=acos(cosvalue);
  //		cout <<"ACos:" << retval <<endl;
  return retval;
#endif
} 
// Do not worry about the hip rotation
void LegGeometry::compute2D(float x,float z,float &knee,float &femur) {
  float d=sqrt(x*x+z*z); // distance from hip point in space
  //	  cout << "2D x="<<x << " z="<<z << " d="<<d<<endl;
  //	  cout << "l0="<<l0<<" l1="<<l1<<" l2="<<l2<<" kangle="<<kangle<<endl;
  float theta1=robustACos((l1*l1+l2*l2-d*d)/(2.0*l1*l2))*ANGLE180/M_PI;
  float theta2=robustACos((d*d+l1*l1-l2*l2)/(2.0*d*l1))*ANGLE180/M_PI;
  float theta3=fabs(atan(x/z))*ANGLE180/M_PI;
  //	  cout << "Theta 1:"<< theta1 << endl;
  //	  cout << "Theta 2:"<< theta2 << endl;	  
  //	  cout << "Theta 3:"<< theta3 << endl;
  knee=theta1+(ANGLE90-kangle)-ANGLE180;
  femur=(theta2+theta3)-ANGLE90;
}
void LegGeometry::compute3D(float x,float y,float z,float &knee,float &femur,float &hip) {
  cout << "3D x="<<x << " y="<<y << " z="<<z<<endl;
  float nx=x;
  float ny=y;
  
  x=nx-lcx;
  y=ny-lcy;
  //	  cout << "x=" << x << "  y=" << y << endl;
  float d=sqrt(x*x+y*y);
  if (y>0) 
    hip=ANGLE180+(-acos(x/d)*ANGLE180/M_PI-ANGLE45);
  else  
    hip=-(ANGLE180+(-acos(x/d)*ANGLE180/M_PI-ANGLE45))-ANGLE90;
  hip+=hipOffset;
  if (hip<-ANGLE180) hip+=ANGLE360;  	    
  compute2D(d-l0,z-zoffset,knee,femur);
  cout << "knee=" << knee << " femur=" << femur << " hip=" << hip << endl;
}
