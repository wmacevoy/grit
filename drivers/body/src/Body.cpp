#include <iostream>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <memory>
#include <thread>

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

  void report()
  {
    cout << "servo " << name << " at " << angle() << endl;
  }
};



using namespace std;

const float ANGLE360=360.0;
const float ANGLE180=ANGLE360/2.0;
const float ANGLE90=ANGLE180/2.0;
const float ANGLE45=ANGLE90/2.0;

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
protected:
  // Multiplier for gear ration from full circle movement
  float g0,g1,g2;
  // Angular offset for 0
  float o0,o1,o2;
  float coordinateMap[2][2];
  float origin[3];
public:
  
  void setMap(float mxx=1.0,float mxy=0.0,float myx=0.0,float myy=1.0) {
    coordinateMap[0][0]=mxx;
    coordinateMap[1][0]=mxy;
    coordinateMap[0][1]=myx;
    coordinateMap[1][1]=myy;
  }
  void setOrigin(float x=0.0, float y=0.0,float z=0.0)
  {
    origin[0]=x;
    origin[1]=y;
    origin[2]=z;
  }
  
  LegGeometry() {
    koffset=2.25;
    ktibia=15.25;
    l0=2.375; //inches
    l1=8.25;
    l2=sqrt(ktibia*ktibia+koffset*koffset);
    //    kangle=acos(koffset*koffset+ktibia*ktibia-l2*l2)/(2.0*koffset*ktibia)*ANGLE180/M_PI;
    kangle = ANGLE180/M_PI*atan2(koffset,ktibia);
    g0=1.0;
    g1=4.0;
    g2=4.0;

    o0=180.0;
    o1=180.0;
    o2=180.0;
    setMap(); // The identity map
  }
  
  void compute3D(float x,float y,float z,float &knee,float &femur,float &hip,invert)
  {
    float nx=(x-origin[0])*coordinateMap[0][0]+(y-origin[1])*coordinateMap[0][1];
    float ny=(x-origin[0])*coordinateMap[0][1]+(y-origin[1])*coordinateMap[1][1];
    float nz=(z-origin[2]);

    float r=sqrt(nx*nx+ny*ny);
    float d=-nz;

    float ux=-2*(r-l0)*l1;
    float uy=-2*d*l1;

    float R=sqrt(ux*ux+uy*uy);
    float phi_rad=atan2(uy,ux);

    float theta1_rad = -phi_rad + acos((l2*l2-l1*l1-(r-l0)*(r-l0)-d*d)/R);
    float theta2_rad = -theta1_rad+asin((d-l1*sin(theta1_rad))/l2);

    knee = (ANGLE180/M_PI)*(-M_PI/2.0-theta2_rad)+kangle;
    femur = (ANGLE180/M_PI)*theta1_rad;
    hip=(ANGLE180/M_PI)*(atan2(ny,nx)-M_PI/4);
    
    if (invert) {
      hip = -hip;
    }
  }

  void compute3D(float x,float y,float z,float &knee,float &femur,float &hip)
  {
    compute3D(x,y,z,knee,femur,hip,inverted);
  }
};

class Leg: public LegGeometry {
  MyServo knee,femur,hip;
  string name;
  float kneeAngle,femurAngle,hipAngle;
  bool inverted;
public:

  void init(int kneeid,int femurid,int hipid,string newName) {
    name=newName;
    knee.init(kneeid,name+"k");
    femur.init(femurid,name+"f");
    hip.init(hipid,name+"h");
    inverted = false;
  }

  void setEnd(Point &tip)
  {
    float kneeAngle,femurAngle,hipAngle;
    compute3D(tip.x,tip.y,tip.z,kneeAngle,femurAngle,hipAngle);
    knee.angle(kneeAngle);
    femur.angle(femurAngle);
    hip.angle(hipAngle);
  }

