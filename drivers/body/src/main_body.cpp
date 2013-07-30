#include <csignal>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <memory>
#include <thread>
#include <mutex>
#include <map>
#include <list>
#include <assert.h>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <memory>
#include <string.h>

#include "config.h"

#include "CreateZMQServoController.h"
#include "ZMQHub.h"
#include "CSVRead.h"
#include "BodyMessage.h"
#include "Lock.h"
#include "now.h"

using namespace std;

#include <mutex>

double sim_time;
double sim_speed;
double sim_torque;

typedef shared_ptr < Servo > SPServo;
typedef shared_ptr < ServoController > SPServoController;

const float MOVE_RATE=50.0;

const float ANGLE360=360.0;
const float ANGLE180=ANGLE360/2.0;
const float ANGLE90=ANGLE180/2.0;
const float ANGLE45=ANGLE90/2.0;
const float HIPAXISX=5.707;
const float HIPAXISY=5.707;

const int LEG1=0;
const int LEG2=1;
const int LEG3=2;
const int LEG4=3;

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
  Point interp(float t,const Point &b) const {
    Point newp(t*(b.p.x-p.x)+p.x,t*(b.p.y-p.y)+p.y,t*(b.p.z-p.z)+p.z);
    return newp;
  }
  Point interp(int i,int max,const Point &b) const {
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
	float lcx,lcy;
    protected:
	float hipOffset;
    string name;
	public:
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
	  kangle=atan(koffset/ktibia)*ANGLE180/M_PI;
	  zoffset=1.335;
	  lcx=5.707; // distance from center of chassis to hip axis
	  lcy=5.707; // distance from center of chassis to hip axis
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
	void compute3D(float x,float y,float z,float &knee,float &femur,float &hip) {
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
	void compute(Point &p,Point &joint) {
	  compute3D(p.p.x,p.p.y,p.p.z,joint.a.knee,joint.a.femur,joint.a.hip);
	}
};

class Leg : public LegGeometry {
public:
  SPServo knee,femur,hip;
  string name;
  float kneeAngle,femurAngle,hipAngle;
  bool inverted;

  void init(SPServoController &controller, const map<string,int> &servoMap,string newName) {
    name=newName;

    {
      string part=name;
      part.append("_KNEE");
      map<string,int>::const_iterator i=servoMap.find(part);
      assert(i != servoMap.end());
      int id=i->second;
      knee = SPServo(controller->servo(id));
    }

    {
      string part=name;
      part.append("_FEMUR");
      map<string,int>::const_iterator i=servoMap.find(part);
      assert(i != servoMap.end());
      int id=i->second;
      femur = SPServo(controller->servo(id));
    }

    {
      string part=name;
      part.append("_HIP");
      map<string,int>::const_iterator i=servoMap.find(part);
      assert(i != servoMap.end());
      int id=i->second;
      hip = SPServo(controller->servo(id));
    }
  }

  void setEnd(const Point &tip)
  {
    float kneeAngle,femurAngle,hipAngle;
    compute3D(tip.p.x,tip.p.y,tip.p.z,kneeAngle,femurAngle,hipAngle);
    knee->angle(kneeAngle);
    femur->angle(femurAngle);
    hip->angle(hipAngle);
  }

  void setAngles(const Point &p)
  {
    knee->angle(p.a.knee);
    femur->angle(p.a.femur);
    hip->angle(p.a.hip);
  }

  void setSpeeds(const Point &p)
  {
    knee->speed(p.a.knee);
    femur->speed(p.a.femur);
    hip->speed(p.a.hip);
  }

  void setTorques(const Point &p)
  {
    knee->torque(p.a.knee);
    femur->torque(p.a.femur);
    hip->torque(p.a.hip);
  }

  void report() {
    cout << name << ":" << " knee=" << knee->angle() << " femur=" << femur->angle() << " hip=" << hip->angle() << endl;
  }
};

void fit0(double t[3],float p[3],float c[3])
{
  if (fabs(t[0]-t[1]) > 0.001) {
    if (fabs(t[1]-t[2]) > 0.001) {
      c[0]=p[1];
      c[2]=2.0*((p[0]-p[1])/(t[0]-t[1])-(p[2]-p[1])/(t[2]-t[1]))/(t[2]-t[0]);
      c[1]=(p[0]-p[1])/(t[0]-t[1])-c[2]*(t[0]-t[1])/2.0;
      if (fabs((t[2]-t[0])*c[2]) > fabs(c[1])) {
	c[2]=0;
	c[1]=(p[0]-p[1])/(t[0]-t[1]);
      }
    } else {
      c[0]=p[1];
      c[2]=0;
      c[1]=(p[0]-p[1])/(t[0]-t[1]);
    }
  } else {
    c[0]=p[1];
    c[1]=0.0;
    c[2]=0.0;
  }
}

void fit(double ts[3],float p[3],float c0[3], float c1[3])
{
  fit0(ts,p,c0);
  ts[0]=-ts[0];
  ts[1]=-ts[1];
  ts[2]=-ts[2];
  fit0(ts,p,c1);
  c1[1]=-c1[1];
  ts[0]=-ts[0];
  ts[1]=-ts[1];
  ts[2]=-ts[2];
}

class ServoMover
{
public:
  typedef map < float , float > Angles;
  Angles angles;
  std::mutex anglesMutex;
  Angles::iterator at;
  bool loop;

  float t0,T;

  ServoMover()
  {
    loop = true;
  }

  void setLoop(bool doLoop) { loop=doLoop; }

  void move(float simTime,float realTime, Servo &servo)
  {
    Lock lock(anglesMutex);

    double ts[3];
    float p[3],c0[3],c1[3];

    if (angles.size() >= 2) {
      float s= (simTime-t0)/T;
      if (!loop && s>=1.0) s=1.0;
      s=s-floor(s);
      s=t0+T*s;

      while (at != angles.end() && s > at->first) ++at;
      if (at == angles.end()) at=angles.begin();
      while (at != angles.begin() && s < at->first) --at;
      float oldTime = at->first;
      float oldAngle = at->second;
      Angles::iterator after(at);
      if (++after == angles.end()) after=angles.begin();
      float newTime = after->first;
      if (newTime < oldTime) newTime += T;
      float newAngle = after->second; 
      if (++after == angles.end()) at=angles.begin();
      float newTime2 = after->first;
      if (newTime2 < oldTime) newTime2 += T;
      float newAngle2 = after->second;

      double realTimeNow  = realTime;

      double lambda = (fabs(sim_speed) > 0.1) ? 1/sim_speed : 10.0;
      ts[0] = lambda*(oldTime-s) + realTimeNow;
      ts[1] = lambda*(newTime-s) + realTimeNow;
      ts[2] = lambda*(newTime2-s) + realTimeNow;

	
      p[0]=oldAngle;
      p[1]=newAngle;
      p[2]=newAngle2;
      fit(ts,p,c0,c1);
      servo.curve(ts+1,c0,c1);
      servo.torque(sim_torque);
    } else if (angles.size() == 1) {
      servo.angle(angles.begin()->second);
      servo.speed(15);
      servo.torque(sim_torque);
    } else {
      servo.angle(0);
      servo.speed(15);
      servo.torque(sim_torque);
    }
  }

  void setup(const map < float , float > &angles_) {
    Lock lock(anglesMutex);
    angles=angles_;

    at=angles.begin();
    if (angles.size() == 0) {
      t0=0;
      T=1.0;
    } else {
      t0=angles.begin()->first;
      if (angles.size() > 1) {
	T=angles.rbegin()->first-t0;
      } else {
	T=1.0;
      }
    }
  }
};

class LegMover
{
public:
  typedef map < float , Point > Angles;
  Angles angles;
  std::mutex anglesMutex;
  Angles::iterator at;
  bool loop;

  float t0,T;

  LegMover() { loop=true;}
  
  void setLoop(bool doLoop) { loop=doLoop; }
  void move(float simTime, float realTime, Leg &leg)
  {
    Lock lock(anglesMutex);

    if (angles.size() >= 2) {
      float s= (simTime-t0)/T;
      if (!loop && s>=1.0) s=1.0;
      s=s-floor(s);
      s=t0+T*s;

      while (at != angles.end() && s > at->first) ++at;
      if (at == angles.end()) at=angles.begin();
      while (at != angles.begin() && s < at->first) --at;
      float oldTime = at->first;
      const Point &oldAngle = at->second;
      Angles::iterator after(at);
      if (++after == angles.end()) after=angles.begin();
      float newTime = after->first;
      if (newTime < oldTime) newTime += T;
      const Point &newAngle = after->second; 
      if (++after == angles.end()) at=angles.begin();
      float newTime2 = after->first;
      if (newTime2 < oldTime) newTime2 += T;
      const Point &newAngle2 = after->second;

      //      cout << "body move s=" << s << "oldt=" << oldTime << " newTime=" << newTime << " newTime2=" << newTime2 << endl;

      double realTimeNow  = realTime;
      double ts[3];


      double lambda = (fabs(sim_speed) > 0.1) ? 1/sim_speed : 10.0;
      ts[0] = lambda*(oldTime-s) + realTimeNow;
      ts[1] = lambda*(newTime-s) + realTimeNow;
      ts[2] = lambda*(newTime2-s) + realTimeNow;

      float curves0[3][3],curves1[3][3];
      float p[3];

      p[0]=oldAngle.a.knee;
      p[1]=newAngle.a.knee;
      p[2]=newAngle2.a.knee;
      fit(ts,p,curves0[0],curves1[0]);

      p[0]=oldAngle.a.femur;
      p[1]=newAngle.a.femur;
      p[2]=newAngle2.a.femur;
      fit(ts,p,curves0[1],curves1[1]);

      p[0]=oldAngle.a.hip;
      p[1]=newAngle.a.hip;
      p[2]=newAngle2.a.hip;
      fit(ts,p,curves0[2],curves1[2]);
      
      leg.knee->curve(ts+1,curves0[0],curves1[0]);
      leg.femur->curve(ts+1,curves0[1],curves1[1]);
      leg.hip->curve(ts+1,curves0[2],curves1[2]);
      leg.setTorques(Point(1.0,1.0,1.0));
    } else if (angles.size() == 1) {
      leg.setAngles(angles[0]);
      leg.setTorques(Point(sim_torque,sim_torque,sim_torque));
      leg.setSpeeds(90.0);
    } else {
      leg.setSpeeds(0);
      leg.setTorques(Point(sim_torque,sim_torque,sim_torque));
      leg.setAngles(Point(0,0,0));
    }
  }

  void setupFromTips(Leg &leg, const map < float , Point > &t2tips) {
    Lock lock(anglesMutex);
    if (t2tips.size() == 0) {
      angles.clear();
      at=angles.begin();
      return;
    }
    t0=t2tips.begin()->first;
    if (t2tips.size() > 1) {
      T=t2tips.rbegin()->first-t0;
    } else {
      T=1.0;
    }

    angles.clear();
    
    for (map < float , Point > :: const_iterator i = t2tips.begin();
	 i != t2tips.end();
	 ++i) {
      const Point &p=i->second;
      Point a;
      leg.compute3D(p.p.x,p.p.y,p.p.z,a.a.knee,a.a.femur,a.a.hip);
      angles[i->first]=a;
    }
    at=angles.begin();
  }

  void report(ostream &out, Leg &leg)
  {
    string name = leg.name;
    out << name << "t" 
	 << "," << name << "knee" << "," << name << "femur"<< "," << name << "hip"
	 << "," << name << "vknee" << "," << name << "vfemur"<< "," << name << "vhip"  << endl;

    for (size_t i=0; i<angles.size(); ++i) {
      double t = t0+double(i)/angles.size()*T;
      size_t i1=(i+1) % angles.size();
      double dt = T/angles.size();
      out << t 
	   << "," << angles[i].a.knee 
	   << "," << angles[i].a.femur 
	   << ","<< angles[i].a.hip
	   << "," << (angles[i1].a.knee-angles[i].a.knee)/dt
	   << "," << (angles[i1].a.femur-angles[i].a.femur)/dt
	   << ","<< (angles[i1].a.hip-angles[i].a.hip)/dt << endl;
    }
    out << endl;

  }
};

class Legs
{
public:
  Leg legs[4];

  Legs()
  {

    legs[LEG1].setPosition(-HIPAXISX,HIPAXISY);
    legs[LEG2].setPosition(HIPAXISX,HIPAXISY);
    legs[LEG2].setHipOffset(-ANGLE90);
    legs[LEG3].setPosition(HIPAXISX,-HIPAXISY);
    legs[LEG3].setHipOffset(-ANGLE180);
    legs[LEG4].setPosition(-HIPAXISX,-HIPAXISY);
    legs[LEG4].setHipOffset(ANGLE90);
  }

  void init(SPServoController &controller, const map<string,int> &servoMap)
  {
      legs[LEG1].init(controller,servoMap,"LEG1");
      legs[LEG2].init(controller,servoMap,"LEG2");
      legs[LEG3].init(controller,servoMap,"LEG3");
      legs[LEG4].init(controller,servoMap,"LEG4");
  }

};

class LegsMover
{
public:
  LegMover legMovers[4];

  void move(double simTime, double realTime, Legs &legs)
  {
    for (int i=0; i<4; ++i ) {
      legMovers[i].move(simTime,realTime,legs.legs[i]);
    }
  }
  void setLoop(bool doLoop) {
    for (int i=0; i<4; ++i ) legMovers[i].setLoop(doLoop);
  }

  void setupFromTips(Legs &legs, const map < float , Point > *t2tips) 
  {
    for (int i=0; i<4; ++i) {
      legMovers[i].setupFromTips(legs.legs[i],t2tips[i]);
    }
  }

  void report(ostream &out, Legs &legs)
  {
    for (int i=0; i<4; ++i) {
      legMovers[i].report(out,legs.legs[i]);
    }
  }

  LegsMover()
  {
  }

};


class Body {
public:
  Legs legs;
  shared_ptr<LegsMover> legsMover;
  SPServo waistServo;
  shared_ptr<ServoMover> waistMover;
  SPServo neckLeftRightServo;
  float neckLeftRightAngle;
  SPServo neckUpDownServo;
  float neckUpDownAngle;


  void init(SPServoController controller, const map<string,int> &servoMap)
  {
    neckLeftRightAngle=0;
    neckUpDownAngle=0;
    legs.init(controller,servoMap);

    {
      map<string,int>::const_iterator i=servoMap.find("WAIST");
      assert(i != servoMap.end());
      waistServo=SPServo(controller->servo(i->second));
      waistServo->angle(0);
      waistServo->speed(15);
      waistServo->torque(0.700);
    }

    
    {
      map<string,int>::const_iterator i=servoMap.find("NECKUD");
      assert(i != servoMap.end());
      neckUpDownServo=SPServo(controller->servo(i->second));
      neckUpDownServo->speed(45);
      neckUpDownServo->angle(neckUpDownAngle);
      neckUpDownServo->torque(0.700);
    }


    {
      map<string,int>::const_iterator i=servoMap.find("NECKLR");
      assert(i != servoMap.end());
      neckLeftRightServo=SPServo(controller->servo(i->second));
      neckLeftRightServo->speed(45);
      neckLeftRightServo->angle(neckLeftRightAngle);
      neckLeftRightServo->torque(0.700);
    }

    legsMover = shared_ptr <LegsMover> ( new LegsMover () );
    waistMover = shared_ptr <ServoMover > ( new ServoMover() );
  }
  
  void move(double simTime,double realTime)
  {
    legsMover->move(simTime,realTime,legs);
    waistMover->move(simTime,realTime,*waistServo);
    neckLeftRightServo->angle(5*cos(6.28*simTime/10.0));
    neckUpDownServo->angle(neckUpDownAngle);
    neckLeftRightServo->angle(neckLeftRightAngle);
  }

  void report(std::ostream &out)
  {
    legsMover->report(out,legs);
  }
};

class BodyController : public ZMQHub
{
public:
  list < string > replies;
  mutex repliesMutex;

  shared_ptr < Body > body;

  void answer(const string &reply)
  {
    Lock lock(repliesMutex);
    replies.push_back(reply);
  }

  void answer(ostringstream &oss)
  {
    answer(oss.str());
  }
  
  void setPitch(float angle) {
	  body->neckUpDownAngle=angle;
  }
  
  void setYaw(float angle) {
	  body->neckLeftRightAngle=angle;
  }
  
  void setWaist(float angle) {
    map < float , float > waist;
    waist[0]=angle;
    waist[1]=angle;
    body->waistMover->setup(waist);
  }
  
  bool load(const string &file)
  {
    vector<vector<double>> data;
    //                             0  1  2  3  4  5  6  7  8  9 10 11 12 13
    string headers = "Time (seconds),x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,waist";
    if (!CSVRead(file,headers,data)) {
      return false;
    }

    cout << "read '" << file << "' ok." << endl;

    // add last row to finish cycle
    data.push_back(data[0]);
    int nr=data.size();
    data[nr-1][0]=data[nr-2][0]+(data[nr-2][0]-data[nr-3][0]);

    map < float , Point > t2tips[4];
    map < float , float > t2waist;

    for (size_t r=0; r<data.size(); ++r) {
      for (int el=0; el<4; ++el) {
	float t=data[r][0];
	t2tips[el][t]=Point(data[r][1+3*el],
				     data[r][2+3*el],
				     data[r][3+3*el]);
	t2waist[t]=data[r][13];
      }
      sim_time = 0; // reset to sync with t=0 in gait.
    }

    body->legsMover->setupFromTips(body->legs,t2tips);
    body->waistMover->setup(t2waist);
    return true;
  }
  
  void noLoop() {
	body->legsMover->setLoop(false);
  }
  void loop() {
	body->legsMover->setLoop(true);
  }

  void act(string &command)
  {
    istringstream iss(command);
    ostringstream oss;

    string head;
    iss >> head;
    if (head == "report") {
      string file;
      iss >> file;
      ofstream fout(file.c_str());
      body->report(fout);
      oss << "report sent to file " << file;
      answer(oss.str());
    }
    if (head == "loop") {
	  loop();
	}
	if (head == "once") {
	  noLoop();
	}
    if (head == "home") {
	  load("home.csv");
      ostringstream oss;
      oss << "played Home script"; 
      answer(oss.str());
    }
    if (head == "walk") {
	  load("Gait1_3.csv");
      ostringstream oss;
      oss << "played Gait1_3 script"; 
      answer(oss.str());
    }
    if (head == "headPitch") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setPitch(angle);
      oss << "headPitch " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "headYaw") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setYaw(angle);
      oss << "headYaw " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "waist") {
      float angle;
      iss >> angle;
      ostringstream oss;
      setWaist(angle);
      oss << "waist " << angle << " :ok."; 
      answer(oss.str());
    }
    if (head == "load") {
      string file;
      iss >> file;
      ostringstream oss;
      oss << "load file '" << file << "' :" 
	  << (load(file) ? "ok" : "failed") << ".";
      answer(oss.str());
    }
    if (head == "time") {
      double value;
      iss >> value;
      sim_time = value;
      oss << "set time to " << value << ".";
      answer(oss.str());
    }
    if (head == "speed") {
      double value;
      iss >> value;
      sim_speed = value;
      oss << "set speed to " << value << ".";
      answer(oss.str());
    }
    if (head == "torque") {
      double value;
      iss >> value;
      sim_torque = value;
      oss << "set torque to " << value << ".";
      answer(oss.str());
    }
  }

  void update()
  {
    double rho=0.01;
    double lastRealTime=now();
    double delta_bar=0;
    double delta2_bar=0;
    double max_delta=0;
    while (running) {
      usleep(int((1.0/MOVE_RATE)*1000000));
      double thisRealTime = now();
      sim_time += sim_speed*(thisRealTime-lastRealTime);
      if (floor(thisRealTime) != floor(lastRealTime)) {
	cout << "delta=" << delta_bar << " sigma=" << sqrt(delta2_bar - delta_bar*delta_bar) << " max=" << max_delta << endl;
	max_delta=0;
      }
      thisRealTime = now();
      lastRealTime = thisRealTime;
      body->move(sim_time,thisRealTime+1.0/MOVE_RATE);
      double delta = now()-thisRealTime;
      delta_bar = (1-rho)*delta_bar + rho*delta;
      delta2_bar = (1-rho)*delta2_bar + rho*delta*delta;
      if (delta > max_delta) max_delta=delta;

      if (delta > delta_bar + 3*sqrt(delta2_bar - delta_bar*delta_bar)) {
	cout << "slow at sim_time = " << sim_time << " real_time=" << thisRealTime << endl;
      }
    }
  }

  void rx(ZMQSubscribeSocket &socket)
  {
    ZMQMessage msg;
    msg.recv(socket);
    char *data = (char *)msg.data();
    string command((const char *)(data+1),data[0]);
    cout << "got: " << command << endl;
    act(command);
  }

  void tx(ZMQPublishSocket &socket)
  {
    Lock lock(repliesMutex);

    while (!replies.empty()) {
      string &reply = *replies.begin();
      uint8_t size = (reply.size() < BODY_MESSAGE_MAXLEN) ? 
	reply.size() : BODY_MESSAGE_MAXLEN;

      ZMQMessage msg(size+1);
      char *data = (char *)msg.data();
      data[0]=size;
      memcpy(data+1,&reply[0],size);
      msg.send(socket);
      replies.pop_back();
    }
  }

  BodyController()
  {
    sim_speed=1;
    sim_torque = 0.70;
    sim_time=0;
    goUpdate = 0;
  }

  thread *goUpdate;

  void start()
  { 
    if (!running) {
      ZMQHub::start();
      goUpdate = new thread(&BodyController::update, this);
    }
  }

  void join()
  {
    ZMQHub::join();
    if (goUpdate != 0) { 
      goUpdate->join(); 
      delete goUpdate; 
      goUpdate = 0; 
    }    
  }

  ~BodyController()
  {
    stop();
    BodyController::join();
  }

};

