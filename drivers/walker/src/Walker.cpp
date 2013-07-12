#include <iostream>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <memory>
// #include "DynamixelDriver.h"
#include "CreateZMQServoController.h"

using namespace std;

const char *ME = "tcp://*:5502";
const char *SERVER = "tcp://localhost:5500";

class MyController : public ServoController
{
public:
  shared_ptr<ServoController> me;
  MyController() : me(CreateZMQServoController(ME,SERVER)) {}
  Servo* servo(int id) { return me->servo(id); }
  void start() { cout << "start mycontroller"; me->start(); }
};
MyController controller;

class MyServo : public Servo
{
private:
  //  MyServo(const MyServo &copy) {}
public:
  shared_ptr<Servo> me;
  string name;

  void init(int id_, const string &name_)
  {
    me = shared_ptr<Servo>(controller.servo(id_));
    name=name_;
  }

  float angle() const { return me->angle(); }
  void angle(float value) { me->angle(value); }

  void joint(float value) {
    angle((value-2048)*(180.0/2048.0));
  }

  void setTorque(float value) { 
    // ignored
  }

  void report()
  {
    cout << "servo " << name << " at " << angle() << endl;
  }
};

const int divider=1;

using namespace std;

const int XCOORD=0;
const int YCOORD=1;
const float MAXPOSITION=4095.0;
const float ANGLE360=360.0;
const float ANGLE180=ANGLE360/2.0;
const float ANGLE90=ANGLE180/2.0;
const float ANGLE45=ANGLE90/2.0;
const float MINPOSITION=0;
const int MAXLIMIT=4046;
const int MINLIMIT=50;

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
	// Length of the members of the leg starting with hip
	float l0,l1,l2,zoffset,koffset,ktibia,kangle;
	// Multiplier for gear ration from full circle movement
    protected:
	float g0,g1,g2;
	// Angular offset for 0
	float o0,o1,o2;
    float coordinateMap[2][2];
	public:
    void setMap(float mxx=1.0,float mxy=0.0,float myx=0.0,float myy=1.0) {
  	  coordinateMap[0][0]=mxx;
  	  coordinateMap[1][0]=mxy;
  	  coordinateMap[0][1]=myx;
  	  coordinateMap[1][1]=myy;
    }
	LegGeometry() {
	  koffset=2.25;
	  ktibia=15.25;
	  l0=2.375; //inches
	  l1=8.25;
	  l2=sqrt(ktibia*ktibia+koffset*koffset);
	  kangle=acos(koffset*koffset+ktibia*ktibia-l2*l2)/(2.0*koffset*ktibia)*ANGLE180/M_PI;
	  g0=1.0;
	  g1=4.0;
	  g2=4.0;
	  zoffset=1.25;
	//  o0=500.0;
	//  o1=-5700.0;
	//  o2=-1400.0;
	  o0=180.0; // degrees
	  o1=180.0;
	  o2=180.0;
	  setMap(); // The identity map
	}
/*	float robustACos(float cosvalue) {
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
	} */
	// Do not worry about the hip rotation
	void compute2D(float x,float z,float &knee,float &femur) {
	  float d=sqrt(x*x+z*z); // distance from hip point in space
	  cout << "2D x="<<x << " z="<<z << " d="<<d<<endl;
	  cout << "l0="<<l0<<" l1="<<l1<<" l2="<<l2<<" kangle="<<kangle<<endl;
	  float theta1=acos((l1*l1+l2*l2-d*d)/(2.0*l1*l2))*ANGLE180/M_PI-kangle;
	  float theta2=acos((d*d+l1*l1-l2*l2)/(2.0*d*l1))*ANGLE180/M_PI;
	  float theta3=acos((d*d+z*z-x*x)/(2.0*d*z))*ANGLE180/M_PI;
	  cout << "Theta 1:"<< theta1 << endl;
	  cout << "Theta 2:"<< theta2 << endl;	  
	  cout << "Theta 3:"<< theta3 << endl;
	  knee=(ANGLE90-theta1);
	  femur=-(theta2+theta3-ANGLE90);
	}
	void compute3D(float x,float y,float z,float &knee,float &femur,float &hip,bool invert) {
	  cout << "3D x="<<x << " y="<<y << " z="<<z<<endl;
	  float nx=x*coordinateMap[0][0]+y*coordinateMap[0][1];
	  float ny=x*coordinateMap[0][1]+y*coordinateMap[1][1];
	  x=nx;
	  y=ny;
	  float d=sqrt(x*x+y*y);
	  hip=-(ANGLE90-acos(x/d)*ANGLE180/M_PI-ANGLE45);
	  compute2D(d-l0,z+zoffset,knee,femur);
	  if (invert) {
	    hip=ANGLE180-hip;
	  }
	}
	void compute(Point &p,Point &joint,bool invert) {
	  compute3D(p.p.x,p.p.y,p.p.z,joint.a.knee,joint.a.femur,joint.a.hip,invert);
	}
};

