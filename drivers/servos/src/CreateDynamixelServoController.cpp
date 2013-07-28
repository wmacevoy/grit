#include <sstream>
#include <iostream>
#include <assert.h>
#include <map>
#include <thread>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "config.h"
#include "ServoController.h"

#define USE_BROADCAST 1

#if USE_BROADCAST == 1
#include <dynamixel.h>
#endif

#include "now.h"

// http://support.robotis.com/en -- MX-106t (legs)  MX-28t (neck & fingers)
//
// http://support.robotis.com/en/product/dynamixel/ax_series/dxl_ax_actuator.htm
#include "DXLIO.h"

using namespace std;

const int UPDATE_RATE = 20;



struct DynamixelServo : Servo
{
  DXLIO &io;
  int id;
  int presentPosition;
  int presentSpeed;
  int presentTorque;
  int goalPosition;
  int goalSpeed;
  int goalTorque;

#if SERVO_CURVE == 1
  bool curveMode;
  double t0;
  float c0[3],c1[3];
#endif

  DynamixelServo(DXLIO &io_, int id_) 
    : io(io_),id(id_), presentPosition(2048), goalPosition(2048) 
  {
    goalSpeed = 0;
    presentSpeed = 0;
    goalTorque = .70;
    presentTorque = 0;
    goalPosition = 0;
    io.writeWord(id,DXL_TORQUE_WORD,int(goalTorque*1023));
#if SERVO_CURVE == 1
    curveMode = false;
#endif
#if USE_BROADCAST != 1
    update();
#endif
  }

#if SERVO_CURVE == 1
  void curve(double t0_, float c0_[3],float c1_[3])
  {
    curveMode = true;
    t0=t0_;
    c0[0]=c0_[0];
    c0[1]=c0_[1];
    c0[2]=c0_[2];
    c1[0]=c1_[0];
    c1[1]=c1_[1];
    c1[2]=c1_[2];

    //    cout << "dynamixel curve" << " servo=" << id << " t0=" << t0 << " c0=[" << c0[0] << "," << c0[1] << "," << c0[2] << "]" << " c1=[" << c1[0] << "," << c1[1] << "," << c1[2] << "]"  << endl;
  }
#endif

  float angle() const { 
    return (180.0/2048)*(presentPosition-2048); 
  }

  void angle0(float value) {
    goalPosition = value*(2048/180.0)+2048;
    //    cout << "dynamixel servo=" << id << " goal position=" << goalPosition << endl;
  }
  void angle(float value) {
#if SERVO_CURVE == 1
    curveMode = false;
#endif
    angle0(value);
  }

  void speed(float value) {
    // speed for MX-110t (not MX-28t)
    //    cout << "id=" << id << " set speed = " << value << endl;
    goalSpeed = fabs(value)*(60.0/360.0)*(1023/117.07);
    if (goalSpeed > 480) goalSpeed = 480;
  }

  float speed() const {
    return presentSpeed/((60.0/360.0)*(1023/117.07));
  }

  void torque(float value) {
    //    cout << "id=" << id << " set torque = " << value << endl;
    goalTorque = fabs(value)*(1023);
    if (goalTorque > 1023) goalTorque = 1023;
  }

  float torque() const {
    return presentTorque/1023.0;
  }

  void tx()
  {
    io.writeWord(id,DXL_GOAL_POSITION_WORD,(goalPosition & 4095));
    io.writeWord(id,DXL_MOVING_SPEED_WORD,goalSpeed);
    io.writeWord(id,DXL_TORQUE_WORD,goalTorque);
  }

  void rx()
  {
    int inp;
    if (io.readWord(id,DXL_PRESENT_POSITION_WORD,&inp)) {
      presentPosition = inp;
    } else {
      cout << "comm rx position error" << endl;
    }
    if (io.readWord(id,DXL_PRESENT_SPEED_WORD,&inp)) {
      presentSpeed = inp;
    } else {
      cout << "comm rx speed error" << endl;
    }
    if (io.readWord(id,DXL_PRESENT_LOAD_WORD,&inp)) {
      presentTorque = inp;
    } else {
      cout << "comm rx load error" << endl;
    }
  }

  void update()
  {
    tx();
    rx();
  }

  ~DynamixelServo()
  {
    io.writeWord(id,DXL_TORQUE_WORD,1);
  }
};

