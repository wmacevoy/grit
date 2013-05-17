#include <iostream>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include "DynamixelDriver.hpp"

const int divider=1;

using namespace std;

const int MAXPOSITION=4095;
const float ANGLE180=(float)360.0/2.0;
const float ANGLE90=ANGLE180/2.0;
const int MINPOSITION=0;
const int MAXLIMIT=4000;
const int MINLIMIT=100;

class point {
	public:
	float x,y,z;
};
class angles {
	public:
	float knee,femur,hip;
};

class Point {  // A point is space or set of angles
	public:
	union {
		point p;
		angles a;  
	};
	Point(float newX=0,float newY=0,float newZ=0) {
		p.x=newX;
		p.y=newY;
		p.z=newZ;
	}
	Point interp(float t,const Point &b) {
	  Point newp(t*(b.p.x-p.x)+p.x,t*(b.p.y-p.y)+p.y,t*(b.p.z-p.z)+p.z);
	  return newp;
	}
	Point interp(int i,int max,const Point &b) {
	  float t=(float)i/(float)max;
	  return interp(t,b);
	}
	void mapAngle(float min,float max,float newMin,float newMax) {
		a.knee=(a.knee-min)*(newMax-newMin)/(max-min)+newMin;
		a.femur=(a.femur-min)*(newMax-newMin)/(max-min)+newMin;
		a.hip=(a.hip-min)*(newMax-newMin)/(max-min)+newMin;		
	}
	void reportPoint() {
	  cout << "Point:"<<p.x << "," << p.y <<","<< p.z<<endl;
	}
	void reportAngle() {
	  cout << "Angle:"<<a.knee << "," << a.femur <<","<< a.hip <<endl;
	}
};

class LegGeometry {
	float l0,l1,l2;
	public:
	LegGeometry() {
	  l0=2.5; //inches
	  l1=8.5;
	  l2=15.5;
	}
	// Do not worry about the hip rotation
	void compute2D(float x,float z,float &knee,float &femur) {
	  float d=sqrt(x*x+z*z); // distance from hip point in space
	  float theta1=acos((l1*l1+l2*l2-d*d)/2.0*l1*l2)*ANGLE180/M_PI;
	  float theta2=acos((d*d+l1*l1-l2*l2)/2.0*d*l1)*ANGLE180/M_PI;
	  float theta3=acos((d*d+z*z-x*x)/2.0*d*z)*ANGLE180/M_PI;
	  cout << "x="<<x << " z="<<z << " d="<<d<<endl;
	  cout << "l0="<<l0<<" l1="<<l1<<" l2="<<l2<<endl;
	  cout << "Theta 1:"<< theta1 << endl;
	  cout << "Theta 2:"<< theta2 << endl;	  
	  cout << "Theta 3:"<< theta3 << endl;      
	  knee=theta1;
	  femur=ANGLE180-theta2-theta3;
	}
	void compute3D(float x,float y,float z,float &knee,float &femur,float &hip,bool invert) {
	  float d=sqrt(x*x+y*y);
	  hip=ANGLE180-acos(x/d)*ANGLE180/M_PI;
	  compute2D(d,z,knee,femur);
	  if (invert) {
	    hip=ANGLE180-hip;
	  }
	}
	void compute(Point &p,Point &joint,bool invert) {
	  compute3D(p.p.x,p.p.y,p.p.z,joint.a.knee,joint.a.femur,joint.a.hip,invert);
	}
};

class Leg:public LegGeometry {
	Servo knee,femur,hip;
	string name;
	int kneePos,femurPos,hipPos;
public:
	void init(int kneeid,int femurid,int hipid,string newName) {
	  name=newName;
	  knee.init(kneeid,name+"k");
	  femur.init(femurid,name+"f");
	  hip.init(hipid,name+"h");
	}
	void setPos(int newKnee=2048,int newFemur=2048,int newHip=2048) {
	  kneePos=newKnee;
	  femurPos=newFemur;
	  hipPos=newHip;
	  knee.joint(kneePos);
	  femur.joint(femurPos);
	  hip.joint(hipPos);
	}
	void setTorque(int torque) {
		knee.setTorque(torque);
		femur.setTorque(torque);
		hip.setTorque(torque);
	}
	void report() {
		knee.report();
		femur.report();
		hip.report();
	}
};

const int MAXPOS=10;