class Leg:public LegGeometry {
	MyServo knee,femur,hip;
	string name;
	int kneePos,femurPos,hipPos;
public:
	void init(int kneeid,int femurid,int hipid,string newName) {
	  name=newName;
	  knee.init(kneeid,name+"k");
	  femur.init(femurid,name+"f");
	  hip.init(hipid,name+"h");
	}
	void setPos(int newKnee=0,int newFemur=0,int newHip=0) {
		  cout << "Knee Angle:"<<newKnee<<endl;
		  cout << "Femur Angle:"<<newFemur<<endl;
		  cout << "Hip Angle:"<<newHip<<endl;	  hipPos  =(newHip  *g0+o0)*MAXPOSITION/ANGLE360;
	  femurPos=(newFemur*g1+o1)*MAXPOSITION/ANGLE360;
	  kneePos =(newKnee *g2+o2)*MAXPOSITION/ANGLE360;
	  cout << "Knee:"<<kneePos<<endl;
	  cout << "Femur:"<<femurPos<<endl;
	  cout << "Hip:"<<hipPos<<endl;
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

const int MAXPOS=20;

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
    long s=(millis+offset) % total;
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
		Point top_front   (8.0,8.0,6.0);
		Point top_back    (8.0,0.0,6.0);
		Point bottom_back (8.0,0.0,13.0);
		Point bottom_front(8.0,8.0,13.0);
		rectangle(top_front,top_back,bottom_front,bottom_back,total,false);
	}
};
class Crab2:public LegSequence {
public:
    Crab2(LegGeometry lg):LegSequence(lg) {
	}
	void init(int start,int total) {
		LegSequence::init(start,false,true);
		Point top_front   (-8.0,8.0,6.0);
		Point top_back    (-8.0,0.0,6.0);
		Point bottom_back (-8.0,0.0,13.0);
		Point bottom_front(-8.0,8.0,13.0);
		rectangle(top_front,top_back,bottom_front,bottom_back,total,false);
	}
};
class Crab3:public LegSequence {
public:
    Crab3(LegGeometry lg):LegSequence(lg) {
	}
	void init(int start,int total) {
		LegSequence::init(start,false,true);
		Point top_front   (-8.0,0.0,6.0);
		Point top_back    (-8.0,8.0,6.0);
		Point bottom_back (-8.0,8.0,13.0);
		Point bottom_front(-8.0,0.0,13.0);
		rectangle(top_front,top_back,bottom_front,bottom_back,total,false);
	}
};
class Crab4:public LegSequence {
public:
    Crab4(LegGeometry lg):LegSequence(lg) {
	}
	void init(int start,int total) {
		LegSequence::init(start,false,true);
		Point top_front   (8.0,0.0,6.0);
		Point top_back    (8.0,-8.0,6.0);
		Point bottom_back (8.0,-8.0,13.0);
		Point bottom_front(8.0,0.0,13.0);
		rectangle(top_front,top_back,bottom_front,bottom_back,total,false);
	}
};

