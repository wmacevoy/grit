#include <iostream>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include "DynamixelDriver.hpp"
#include "PointFileReader.hpp"

using namespace std;


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

class LegGeometry {
	// Length of the members of the leg starting with hip
	float l0,l1,l2,zoffset,koffset,ktibia,kangle;
	float lcx,lcy;
	// Multiplier for gear ration from full circle movement
    protected:
	float g0,g1,g2;
	// Angular offset for 0
	float o0,o1,o2;
	float hipOffset;
 //   float coordinateMap[2][2];
    string name;
	public:
 /*   void setMap(float mxx=1.0,float mxy=0.0,float myx=0.0,float myy=1.0) {
  	  coordinateMap[0][0]=mxx;
  	  coordinateMap[1][0]=mxy;
  	  coordinateMap[0][1]=myx;
  	  coordinateMap[1][1]=myy;
    } */
    void setPosition(float newlcx,float newlcy) {
		lcx=newlcx;
		lcy=newlcy;
	}
	void setHipOffset(float newHipOffset) {
	  hipOffset=newHipOffset;
	}
	LegGeometry() {
	  hipOffset=0.0;
	  koffset=2.125;
	  ktibia=16.00;
	  l0=2.625; //inches
	  l1=8.25;
	  l2=sqrt(ktibia*ktibia+koffset*koffset);
//	  kangle=acos(koffset*koffset+ktibia*ktibia-l2*l2)/(2.0*koffset*ktibia)*ANGLE180/M_PI;
	  kangle=atan(koffset/ktibia)*ANGLE180/M_PI;
	  g0=1.0;
	  g1=4.0;
	  g2=4.0;
	  zoffset=1.335;
	  lcx=5.707; // distance from center of chassis to hip axis
	  lcy=5.707; // distance from center of chassis to hip axis
	//  o0=500.0;
	//  o1=-5700.0;
	//  o2=-1400.0;
	  o0=180.0; // degrees
	  o1=180.0;
	  o2=180.0;
	//  setMap(); // The identity map
	}
	void setName(string newName) {
	  name=newName;
	}
	void outputName(ostream &out) {
		out << name << endl;
	}
	float robustACos(float cosvalue) {
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
	} 
	// Do not worry about the hip rotation
	void compute2D(float x,float z,float &knee,float &femur) {
	  float d=sqrt(x*x+z*z); // distance from hip point in space
	  cout << "2D x="<<x << " z="<<z << " d="<<d<<endl;
	  cout << "l0="<<l0<<" l1="<<l1<<" l2="<<l2<<" kangle="<<kangle<<endl;
	  float theta1=robustACos((l1*l1+l2*l2-d*d)/(2.0*l1*l2))*ANGLE180/M_PI;
	  float theta2=robustACos((d*d+l1*l1-l2*l2)/(2.0*d*l1))*ANGLE180/M_PI;
//	  float theta3=acos((d*d+z*z-x*x)/(2.0*d*z))*ANGLE180/M_PI;
	  float theta3=fabs(atan(x/z))*ANGLE180/M_PI;
	  cout << "Theta 1:"<< theta1 << endl;
	  cout << "Theta 2:"<< theta2 << endl;	  
	  cout << "Theta 3:"<< theta3 << endl;
	  knee=theta1+(ANGLE90-kangle)-ANGLE180;
	  femur=(theta2+theta3)-ANGLE90;
	}
	void compute3D(float x,float y,float z,float &knee,float &femur,float &hip,bool invert) {
	  cout << "3D x="<<x << " y="<<y << " z="<<z<<endl;
//	  float nx=x*coordinateMap[0][0]+y*coordinateMap[0][1];
//	  float ny=x*coordinateMap[0][1]+y*coordinateMap[1][1];
	  float nx=x;
	  float ny=y;
	  
//	  z=-z;
	  x=nx-lcx;
	  y=ny-lcy;
	  cout << "x=" << x << "  y=" << y << endl;
	  float d=sqrt(x*x+y*y);
	  cout << "x/d" << (x/d)  << "acos " << acos(x/d) << endl;
	  if (y>0) 
	    hip=ANGLE180+(-acos(x/d)*ANGLE180/M_PI-ANGLE45);
	  else  
	    hip=-(ANGLE180+(-acos(x/d)*ANGLE180/M_PI-ANGLE45))-ANGLE90;
	  hip+=hipOffset;
	  if (hip<-ANGLE180) hip+=ANGLE360;  	    
	  compute2D(d-l0,z-zoffset,knee,femur);
/*	  if (invert) {
	    hip=ANGLE180-hip;
	  } */
	  cout << "knee=" << knee << " femur=" << femur << " hip=" << hip << endl;
	}
	void compute(Point &p,Point &joint,bool invert) {
	  compute3D(p.p.x,p.p.y,p.p.z,joint.a.knee,joint.a.femur,joint.a.hip,invert);
	}
};

class Leg:public LegGeometry {
	Servo knee,femur,hip;
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
	void outputName(ostream &out) {
		out << name << endl;
	}
	void report() {
		knee.report();
		femur.report();
		hip.report();
	}
};

const int MAXPOS=50;

