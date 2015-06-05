#include <sstream>
//#include <strstream>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <map>
#include <thread>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <zmq.h>
#include <string.h>

#include "ServoGlobals.h"
#include "ServoController.h"
#include "Configure.h"
#include "CRC16.h"

#include "GritIO.h"

#include "now.h"

#define USE_TORQUE_ENABLED 1
#define SPEED_FACTOR 1.1

using namespace std;

struct GritServo : Servo
{
public:
  GritIO io;
  int id;

  int presentPosition;
  float minAngle;
  float maxAngle;
  int goalPosition;
  int goalSpeed;
  float minSpeed;
  float maxSpeed;
  bool enabled;
  bool curveMode;
  double t[2];
  float c0[3],c1[3];

public:

  void torque(float value) {};
  void rate(float value) {};
  uint8_t temp() const { return 0; }

  GritServo(GritIO &io_, int id_)
    : io(io_), id(id_), presentPosition(2048), goalPosition(2048) 
  {
    enabled=true;
    minSpeed = atof(cfg.servo(id,"minspeed").c_str());
    maxSpeed = atof(cfg.servo(id,"maxspeed").c_str());
    minAngle = atof(cfg.servo(id,"minangle").c_str());
    maxAngle = atof(cfg.servo(id,"maxangle").c_str());

    angle(0.0);
    speed(atof(cfg.servo(id,"minspeed").c_str()));
    curveMode = false;

    if (verbose) {
      cout << "GritServo(id=" << id << ",minSpeed=" << minSpeed << ",maxSpeed=" << maxSpeed << ",minAngle=" << minAngle << ",maxAngle=" << maxAngle << ")" << endl;
    }
  }
  
  void curve(double t_[2], float c0_[3],float c1_[3])
  {
    curveMode = true;
    t[0]=t_[0];
    t[1]=t_[1];
    c0[0]=c0_[0];
    c0[1]=c0_[1];
    c0[2]=c0_[2];
    c1[0]=c1_[0];
    c1[1]=c1_[1];
    c1[2]=c1_[2];
  }

  float angle() const { 
    return presentPosition;
  }

  void angle0(float value) {
    if (value < minAngle || !isfinite(value)) value = minAngle;
    else if (value > maxAngle) value = maxAngle;
    std::cout << "GritServo::angle0(id=" << id << ",value=" << value << ")" << std::endl;
    goalPosition = value;
  }

  void angle(float value) {
    curveMode = false;
    angle0(value);
  }

  void speed(float signedValue) {
    int value = ::abs(signedValue);
    if (value < minSpeed || !isfinite(value)) { value = minSpeed; }
    else if (value > maxSpeed) { value = maxSpeed; }
    goalSpeed = copysign(value,signedValue);
    std::cout << "GritServo::speed(id=" << id << " value=" << signedValue << ")=" << goalSpeed << std::endl;
  }

  float speed() const {
    return goalSpeed;
  }

  void update()
  {
    uint8_t outmsg[6];
    outmsg[0] = 'g';
    outmsg[1] = id;
#if 1
    outmsg[2] = goalPosition;
    outmsg[3] = safety->safe() ? 0 : 128+goalSpeed;
#else
    std::cout << "DEBUG!!!" << std::endl;
    outmsg[2] = 90;
    outmsg[3] = 160;
#endif
    (*(uint16_t*)&outmsg[4])=CRC16(outmsg,4);
    //    outmsg[3] = (goalSpeed >> 1);
    std::cout << "curveMode: " << curveMode << " write: [" << int(outmsg[0]) << "," << int(outmsg[1]) << "," << int(outmsg[2]) << "," << int(outmsg[3]) << "," << int(outmsg[4]) << "," << int(outmsg[5]) << "]" << std::endl;

#if 1
    io.write(sizeof(outmsg),outmsg);
#else
#warning no write to grit servos
    std::cout << "skip write to grit servos." << std::endl;
#endif

#if 0
    uint8_t inmsg[3];
    if (io.read(sizeof(inmsg),inmsg) == sizeof(inmsg)) {
      //      std::cout << "read: [" << int(inmsg[0]) << "," << int(inmsg[1]) <<"," << int(inmsg[2]) << "]" << std::endl;      
      presentPosition = inmsg[1];
    } else {
      std::cout << "read: []" << std::endl;
    }
#endif
  }

  ~GritServo()
  {
  }
};

struct GritServoController : ServoController
{
  GritIO io;

  typedef std::map < int , std::shared_ptr <GritServo> > Servos;
  Servos servos;
  bool running;
  double txRate;
  Servo* servo(int id) {
    Servos::iterator i = servos.find(id);
    if (i != servos.end()) return &*i->second;
    assert(running == false);

    return &*(servos[id] = 
	      std::shared_ptr <GritServo> (new GritServo(io,id)));
  }

  void update() {
    while (running) {
      double t = now();
      double t1=t+1.0/txRate;

      for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
	if (k->second->enabled) {
	  GritServo *servo = &*k->second;
	  if (servo->curveMode) {
	    double dt;
	    if (t < servo->t[1]) {
	      dt = t-servo->t[0];
	    } else {
	      dt = servo->t[1]-servo->t[0];
	    }

	    float speed;
	    {
	      float *c = (dt <= 0) ? servo->c0 : servo->c1;
#if USE_SERVO_LINEAR == 1
	      //	      angle = c[0];
	      speed = c[1];

#else
	      //	      angle = c[0]+c[1]*dt+c[2]*dtsq/2.0;
	      speed = c[1]+c[2]*dt;
#endif
	    }

	    double dt1;
	    if (t1 < servo->t[1]) {
	      dt1 = t-servo->t[0];
	    } else {
	      dt1 = servo->t[1]-servo->t[0];
	    }

	    float angle1;
	    float speed1;
	    {
	      double dt1sq = dt1*dt1;
	      float *c = (dt1 <= 0) ? servo->c0 : servo->c1;
#if USE_SERVO_LINEAR == 1
	      angle1 = c[0];
	      speed1 = c[1];
#else
	      angle1 = c[0]+c[1]*dt1+c[2]*dt1sq/2.0;
	      speed1 = c[1]+c[2]*dt1;
#endif
	    }

	    servo->angle0(angle1);
	    float speedIn = max(fabs(speed),fabs(speed1));
	    servo->speed(speedIn);
	  }
	}
	k->second->update();
      }

      for (Servos::iterator i = servos.begin(); i != servos.end(); ++i) {
	if (i->second->curveMode) {
	  if (t < i->second->t[0] && t1 > i->second->t[0]+0.0001) {
	    t1=i->second->t[0]+0.0001;
	  }
	}
      }

      t=now();
      if (t1 > t) {
	usleep(int((t1-t)*1000000));
      }
    }
  }

  std::thread *go;

  void start()
  {
    if (go == 0) {
      running = true;
      go = new thread(&GritServoController::update,this);
    }
  }

  void stop()
  {
    if (go != 0) {
      running = false;
      go->join();
      delete go;
      go=0;
    }
  }

  GritServoController(int deviceIndex, int baudNum)
    : io(deviceIndex,baudNum), running(false), go(0)
  {
    txRate=cfg.num("servos.grit.rate.tx");
  }

  ~GritServoController()
  {
    stop();
  }
};

ServoController* CreateGritServoController(int deviceIndex,int baudNum)
{
  return new GritServoController(deviceIndex,baudNum);
}