class Dog1:public LegSequence {
public:
    Dog1(LegGeometry lg):LegSequence(lg) {
	}
	void init(int start,int total) {
		LegSequence::init(start,false,true);
		Point top_front   (0.0,11.0,6.0);
		Point top_back    (0.0,1.0,6.0);
		Point bottom_back (0.0,1.0,13.0);
		Point bottom_front(0.0,11.0,13.0);
		rectangle(top_front,top_back,bottom_front,bottom_back,total,false);
	}
};
class Dog2:public LegSequence {
public:
    Dog2(LegGeometry lg):LegSequence(lg) {
	}
	void init(int start,int total) {
		LegSequence::init(start,false,true);
		Point top_front   (0.0,11.0,6.0);
		Point top_back    (0.0,1.0,6.0);
		Point bottom_back (0.0,1.0,13.0);
		Point bottom_front(0.0,11.0,13.0);
		rectangle(top_front,top_back,bottom_front,bottom_back,total,false);
	}
};
class Dog3:public LegSequence {
public:
    Dog3(LegGeometry lg):LegSequence(lg) {
	}
	void init(int start,int total) {
		LegSequence::init(start,false,true);
		Point top_front   (0.0,-1.0,6.0);
		Point top_back    (0.0,-13.0,6.0);
		Point bottom_back (0.0,-13.0,13.0);
		Point bottom_front(0.0,-1.0,13.0);
		rectangle(top_front,top_back,bottom_front,bottom_back,total,false);
	}
};
class Dog4:public LegSequence {
public:
    Dog4(LegGeometry lg):LegSequence(lg) {
	}
	void init(int start,int total) {
		LegSequence::init(start,false,true);
		Point top_front   (0.0,-1.0,6.0);
		Point top_back    (0.0,-13.0,6.0);
		Point bottom_back (0.0,-13.0,13.0);
		Point bottom_front(0.0,-1.0,13.0);
		rectangle(top_front,top_back,bottom_front,bottom_back,total,false);
	}
};


class Center:public LegSequence {
public:
    Center(LegGeometry lg):LegSequence(lg) {
	}
	void init(int offset,bool reverse,bool interp) {
		LegSequence::init(offset,reverse,interp);
		add(0,0,0,1000);
	}
};


class Quad {
	Leg l1,l2,l3,l4;
	LegSequence *l1s,*l2s,*l3s,*l4s;
	Center *f1,*f2,*f3,*f4;
	float legDistance;
public:
	Quad() {
		legDistance=11.5;
		f1=new Center(l1);
		f2=new Center(l2);
		f3=new Center(l3);
		f4=new Center(l4);
		l1.setMap(); // The identity x<=x,  y<=y
		l2.setMap(0.0,-1.0,1.0,0.0); // x<=-y, y<=x
		l3.setMap(-1.0,0.0,0.0,-1.0); // x<=-x, y<=-y
		l4.setMap(0.0,1.0,-1.0,0.0); // x<=y, y<=-x
		f1->init(0,false,false);
		f2->init(0,false,false);
		f3->init(0,false,false);
		f4->init(0,false,false);
		setSequences(f1,f2,f3,f4);
	}
	Leg leg1() {return l1;}
	Leg leg2() {return l2;}
	Leg leg3() {return l3;}
	Leg leg4() {return l4;}
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
	bool angleMode=true;
	int wpos=2048-1200;
	int necklr=2048;
	int neckud=2048;
	/*	try { */
	MyServo w;
	w.init(91,"Waist");
	w.setTorque(315);
	w.joint(wpos);
	MyServo n1;
	n1.init(93,"Waist");
	n1.setTorque(315);
	n1.joint(neckud);
	MyServo n2;
	n2.init(94,"Waist");
	n2.setTorque(315);
	n2.joint(necklr);
    Quad legs;
  //2   sleep(2);
    legs.init(768);
    Crab1 l1s(legs.leg1());
    Crab2 l2s(legs.leg2());
    Crab3 l3s(legs.leg3());
    Crab4 l4s(legs.leg4());
    Dog1 d1s(legs.leg1());
    Dog2 d2s(legs.leg2());
    Dog3 d3s(legs.leg3());
    Dog4 d4s(legs.leg4());
    int cycle=8000;
    l1s.init(0,cycle);
    l2s.init(cycle/4,cycle);
    l3s.init(3*cycle/4,cycle);
    l4s.init(cycle/2,cycle);
    d1s.init(0,cycle);
    d2s.init(3*cycle/4,cycle);
    d3s.init(cycle/4,cycle);
    d4s.init(cycle/2,cycle);
    legs.setSequences(&l1s,&l2s,&l3s,&l4s);