  void setAngle(Point &angle)
  {
    knee.angle(angle.a.knee);
    femur.angle(angle.a.femur);
    hip.angle(angle.a.hip);
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

class LegMover
{
public:
  typedef vector < Point > Angles;
  Angles angles;

  Leg *leg;
  float t0,T;
  LegMover() {}

  void at(float t)
  {
    if (sequence.size() >= 2) {
      float s= (t-t0)/T;
      s=s-floor(s);
      s=s*sequence.size();
      int i0=int(s);
      int i1=(i0+1) % size();
      float ds = s-floor(s);
      leg->setAngles(s[i0].interp(ds,s[i1]));
    } else if (sequence.size() == 1) {
      leg->setAngles(s[0]);
    } else {
      leg->setAngles(Point(0,0,0));
    }
  }

  // re-sample t2tips uniformly in points periodic sample 
  void setupFromTips(const map < float , Point > &t2tips, int points = 20) {
    if (t2tips.size() == 0) {
      angles.clear();
      return;
    }
    if (t2tips.size() == 0) {
      t0=t2tips.begin()->first;
      t1=t0+1;
      T=t1-t0;
      angles.resize(points);
      
    }
    t0=t2tips.begin()->first;
    if (t2tips.size() > 1) {
      T=t2tips.rbegin()->first-t0;
    } else {
      T=1.0;
    }

    angles.resize(points);

    for (int i=0; i<points; ++i) {
      float s=t0+T*i/float(points);
      pair < float , Point > *prev=0, *next=0;
      for (map < float , Point > :: const_iterator i = t2tips.begin();
	   i != t2tips.end();
	   ++i) {

	prev = next;
	next = &*i;
	if (i->second > s) break;
      }
      assert(prev != 0); // shouldn't happen
      Point p=prev->second->interp((next->first-prev->first)/(T/points),next->second);
      leg->compute3D(p.p.x,p.p.y,p.p.z,angles[i].a.knee,angles[i].a.femur,angles[i].a.hip);
    }
  }

  void setupFromTips(float t0_,float T_,vector < Point > tips, int points = 20)
  {
    map < float , Point > t2tips;
    for (size_t i = 0; i <= tips.size(); ++i) {
      t2tips[t0+T*i/tips.size()]=tips[i % size()];
    }
    setupFromTips(t2tips);
  }
};

class Legs
{
  Leg leg1,leg2,leg3,leg4;
  LegMover legMover1,legMover2,legMover3,legMover4;

  void pointfile(const char *file)
  {

  }

  Legs()
  {
    leg1.setMap(-1.0,0.0,0.0,1.0);// (x,y)=>(-x,y)
    leg1.setOrigin(-5.75,5.75,1.50);
    leg1.inverted = true;
    legMover1.leg = &leg1;

    
    leg2.setMap(1.0,0.0,0.0,1.0);//  (x,y)=>(x,y)
    leg2.setOrigin(5.75,5.75,1.50);
    leg2.inverted = false;
    legMover2.leg = &leg2;
    
    leg3.setMap(1.0,0.0,0.0,-1.0);// (x,y)=->(x,-y)
    leg3.setOrigin(5.75,-5.75,1.50);
    leg3.inverted = true;
    legMover3.leg = &leg3;
    
    leg4.setMap(-1.0,0.0,0.0,-1.0);
    leg4.setOrigin(-5.75,-5.75,1.50);
    leg4.inverted = false;
    legMover4.leg = &leg4;

    l1.init(11,12,13,"leg1");
    l2.init(21,22,23,"leg2");
    l3.init(31,32,33,"leg3");
    l4.init(41,42,43,"leg4");
  }

  void report() {
    l1.report();
    l2.report();
    l3.report();
    l4.report();
  }
  
  void setAnglesAll(float knee,float femur,float hip) {
    l1.setAngles(knee,femur,hip);
    l2.setAngles(knee,femur,hip);
    l3.setAngles(knee,femur,hip);
    l4.setAngles(knee,femur,hip);
  }
  
  void at(float t)
  {
    l1m->at(t);
    l2m->at(t);
    l3m->at(t);
    l4m->at(t);
  }

  void pointfile(string filename)
  {
    
  }
};


class Quad {
  Legs legs;
  MyServo waistServo;
  float waistAngle;
  MyServo neckLeftRightServo;
  float neckLeftRgihtAngle;
  MyServo neckUpDownServo;
  float neckUpDownAngle;
  
  Quad() {
    waistServo.init(91,"waist");
    neckUpDown.init(93,"neckud");
    neckLeftRight.init(94,"necklr");
  }
  
  void at(double s)
  {
    legs.at(s);
    waistServo.angle(waistServoAngle);
    neckUpDownServo.angle(neckUpDownAngle);
    neckLeftRightServo.angle(neckLeftRightAngle);
  }
    
  void run()
  {
    double t0=now();
    double s=0;
    while (running) {
      if (!paused) {
	double t1=now();
	  float deltat=t1-t;
	  t=t1;
	  if (deltat < 1.0) {
	    s += speed*deltat;
	  }
      }
      at(s);
    }
  }

  thread *go;

  void start()
  {
    if (go != 0) {
      running = true;
      go = new thread(&Body::run,this);
    }
  }
};


int main()
{
  Body body;

  

    Quad legs;
  //2   sleep(2);
    legs.init(768); // initial torque

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
