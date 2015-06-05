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
  m_number=0;
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

int LegGeometry::number() const {
  return m_number;
}

void LegGeometry::number(int m_number_) {
  m_number=m_number_;
}

void LegGeometry::name(const std::string &name_) {
  m_name=name_;
}

const std::string &LegGeometry::name() const 
{
  return m_name;
}

float LegGeometry::robustACos(float cosvalue) {
  if (cosvalue >= 1.0) {
    return 0;
  } else if (cosvalue <= -1.0) {
    return M_PI;
  } else {
    return acos(cosvalue);
  }
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
  //  cout << "3D x="<<x << " y="<<y << " z="<<z<<endl;
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
  //  cout << "knee=" << knee << " femur=" << femur << " hip=" << hip << endl;
}

void LegGeometry::forward3D(float knee,float femur,float hip, float &x,float &y,float &z)
{
  float achasis = 0;
  switch(number()) {
  case LEG1: achasis = -45*(M_PI/180.0); break;
  case LEG2: achasis =  45*(M_PI/180.0); break;
  case LEG3: achasis = 135*(M_PI/180.0); break;
  case LEG4: achasis =-135*(M_PI/180.0); break;
  }
  float aknee = M_PI/180.0*knee;
  float afemur = M_PI/180.0*femur;
  float ahip = -M_PI/180.0*hip;

  //  cout << "achasis=" << ((360.0/(2.0*M_PI))*achasis) << endl;
  achasis = -achasis;

  float dhip2femur=l0;
  float dfemur2knee=l1;
  float dyknee2tip=koffset;
  float dzknee2tip=-ktibia;
  float drchasis2hip=sqrt(lcx*lcx+lcy*lcy)/2.0;
  float dzchasis2hip=zoffset;

  x=cos(afemur)*(((-1.0))*cos(achasis)*sin(ahip)+((-1.0))*cos(ahip)*sin(achasis))*dfemur2knee+(cos(afemur)*cos(aknee)*(((-1.0))*cos(achasis)*sin(ahip)+((-1.0))*cos(ahip)*sin(achasis))+((-1.0))*sin(afemur)*sin(aknee)*(((-1.0))*cos(achasis)*sin(ahip)+((-1.0))*cos(ahip)*sin(achasis)))*dyknee2tip+(((-1.0))*cos(achasis)*sin(ahip)+((-1.0))*cos(ahip)*sin(achasis))*dhip2femur+(((-1.0))*cos(afemur)*sin(aknee)*(((-1.0))*cos(achasis)*sin(ahip)+((-1.0))*cos(ahip)*sin(achasis))+((-1.0))*cos(aknee)*sin(afemur)*(((-1.0))*cos(achasis)*sin(ahip)+((-1.0))*cos(ahip)*sin(achasis)))*dzknee2tip+((-1.0))*sin(achasis)*drchasis2hip;
  y=cos(achasis)*drchasis2hip+cos(afemur)*(cos(achasis)*cos(ahip)+((-1.0))*sin(achasis)*sin(ahip))*dfemur2knee+(cos(achasis)*cos(ahip)+((-1.0))*sin(achasis)*sin(ahip))*dhip2femur+(cos(afemur)*cos(aknee)*(cos(achasis)*cos(ahip)+((-1.0))*sin(achasis)*sin(ahip))+((-1.0))*sin(afemur)*sin(aknee)*(cos(achasis)*cos(ahip)+((-1.0))*sin(achasis)*sin(ahip)))*dyknee2tip+(((-1.0))*cos(afemur)*sin(aknee)*(cos(achasis)*cos(ahip)+((-1.0))*sin(achasis)*sin(ahip))+((-1.0))*cos(aknee)*sin(afemur)*(cos(achasis)*cos(ahip)+((-1.0))*sin(achasis)*sin(ahip)))*dzknee2tip;
  z=sin(afemur)*dfemur2knee+(cos(afemur)*cos(aknee)+((-1.0))*sin(afemur)*sin(aknee))*dzknee2tip+(cos(afemur)*sin(aknee)+cos(aknee)*sin(afemur))*dyknee2tip+dzchasis2hip;
}