	float k=0;
	float f=0;
	float h=0;
	float x=8;
	float y=8;
	float z=13;
	int step=5;
	legs.setPosAll(k,f,h);

    controller.start();
	while(1)
	{
		cout <<"Press Enter key to continue!(press q and Enter to quit)"<<endl;
		char key=getchar();
		if( key == 'q')
			break;
		if (key=='w') {
			wpos+=100;
			if (wpos>4095) wpos=4095;
			w.joint(wpos);
			cout <<"Waist "<<wpos<<endl;
		}
		if (key=='W') {
			wpos-=100;
			if (wpos<0) wpos=0;
			w.joint(wpos);
		}
		if (key=='u') {
			neckud+=100;
			if (neckud>4095) neckud=4095;
			n2.joint(neckud);
		}
		if (key=='U') {
			neckud-=100;
			if (neckud<0) neckud=0;
			n2.joint(neckud);
		}
		if (key=='l') {
			necklr+=100;
			if (necklr>4095) necklr=4095;
			n1.joint(necklr);
		}
		if (key=='L') {
			necklr-=100;
			if (necklr<0) necklr=0;
			n1.joint(necklr);
		}
		if (key=='A') angleMode=true;
		if (key=='a') angleMode=false;
		if (key=='c') legs.setSequences(&l1s,&l2s,&l3s,&l4s);
		if (key=='d') legs.setSequences(&d1s,&d2s,&d3s,&d4s);
		if (angleMode) {
          if(key=='k')k-=step;
          if(key=='K')k+=step;
          if (k<=-45)k=-44;
          if (k>=45)k=44;
          if(key=='f')f-=step;
          if(key=='F')f+=step;
          if (f<=-45)f=-44;
          if (f>=45)f=44;
          if(key=='h')h-=step;
          if(key=='H')h+=step;
          if (h<=-90)h=-89;
          if (h>=90)h=89;
          if (key=='0') {
            k=0;
            f=0;
            h=0;
          }
          if (key=='1') {
              k=44;
              f=-44;
              h=0;
          }
          if (key=='2'){
            k=10;
            f=0;
            h=0;
          }
          if (key=='3'){
            k=-44;
            f=44;
            h=0;
          }
          legs.setPosAll(k,f,h);
		} else {
			if (key=='x') x+=2;
			if (key=='X') x-=2;
			if (key=='y') y+=2;
			if (key=='Y') y-=2;
			if (key=='z') z+=2;
			if (key=='Z') z-=2;
            legs.leg1().compute3D(x,y,z,k,f,h,false);
            legs.leg1().setPos(k,f,h);
            legs.leg2().compute3D(-x,y,z,k,f,h,false);
            legs.leg2().setPos((int)k,(int)f,(int)h);
            legs.leg3().compute3D(-x,-y,z,k,f,h,false);
            legs.leg3().setPos((int)k,(int)f,(int)h);
            legs.leg4().compute3D(x,-y,z,k,f,h,false);
            legs.leg4().setPos((int)k,(int)f,(int)h);
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
        key=' ';
		cout << "h:" <<h<<" f: "<<f<<" k:"<< k << endl;
		cout << "x:" <<x<<" y: "<<y<<" z:"<< z << endl;
	}
	/* } catch (DXL_ComError dce){
		dce.describe();
		} */
	return 0;
}