class LegSequence {
  int hAngle[MAXPOS];
  int fAngle[MAXPOS];
  int kAngle[MAXPOS];
  long time[MAXPOS];
  protected:
  int pos;
  long total,offset;
  bool reverse,interp;
  LegGeometry lg;
  public:
  LegSequence(LegGeometry newLg) {
	 lg=newLg;
    init(0,false,false);
  }
  virtual void init(long newOffset,bool newReverse,bool newInterp) {
   total=0;
   pos=0;
   offset=newOffset;
   reverse=newReverse;
   interp=newInterp;
  }
  void add(int k,int f,int h,long t) {
    if (pos>=MAXPOS) return;
    hAngle[pos]=h;
    fAngle[pos]=f;
    kAngle[pos]=k;
    time[pos]=t;
    total+=t;
    pos++;
  }
  void pose(Leg &l) {
 //   long millis=std::time(0);
    clock_t t;
    t = clock();
    long long millis=(((long long)t)*1000)/CLOCKS_PER_SEC;
    long s=(millis-offset) % total;
   // l.report();
   // cout <<"s" <<s << endl;
    if (reverse) s=total-s;
    for (int i=0;i<pos;i++) {
      if (s<time[i]) {
        float fraction=(float)s/(float)time[i];
        if (!interp) fraction=0.0;
        int nf=fAngle[0];
        int nh=hAngle[0];
        int nk=kAngle[0];
        if (i+1<pos) {
         nf=fAngle[i+1];
         nh=hAngle[i+1];
         nk=kAngle[i+1];
        }
        int f=fAngle[i]+fraction*(nf-fAngle[i]);
        int h=hAngle[i]+fraction*(nh-hAngle[i]);
        int k=kAngle[i]+fraction*(nk-kAngle[i]);
        l.setPos(k,f,h);
        return;
      }
      s-=time[i];
    }
  }
  void rectangle(Point top_front,Point top_back,Point bottom_front,Point bottom_back,int time,bool invert) {
    Point angles;
    lg.compute(top_front,angles,invert);
    add(angles.a.knee,angles.a.femur,angles.a.hip,time/MAXPOS);
    top_front.reportPoint();
    angles.reportAngle();
    for (int i=0;i<MAXPOS-2;i++) {
	  	Point m;
	  	m=bottom_front.interp(i,MAXPOS-2,bottom_back);
	  	lg.compute(m,angles,invert);
	  	add(angles.a.knee,angles.a.femur,angles.a.hip,time/MAXPOS);
        m.reportPoint();
        angles.reportAngle();
	}
    lg.compute(top_back,angles,invert);
    add(angles.a.knee,angles.a.femur,angles.a.hip,time/MAXPOS);
    top_back.reportPoint();
    angles.reportAngle();
  }
  void write() {
  }
  virtual ~LegSequence() {
  }
};

class Crab1:public LegSequence {
public:
    Crab1(LegGeometry lg):LegSequence(lg) {
	}
	void init(int start,int total) {
		LegSequence::init(start,false,true);
		Point top_front   (10.0,10.0,10.0);
		Point top_back    ( 0.0,10.0,10.0);
		Point bottom_back ( 0.0,10.0,15.0);
		Point bottom_front(10.0,10.0,15.0);
		rectangle(top_front,top_back,bottom_front,bottom_back,total,false);
//		add(2048,2048,2048,3000/divider);
//		add(2748,2048,1548,333/divider);
//		add(2748,300,1548,333/divider);
//		add(2048,300,2048,333/divider);
	}
};
class Crab2:public LegSequence {
public:
    Crab2(LegGeometry lg):LegSequence(lg) {
	}
	void init(int offset,bool reverse,bool interp) {
		LegSequence::init(offset,reverse,true);
		add(2048,2048,2048,3000/divider);
		add(2748,2048,2548,333/divider);
		add(2748,300,2548,333/divider);
		add(2048,300,2048,333/divider);
	}
};
class Crab3:public LegSequence {
public:
    Crab3(LegGeometry lg):LegSequence(lg) {
	}
	void init(int offset,bool reverse,bool interp) {
		LegSequence::init(offset,reverse,true);
		add(2748,2048,1548,3000/divider);
		add(2048,2048,2048,333/divider);
		add(2048,300,2048,333/divider);
		add(2748,300,1548,333/divider);
	}
};
class Crab4:public LegSequence {
public:
    Crab4(LegGeometry lg):LegSequence(lg) {
	}
	void init(int offset,bool reverse,bool interp) {
		LegSequence::init(offset,reverse,true);
		add(2748,2048,2548,3000/divider);
		add(2048,2048,2048,333/divider);
		add(2048,300,2048,333/divider);
		add(2748,300,2548,333/divider);
	}
};