ZMQHub *hub=0;

void SigIntHandler(int arg) {
  hub->stop();
}

struct App
{
  shared_ptr < ServoController > servoController;
  shared_ptr < Body > body;
  shared_ptr < BodyController > bodyController;
  vector<string> bodyServosSubscribers;
  string bodyServosPublish;
  map < string , int > servoMap;

  void subscribers(vector<string> &subscribers, string arg, char sep=',')
  {
    subscribers.clear();
    while (arg.length() > 0) {
      size_t comma = arg.find(sep);
      string subscriber = arg.substr(0,(comma != string::npos) ? comma : arg.length());
      subscribers.push_back(subscriber);
      arg=arg.substr((comma != string::npos) ? comma+1 : arg.length());
    }
  }

  void configure(const string &config_csv)
  {
    vector < vector < string > > values;
    if (!CSVRead(config_csv,"name,value",values)) {
      cout << "Could not read configuration file '" 
	   << config_csv << "'." << endl;
      exit(1);
    }

    map<string,string> cfg;
    for (size_t i=0; i != values.size(); ++i) {
      cfg[values[i][0]]=values[i][1];
    }

    { 
      map<string,string>::iterator property=
	cfg.find("body.servos.publish");
      if (property != cfg.end()) {
	bodyServosPublish = property->second;
      }
    }

    {
      map<string,string>::iterator property=
	cfg.find("body.servos.subscribers");
      if (property != cfg.end()) {
	subscribers(bodyServosSubscribers,property->second,';');
      }
    }
    
    {
      map<string,string>::iterator property=
	cfg.find("body.commander.publish");
      if (property != cfg.end()) {
	bodyController->publish = property->second;
      }
    }

    {
      map<string,string>::iterator property=
	cfg.find("body.commander.subscribers");
      if (property != cfg.end()) {
	subscribers(bodyController->subscribers,property->second);
      }
    }

    {
      map<string,string>::iterator property=
	cfg.find("servos.map");
      if (property != cfg.end()) {
	vector < vector < string > > csvServoMap;
	if (CSVRead(property->second,"name,id",csvServoMap)) {
	  for (size_t i=0; i<csvServoMap.size(); ++i) {
	    servoMap[csvServoMap[i][0]]=atoi(csvServoMap[i][1].c_str());
	  }
	}
      }
    }
  }

