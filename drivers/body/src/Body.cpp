#include <iostream>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <memory>
#include <thread>

#include "config.h"
#include "CreateZMQServoController.h"
#include "BodyMessage.h"
#include "ZMQHub.h"

using namespace std;

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
  SpServoController controller;
  SpServo knee,femur,hip;
  string name;
  float kneeAngle,femurAngle,hipAngle;
  bool inverted;

public:

  void init(SPServoController &controller, int kneeid,int femurid,int hipid,string newName, bool inverted) {
    name=newName;
    knee = SPServo(controller->servo(kneeid));
    femur= SPServo(controller->servo(femurid));
    hip = SPServo(controller->servo(hipid));
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
    cout << name << ":" << " knee=" << knee.angle() << " femur=" << hip.angle() << " hip=" << hip.angle() << endl;
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
    if (sequence.size() >= 2) {
      float s= (t-t0)/T;
      s=s-floor(s);
      s=s*sequence.size();
      int i0=int(s);
      int i1=(i0+1) % size();
      float ds = s-floor(s);
      leg.setAngles(s[i0].interp(ds,s[i1]));
    } else if (sequence.size() == 1) {
      leg.setAngles(s[0]);
    } else {
      leg.setAngles(Point(0,0,0));
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

class Legs;

class LegsMovers
{
public:
  LegMover legMover1;
  LegMover legMover2;
  LegMover legMover3;
  LegMover legMover4;

  void move(double t, Legs &legs);
};

class Legs
{
  Leg leg1,leg2,leg3,leg4;

  Legs()
  {
    leg1.setMap(-1.0,0.0,0.0,1.0);// (x,y)=>(-x,y)
    leg1.setOrigin(-5.75,5.75,1.50);
    leg1.inverted = true;
    
    leg2.setMap(1.0,0.0,0.0,1.0);//  (x,y)=>(x,y)
    leg2.setOrigin(5.75,5.75,1.50);
    leg2.inverted = false;
    
    leg3.setMap(1.0,0.0,0.0,-1.0);// (x,y)=->(x,-y)
    leg3.setOrigin(5.75,-5.75,1.50);
    leg3.inverted = true;
    
    leg4.setMap(-1.0,0.0,0.0,-1.0);
    leg4.setOrigin(-5.75,-5.75,1.50);
    leg4.inverted = false;
  }

  void init(SPServoController &controller)
  {
    leg1.init(controller,
	      LEG1_SERVO_ID_KNEE,LEG1_SERVO_ID_FEMUR,LEG1_SERVO_ID_HIP,"leg1");
    leg2.init(controller,
	      LEG2_SERVO_ID_KNEE,LEG2_SERVO_ID_FEMUR,LEG2_SERVO_ID_HIP,"leg2");
    leg3.init(controller,
	      LEG3_SERVO_ID_KNEE,LEG3_SERVO_ID_FEMUR,LEG3_SERVO_ID_HIP,"leg3");
    leg4.init(controller,
	      LEG4_SERVO_ID_KNEE,LEG4_SERVO_ID_FEMUR,LEG4_SERVO_ID_HIP,"leg4");
  }

  void report() {
    leg1.report();
    leg2.report();
    leg3.report();
    leg4.report();
  }
};

class LegsMover::move(double t, Legs &legs)
{
  legMover1->move(t,legs->leg1);
  legMover2->move(t,legs->leg2);
  legMover3->move(t,legs->leg3);
  legMover4->move(t,legs->leg4);
}

class Body {
  Legs legs;
  shared_ptr<LegsMover> legsMover;
  SpServo waistServo;
  float waistAngle;
  SpServo neckLeftRightServo;
  float neckLeftRgihtAngle;
  SpServo neckUpDownServo;
  float neckUpDownAngle;
  
  void init(SpServoController controller)
  {
    waistServo=SpServo(controller->servo(WAIST_SERVO_ID));
    neckUpDown=SpServo(controller->servo(NECKUD_SERVO_ID));
    neckLeftRight=SpServo(controller->servo(NECKLR_SERVO_ID));
  }
  
  void move(double s)
  {
    legMover->move(s,legs);
    waistServo.angle(waistServoAngle);
    neckUpDownServo.angle(neckUpDownAngle);
    neckLeftRightServo.angle(neckLeftRightAngle);
  }
};

class BodyController : ZMQHub
{
  list < string > replies;
  shared_ptr < Body > body;

  double time;
  double speed;

  void answer(const string &reply)
  {
    replies.push_back(reply);
  }

  void answer(ostringstream &oss)
  {
    answer(oss.str());
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
      answer(oss << "load file '" << file << "' :" 
	     << (load(file) ? "ok" : "failed") << ".");
    }
    if (head == "time") {
      double value;
      iss >> value;
      t = value;
      answer(oss << "set time to " << value << ".");
    }
    if (head == "speed") {
      double value;
      iss >> value;
      speed = value;
      answer(oss << "set speed to " << value << ".");      
    }
  }

  void update()
  {
    double lastRealTime=now();
    while (running) {
      double thisRealTime = now();
      time += speed*(thisRealTime-lastRealTime);
      thisTime = lastTime;
      body->move(time);
    }
  }

  void rx(ZMQSubscribeSocket &socket)
  {
    ZMQMessage msg;
    msg.recv(socket);
    ZMQServoMessage *data = (ZMQServoMessage *)msg.data();
    string command((const char *)(data+1),data[0]);
    act(command);
  }

  void tx(ZMQPublishSocket &socket)
  {
    while (!replies.empty()) {
      string &reply = *replies.begin();
      uint8_t size = (reply.size() < BodyMessage::CAPACITY) ? reply.size() : BodyMessage::CAPACITY;
      ZMQMessage msg(size+1);
      ZMQBodyMessage *data = (ZMQServoMessage*)msg.data();
      uint8_t *contents = &data->contents;
      contents[0] = size;
      memcpy(contents+1,&reply[0],size);
      msg.send(socket);
      replies.pop_back();
    }
  }
  BodyController()
  {
    speed=1;
    time=0;
    go=0;
  }

  thread *goUpdate;

  void start()
  { 
    if (goUpdate != 0) goUpdate = new thead(&BodyController::update, this);
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

void run()
{
  shared_ptr < ServoController > 
    servoController(CreateZMQServoController(BODY_LISTEN,SERVOS_CONNECT));
  
  shared_ptr < Body > 
    body (new Body());

  shared_ptr < BodyController > 
    bodyController (new BodyController());

  body->init(servoController);
  bodyController->init(body);

  servoController.start();
  bodyController.start();

  bodyController.join();
}

int main()
{
  run();
  cout << "done" << endl;
  return 0;
}
