#include <csignal>
#include <signal.h>
#include <iostream>
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
#include "now.h"

using namespace std;

#include <mutex>

class Lock
{
 public:
  std::mutex &m;
  inline Lock(std::mutex &m_) : m(m_) { m.lock(); }
  inline ~Lock() { m.unlock(); }
};

typedef shared_ptr < Servo > SPServo;
typedef shared_ptr < ServoController > SPServoController;

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
protected:
  // Multiplier for gear ration from full circle movement
  float g0,g1,g2;
  // Angular offset for 0
  float o0,o1,o2;
  float coordinateMap[2][2];
  float origin[3];
  bool inverted;
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
  void setInverted(bool inverted_)
  {
    inverted=inverted_;
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
  
  void compute3D(float x,float y,float z,float &knee,float &femur,float &hip)
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
    
    if (inverted) {
      hip = -hip;
    }
  }
};

class Leg : public LegGeometry {
  SPServo knee,femur,hip;
  string name;
  float kneeAngle,femurAngle,hipAngle;
  bool inverted;

public:

  void init(SPServoController &controller, int kneeid,int femurid,int hipid,string newName) {
    name=newName;
    knee = SPServo(controller->servo(kneeid));
    
    femur= SPServo(controller->servo(femurid));
    hip = SPServo(controller->servo(hipid));
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

  void report() {
    cout << name << ":" << " knee=" << knee->angle() << " femur=" << femur->angle() << " hip=" << hip->angle() << endl;
  }
};

class LegMover
{
public:
  typedef vector < Point > Angles;
  Angles angles;

  float t0,T;
  LegMover() {}

  void move(float t, Leg &leg)
  {
    if (angles.size() >= 2) {
      float s= (t-t0)/T;
      s=s-floor(s);
      s=s*angles.size();
      int i0=int(s);
      int i1=(i0+1) % angles.size();
      float ds = s-floor(s);
      leg.setAngles(angles[i0].interp(ds,angles[i1]));
    } else if (angles.size() == 1) {
      leg.setAngles(angles[0]);
    } else {
      leg.setAngles(Point(0,0,0));
    }
  }

  // re-sample t2tips uniformly in points periodic sample 
  void setupFromTips(Leg &leg, const map < float , Point > &t2tips, int points = 20) {
    if (t2tips.size() == 0) {
      angles.clear();
      return;
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
      const pair < const float , Point > *prev=0, *next=0;
      for (map < float , Point > :: const_iterator i = t2tips.begin();
	   i != t2tips.end();
	   ++i) {

	prev = next;
	next = &*i;
	if (i->first > s) break;
      }
      assert(prev != 0); // shouldn't happen
      Point p=prev->second.interp((next->first-prev->first)/(T/points),next->second);
      leg.compute3D(p.p.x,p.p.y,p.p.z,angles[i].a.knee,angles[i].a.femur,angles[i].a.hip);
    }
  }

  void setupFromTips(Leg &leg, float t0_,float T_,vector < Point > tips, int points = 20)
  {
    map < float , Point > t2tips;
    for (size_t i = 0; i <= tips.size(); ++i) {
      t2tips[t0+T*i/tips.size()]=tips[i % tips.size()];
    }
    setupFromTips(leg,t2tips);
  }
};

class Legs
{
public:
  Leg legs[4];

  Legs()
  {
    legs[0].setMap(-1.0,0.0,0.0,1.0);// (x,y)=>(-x,y)
    legs[0].setOrigin(-5.75,5.75,1.50);
    legs[0].setInverted(true);
    
    legs[1].setMap(1.0,0.0,0.0,1.0);//  (x,y)=>(x,y)
    legs[1].setOrigin(5.75,5.75,1.50);
    legs[1].setInverted(false);
    
    legs[2].setMap(1.0,0.0,0.0,-1.0);// (x,y)=->(x,-y)
    legs[2].setOrigin(5.75,-5.75,1.50);
    legs[2].setInverted(true);
    
    legs[3].setMap(-1.0,0.0,0.0,-1.0);
    legs[3].setOrigin(-5.75,-5.75,1.50);
    legs[3].setInverted(false);
  }

  void init(SPServoController &controller)
  {
    legs[0].init(controller,
	      LEG1_SERVO_ID_KNEE,LEG1_SERVO_ID_FEMUR,LEG1_SERVO_ID_HIP,"leg1");
    legs[1].init(controller,
	      LEG2_SERVO_ID_KNEE,LEG2_SERVO_ID_FEMUR,LEG2_SERVO_ID_HIP,"leg2");
    legs[2].init(controller,
	      LEG3_SERVO_ID_KNEE,LEG3_SERVO_ID_FEMUR,LEG3_SERVO_ID_HIP,"leg3");
    legs[3].init(controller,
	      LEG4_SERVO_ID_KNEE,LEG4_SERVO_ID_FEMUR,LEG4_SERVO_ID_HIP,"leg4");
  }