  void args(int argc, char *argv[])
  {
    for (int argi=1; argi<argc; ++argi) {
      if (strcmp(argv[argi],"--configure") == 0) {
	configure(argv[++argi]);
	continue;
      }
      
      if (strcmp(argv[argi],"--servo.publish") == 0) {
	bodyServosPublish = argv[++argi];
	continue;
      }
      
      if (strcmp(argv[argi],"--servo.subscribers") == 0) {
	subscribers(bodyServosSubscribers,argv[++argi]);
	continue;
      }
      
      if (strcmp(argv[argi],"--body.publish") == 0) {
	bodyController->publish = argv[++argi];
	continue;
      }
      
      if (strcmp(argv[argi],"--body.subscribers") == 0) {
	subscribers(bodyController->subscribers,argv[++argi]);
	continue;
      }
      cout << "unkown arg '" << argv[argi] << "' ignored."  << endl;
    }
    
    if (argc == 1) {
      configure(CONFIG_CSV);
    }
    
    assert(bodyServosSubscribers.size() == 1);
    servoController = shared_ptr<ServoController>(CreateZMQServoController(bodyServosPublish,bodyServosSubscribers[0]));
  }

  void run(int argc, char *argv[])
  {
    body = shared_ptr <Body> (new Body());
    bodyController = shared_ptr <BodyController> (new BodyController());
    args(argc,argv);
    body->init(servoController,servoMap);
    bodyController->body = body;

    servoController->start();
    bodyController->start();
    hub=&*bodyController;
    signal(SIGINT, SigIntHandler);
    bodyController->join();
  }
};

int main(int argc, char *argv[])
{
  App app;
  app.run(argc,argv);
  cout << "done" << endl;
  return 0;
}