class Center:public LegSequence {
public:
    Center(LegGeometry lg):LegSequence(lg) {
	}
	void init(int offset,bool reverse,bool interp) {
		LegSequence::init(offset,reverse,interp);
		add(2048,2048,2048,1000);
	}
};


class Quad {
	Leg l1,l2,l3,l4;
	LegSequence *l1s,*l2s,*l3s,*l4s;
	Center *f1,*f2,*f3,*f4;
public:
	Quad() {
		f1=new Center(l1);
		f2=new Center(l2);
		f3=new Center(l3);
		f4=new Center(l4);
		f1->init(0,false,false);
		f2->init(0,false,false);
		f3->init(0,false,false);
		f4->init(0,false,false);
		setSequences(f1,f2,f3,f4);
	}
	LegGeometry leg1() {return l1;}
	LegGeometry leg2() {return l2;}
	LegGeometry leg3() {return l3;}
	LegGeometry leg4() {return l4;}
	void setSequences(LegSequence *newl1s,LegSequence *newl2s,LegSequence *newl3s,LegSequence *newl4s){
		l1s=newl1s;
		l2s=newl2s;
		l3s=newl3s;
		l4s=newl4s;
	}
	void init(int torque) {
	  l1.init(11,12,13,"l1");
	  l2.init(21,22,23,"l2");
	  l3.init(31,32,33,"l3");
	  l4.init(41,42,43,"l4");
	  l1.setTorque(torque);
	  l2.setTorque(torque);
	  l3.setTorque(torque);
	  l4.setTorque(torque);
	}
	void report() {
		l1.report();
		l2.report();
		l3.report();
		l4.report();
	}
    void setPosAll(int knee,int femur,int hip){
    	l1.setPos(knee,femur,hip);
    	l2.setPos(knee,femur,hip);
    	l3.setPos(knee,femur,hip);
    	l4.setPos(knee,femur,hip);
    }
    void programmed(){
    	l1s->pose(l1);
    	l2s->pose(l2);
    	l3s->pose(l3);
    	l4s->pose(l4);
    }
};

int main()
{
	try {
    Quad legs;
  //2   sleep(2);
    legs.init(255);
    Crab1 l1s(legs.leg1());
    Crab2 l2s(legs.leg2());
    Crab3 l3s(legs.leg3());
    Crab4 l4s(legs.leg4());
    l1s.init(0,4000);
    l2s.init(3000/divider,false,true);
    l3s.init(1000/divider,false,true);
    l4s.init(2000/divider,false,true);
    legs.setSequences(&l1s,&l2s,&l3s,&l4s);

	int k=2048;
	int f=2048;
	int h=2048;
	legs.setPosAll(k,f,h);
	while(1)
	{
		cout <<"Press Enter key to continue!(press q and Enter to quit)"<<endl;
		char key=getchar();
		if( key == 'q')
			break;
        if(key=='k')k-=100;
        if(key=='j')k+=100;
        if (k<0)k=0;
        if (k>4095)k=4095;
        if(key=='f')f-=100;
        if(key=='d')f+=100;
        if (f<0)f=0;
        if (f>4095)f=4095;
        if(key=='h')h-=100;
        if(key=='g')h+=100;
        if (h<0)h=0;
        if (h>4095)h=4095;
        if (key=='1') {
            k=4000;
            f=95;
            h=2048;
        }
        if (key=='2'){
          k=2648;
          f=2048;
          h=2048;
        }
        if (key=='3'){
          k=95;
          f=4000;
          h=2048;
        }
        if (key=='r'){
          legs.report();
        }
        if (key=='p'){
    	  clock_t t;
    	  t = clock();
    	  long long millis=(((long long)t)*1000)/CLOCKS_PER_SEC;
    	  long long start=millis;
          while ((millis-start)<20000) {
            legs.programmed();
            t = clock();
            millis=(((long long)t)*1000)/CLOCKS_PER_SEC;
          }
        }
        legs.setPosAll(k,f,h);
        key=' ';
		cout << "h:" <<h<<" f: "<<f<<" k:"<< k << endl;
	}
	} catch (DXL_ComError dce){
		dce.describe();
	}
	return 0;
}