class LegSequence {
  int hAngle[MAXPOS];
  int fAngle[MAXPOS];
  int kAngle[MAXPOS];
  long time[MAXPOS];
  protected:
  int pos;
  long total,offset;
  bool reverse,interp;
  LegGeometry *lg;
  public:
  LegSequence(LegGeometry *newLg) {
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
  void add(map<float, Point> positions) {
	int count=0;
	map<float,Point>::iterator it=positions.begin();
	cout << "Compute ";
	lg->outputName(cout);
	for (it=positions.begin();it!=positions.end() && count < MAXPOS;it++) { 
      Point angles;
      lg->compute(it->second,angles,false);
      add(angles.a.knee,angles.a.femur,angles.a.hip,it->first*1000.0);
      count++;
    }
  }
  void rectangle(Point top_front,Point top_back,Point bottom_front,Point bottom_back,int time,bool invert) {
    Point angles;
    lg->compute(top_front,angles,invert);
    add(angles.a.knee,angles.a.femur,angles.a.hip,time/MAXPOS);
    top_front.reportPoint(cout);
    angles.reportAngle(cout);
    for (int i=0;i<MAXPOS-2;i++) {
	  	Point m;
	  	m=bottom_front.interp(i,MAXPOS-2,bottom_back);
	  	lg->compute(m,angles,invert);
	  	add(angles.a.knee,angles.a.femur,angles.a.hip,time/MAXPOS);
        m.reportPoint(cout);
        angles.reportAngle(cout);
	}
    lg->compute(top_back,angles,invert);
    add(angles.a.knee,angles.a.femur,angles.a.hip,time/MAXPOS);
    top_back.reportPoint(cout);
    angles.reportAngle(cout);
  }
  void write() {
  }
  virtual ~LegSequence() {
  }
};

class Crab1:public LegSequence {
public:
    Crab1(LegGeometry *lg):LegSequence(lg) {
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
    Crab2(LegGeometry *lg):LegSequence(lg) {
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
    Crab3(LegGeometry *lg):LegSequence(lg) {
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
    Crab4(LegGeometry *lg):LegSequence(lg) {
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
    Dog1(LegGeometry *lg):LegSequence(lg) {
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
    Dog2(LegGeometry *lg):LegSequence(lg) {
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
    Dog3(LegGeometry *lg):LegSequence(lg) {
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
    Dog4(LegGeometry *lg):LegSequence(lg) {
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
    Center(LegGeometry *lg):LegSequence(lg) {
	}
	void init(int offset,bool reverse,bool interp) {
		LegSequence::init(offset,reverse,interp);
		add(0,0,0,1000);
	}
};

const float HIPAXISX=5.707;
const float HIPAXISY=5.707;
class Quad {
	Leg l1,l2,l3,l4;
	LegSequence *l1s,*l2s,*l3s,*l4s;
	Center *f1,*f2,*f3,*f4;
	float legDistance;
public:
	Quad() {
		legDistance=11.5;
		f1=new Center(&l1);
		f2=new Center(&l2);
		f3=new Center(&l3);
		f4=new Center(&l4);
		l1.setPosition(-HIPAXISX,HIPAXISY);
		l2.setPosition(HIPAXISX,HIPAXISY);
		l2.setHipOffset(-ANGLE90);
		l3.setPosition(HIPAXISX,-HIPAXISY);
		l3.setHipOffset(-ANGLE180);
		l4.setPosition(-HIPAXISX,-HIPAXISY);
		l4.setHipOffset(ANGLE90);
		f1->init(0,false,false);
		f2->init(0,false,false);
		f3->init(0,false,false);
		f4->init(0,false,false);
		setSequences(f1,f2,f3,f4);
	}
	Leg *leg1() {return &l1;}
	Leg *leg2() {return &l2;}
	Leg *leg3() {return &l3;}
	Leg *leg4() {return &l4;}
	void setSequences(LegSequence *newl1s,LegSequence *newl2s,LegSequence *newl3s,LegSequence *newl4s){
		l1s=newl1s;
		l2s=newl2s;
		l3s=newl3s;
		l4s=newl4s;
	}
	void init(int torque) {
	  l1.init(11,12,13,"Leg1");
	  l2.init(21,22,23,"Leg2");
	  l3.init(31,32,33,"Leg3");
	  l4.init(41,42,43,"Leg4");
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
	try {
	Servo w;
	w.init(91,"Waist");
	w.setTorque(315);
	w.joint(wpos);
	Servo n1;
	n1.init(93,"Waist");
	n1.setTorque(315);
	n1.joint(neckud);
	Servo n2;
	n2.init(94,"Waist");
	n2.setTorque(315);
	n2.joint(necklr);
    Quad legs;
  //2   sleep(2);
    legs.init(512);
    
	vector<map<float,Point> > data;
	data=PointFileReader::read("StraightGate.csv");
    cout << "Leg 1" << endl;
	PointFileReader::report(cout,data[0]);
    cout << "Leg 2" << endl;
	PointFileReader::report(cout,data[1]);
    cout << "Leg 3" << endl;
	PointFileReader::report(cout,data[2]);
    cout << "Leg 4" << endl;
	PointFileReader::report(cout,data[3]);
    LegSequence s1(legs.leg1());
    s1.add(data[0]);
    LegSequence s2(legs.leg2());
    s2.add(data[1]);
    LegSequence s3(legs.leg3());
    s3.add(data[2]);
    LegSequence s4(legs.leg4());
    s4.add(data[3]);
    
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
		if (key=='s') legs.setSequences(&s1,&s2,&s3,&s4);
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
            legs.leg1()->compute3D(x,y,z,k,f,h,false);
            legs.leg1()->setPos(k,f,h);
            legs.leg2()->compute3D(-x,y,z,k,f,h,false);
            legs.leg2()->setPos((int)k,(int)f,(int)h);
            legs.leg3()->compute3D(-x,-y,z,k,f,h,false);
            legs.leg3()->setPos((int)k,(int)f,(int)h);
            legs.leg4()->compute3D(x,-y,z,k,f,h,false);
            legs.leg4()->setPos((int)k,(int)f,(int)h);
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
	} catch (DXL_ComError dce){
		dce.describe();
	}
	return 0;
}