  void report() {
    for (int i=0; i<4; ++i) {
      legs[i].report();
    }
  }
};

class LegsMover
{
public:
  LegMover legMovers[4];

  void move(double t, Legs &legs)
  {
    for (int i=0; i<4; ++i ) {
      legMovers[i].move(t,legs.legs[i]);
    }
  }

  void setupFromTips(Legs &legs, const map < float , Point > *t2tips, int points = 20) {
    for (int i=0; i<4; ++i) {
      legMovers[i].setupFromTips(legs.legs[i],t2tips[i],points);
    }
  }

};


class Body {
public:
  Legs legs;
  shared_ptr<LegsMover> legsMover;
  SPServo waistServo;
  float waistAngle;
  SPServo neckLeftRightServo;
  float neckLeftRightAngle;
  SPServo neckUpDownServo;
  float neckUpDownAngle;
  
  void init(SPServoController controller)
  {
    legs.init(controller);
    waistServo=SPServo(controller->servo(WAIST_SERVO_ID));
    neckUpDownServo=SPServo(controller->servo(NECKUD_SERVO_ID));
    neckLeftRightServo=SPServo(controller->servo(NECKLR_SERVO_ID));
  }
  
  void move(double s)
  {
    legsMover->move(s,legs);
    waistServo->angle(waistAngle);
    neckUpDownServo->angle(neckUpDownAngle);
    neckLeftRightServo->angle(neckLeftRightAngle);
  }
};

class BodyController : public ZMQHub
{
public:
  list < string > replies;
  mutex repliesMutex;

  shared_ptr < Body > body;
  double time;
  double speed;

  void answer(const string &reply)
  {
    Lock lock(repliesMutex);
    replies.push_back(reply);
  }

  void answer(ostringstream &oss)
  {
    answer(oss.str());
  }

  bool load(const string &file)
  {
    vector<vector<double>> data;
    string headers = "Time (seconds),x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4";
    if (!CSVRead(file,headers,data)) {
      return false;
    }

    // add last row to finish cycle
    data.push_back(data[0]);
    int nr=data.size();
    data[nr-1][0]=data[nr-2][0]+(data[nr-2][0]-data[nr-3][0]);

    map < float , Point > t2tips[4];

    for (size_t r=0; r<data.size(); ++r) {
      for (int el=0; el<4; ++el) {
	t2tips[el][data[r][0]]=Point(data[r][1+3*el],
				     data[r][2+3*el],
				     data[r][3+3*el]);
      }
    }

    body->legsMover->setupFromTips(body->legs,t2tips);
    return true;
  }

					    

  void act(string &command)
  {
    istringstream iss(command);
    ostringstream oss;

    string head;
    iss >> head;
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
      time = value;
      oss << "set time to " << value << ".";
      answer(oss.str());
    }
    if (head == "speed") {
      double value;
      iss >> value;
      speed = value;
      oss << "set speed to " << value << ".";
      answer(oss.str());
    }
  }

  void update()
  {
    double lastRealTime=now();
    while (running) {
      double thisRealTime = now();
      time += speed*(thisRealTime-lastRealTime);
      lastRealTime = thisRealTime;
      body->move(time);
    }
  }

  void rx(ZMQSubscribeSocket &socket)
  {
    ZMQMessage msg;
    msg.recv(socket);
    char *data = (char *)msg.data();
    string command((const char *)(data+1),data[0]);
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
    speed=1;
    time=0;
    goUpdate=0;
  }

  thread *goUpdate;

  void start()
  { 
    if (goUpdate != 0) goUpdate = new thread(&BodyController::update, this);
    ZMQHub::start();
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

};

ZMQHub *hub=0;

void SigIntHandler(int arg) {
  hub->stop();
}


void run()
{
  shared_ptr < ServoController > 
    servoController(CreateZMQServoController(BODY_LISTEN,SERVOS_CONNECT));
  
  shared_ptr < Body > 
    body (new Body());

  shared_ptr < BodyController > 
    bodyController (new BodyController());

  body->init(servoController);
  bodyController->body = body;

  servoController->start();
  bodyController->start();
  hub=&*bodyController;
  signal(SIGINT, SigIntHandler);
  bodyController->join();
}

int main()
{
  run();
  cout << "done" << endl;
  return 0;
}