struct DynamixelServoController : ServoController
{
  DXLIO io;
  typedef std::map < int , std::shared_ptr <DynamixelServo> > Servos;
  Servos servos;
  bool running;
  Servo* servo(int id) {
    Servos::iterator i = servos.find(id);
    if (i != servos.end()) return &*i->second;
    assert(running == false);

    return &*(servos[id] = 
	      std::shared_ptr <DynamixelServo> (new DynamixelServo(io,id)));
  }

  void update() {
    while (running) {
      usleep(int((1.0/UPDATE_RATE)*1000000));
#if USE_BROADCAST
      {
#if SERVO_CURVE == 1
	double t=now();
#endif
	io.reopen(); // reopen if failing recently...

	int N = servos.size();
	int L = 6; // total data payload for position + speed + torque

	dxl_set_txpacket_id(BROADCAST_ID);
	dxl_set_txpacket_instruction(INST_SYNC_WRITE);
	dxl_set_txpacket_parameter(0, DXL_GOAL_POSITION_WORD);
	dxl_set_txpacket_parameter(1, L);   // L bytes sent to each Dynamixel 
	dxl_set_txpacket_length((L+1)*N+4); // bytes in packet exc. Header
	int i = 0;
	for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
	  int id = k->first;
	  DynamixelServo *servo = &*k->second;
	  
#if SERVO_CURVE == 1
	  if (servo->curveMode) {
	    double dt = t-servo->t0;
	    if (fabs(dt) > 1.0) {
	      dt = 0.0;
	      servo->torque(0);
	    }
	    double dt2 = dt*dt;
	    float *c = (dt <= 0) ? servo->c0 : servo->c1;
	    servo->angle0(c[0]+c[1]*dt+c[2]*dt2/2.0);
	    double s=c[1]+c[2]*dt;
	    if (fabs(s) < 5) {
	      if (s < 0) s=-5;
	      else s=5;
	    }
	    servo->speed(s);
	    servo->presentPosition = servo->goalPosition;
	    //	    cout << "servo " << servo->id << " is in curve mode dt=" << dt << endl;
	  }
#endif
	  int position = servo->goalPosition & 4095;
	  int speed = servo->goalSpeed;
	  int torque = servo->goalTorque;

	  dxl_set_txpacket_parameter(i*(L+1)+2,id);
	  dxl_set_txpacket_parameter(i*(L+1)+3,dxl_get_lowbyte(position));
	  dxl_set_txpacket_parameter(i*(L+1)+4,dxl_get_highbyte(position));
	  dxl_set_txpacket_parameter(i*(L+1)+5,dxl_get_lowbyte(speed));
	  dxl_set_txpacket_parameter(i*(L+1)+6,dxl_get_highbyte(speed));
	  dxl_set_txpacket_parameter(i*(L+1)+7,dxl_get_lowbyte(torque));
	  dxl_set_txpacket_parameter(i*(L+1)+8,dxl_get_highbyte(torque));
	  ++i;
	}
	dxl_txrx_packet();
	int result = dxl_get_result(); 

	if (result == COMM_RXSUCCESS) {
	  io.okSince = now();
	}

#if 0
	for (Servos::iterator i = servos.begin(); i != servos.end(); ++i) {
	  i->second->rx();
	}
	
	ostringstream oss;
	oss << "dynamixel," << t;
	for (Servos::iterator i = servos.begin(); i != servos.end(); ++i) {
	  DynamixelServo *servo=&*i->second;
	  oss << "," << servo->id << "," << servo->presentPosition << "," << servo->goalPosition << "," << servo->presentSpeed << "," << servo->goalSpeed << "," << servo->presentTorque << "," << servo->goalTorque;
	}
	cout << oss.str() << endl;
#endif
      }
#else
      for (Servos::iterator i = servos.begin(); i != servos.end(); ++i) {
	i->second->update();
      }
#endif
    }
  }

  std::thread *go;

  void start()
  {
    if (!running) {
      running = true;
      go = new thread(&DynamixelServoController::update,this);
    }
  }

  DynamixelServoController(int deviceIndex, int baudNum)
    : io(deviceIndex,baudNum)
  {
    running = false;
  }

  ~DynamixelServoController()
  {
    if (running) {
      running = false;
      go->join();
      delete go;
    }
  }
};

ServoController* CreateDynamixelServoController(int deviceIndex,int baudNum)
{
  return new DynamixelServoController(deviceIndex,baudNum);
}
