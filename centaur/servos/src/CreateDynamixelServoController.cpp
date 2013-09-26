#include <sstream>
#include <strstream>
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

#include "ServoController.h"
#include "Configure.h"

#define USE_BROADCAST 1

#if USE_BROADCAST == 1
#include <dynamixel.h>
#endif

#include "now.h"

// http://support.robotis.com/en -- MX-106t (legs)  MX-28t (neck & fingers)
//
// http://support.robotis.com/en/product/dynamixel/ax_series/dxl_ax_actuator.htm
#include "DXLIO.h"

#define USE_TORQUE_ENABLED 1

using namespace std;

const float UPDATE_RATE = 10.0;

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
  bool readNextPosition;
  float minSpeed;
  float minTorque;

  bool enabled;
  bool curveMode;
  double t[2];
  float c0[3],c1[3];
  float rxRate;

  DynamixelServo(Configure &cfg, DXLIO &io_, int id_) 
    : io(io_),id(id_), presentPosition(2048), goalPosition(2048) 
  {
    enabled=true;
    minSpeed = atof(cfg.servo(id,"speed").c_str());
    minTorque = atof(cfg.servo(id,"torque").c_str());
    presentSpeed = 0;
    presentTorque = 0;
    //   cout << "create dynamixel servo id = " << id << " minSeed=" << minSpeed << " minTorque=" << minTorque << endl;
    rate(1.0);
    angle(0.0);
    speed(30.0);
    torque(0.10);
    //    io.writeWord(id,DXL_TORQUE_WORD,int(goalTorque*1023));
    curveMode = false;
    readNextPosition=true;
#if USE_BROADCAST != 1
    update();
#endif
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

    //    cout << "dynamixel curve" << " servo=" << id << " t0=" << t0 << " c0=[" << c0[0] << "," << c0[1] << "," << c0[2] << "]" << " c1=[" << c1[0] << "," << c1[1] << "," << c1[2] << "]"  << endl;
  }

  float angle() const { 
    return (180.0/2048)*(presentPosition-2048); 
  }

  void angle0(float value) {
    goalPosition = value*(2048/180.0)+2048;
    //    cout << "dynamixel servo=" << id << " goal position=" << goalPosition << endl;
  }
  void angle(float value) {
    curveMode = false;
    angle0(value);
  }

  void speed(float value) {
    // speed for MX-110t (not MX-28t)
    //    cout << "id=" << id << " set speed = " << value << endl;
    if (fabs(value) < minSpeed) {
      if (value < 0) value=-minSpeed;
      else value = minSpeed;
    }
    goalSpeed = fabs(value)*(60.0/360.0)*(1023/117.07);
    if (goalSpeed > 480) goalSpeed = 480;
  }

  float speed() const {
    return presentSpeed/((60.0/360.0)*(1023/117.07));
  }

  void torque(float value) {
    //    cout << "id=" << id << " set torque = " << value << endl;
    value = fabs(value);
    if (0 < value && value < minTorque) {
      value = minTorque;
    }
    goalTorque = fabs(value)*(1023);
    if (goalTorque > 1023) goalTorque = 1023;
  }

  float torque() const {
    return presentTorque/1023.0;
  }

  void rate(float value) {
    rxRate = value;
    if (rxRate == 0) {
      rxTime = 0;
    } else if (rxTime > now() + 1.0/rxRate) {
      rxTime = now()+1.0/rxRate;
    }
  }

  void tx()
  {
    if (goalTorque != 0) {
      io.writeWord(id,DXL_GOAL_POSITION_WORD,(goalPosition & 4095));
      io.writeWord(id,DXL_MOVING_SPEED_WORD,goalSpeed);
      io.writeWord(id,DXL_TORQUE_WORD,goalTorque);
      io.writeWord(id,DXL_TORQUE_ENABLE_BYTE,1);
    } else {
      io.writeWord(id,DXL_TORQUE_ENABLE_BYTE,0);
    }
  }

  double rxTime;
  void rx()
  {
    if (rxRate ==0 || rxTime >= now()) return;

    rxTime = now() + 1.0/rxRate;

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
#if USE_BROADCAST == 0
    tx();
#endif
    rx();
  }

  ~DynamixelServo()
  {
    io.writeWord(id,DXL_TORQUE_WORD,1);
  }
};

struct DynamixelServoController : ServoController
{
  Configure &cfg;
  DXLIO io;
  typedef std::map < int , std::shared_ptr <DynamixelServo> > Servos;
  Servos servos;
  bool running;
  Servo* servo(int id) {
    Servos::iterator i = servos.find(id);
    if (i != servos.end()) return &*i->second;
    assert(running == false);

    return &*(servos[id] = 
	      std::shared_ptr <DynamixelServo> (new DynamixelServo(cfg,io,id)));
  }

  int countServosInRange(int lower,int upper) {
    int retval=0;
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      if (k->first>=lower && k->first<=upper) retval++;
    }
    return retval;
  }

  void broadcastSpeedPosition(bool output,double t,double t1,int lower,int upper) {
    io.reopen(); // reopen if failing recently...

    int N = 0;
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      if (k->first>=lower && k->first<=upper) {
	if (k->second->enabled) {
	  ++N;
	}
      }
    }
    if (N == 0) return;

    int L = 4; // total data payload for position + speed
    
    dxl_set_txpacket_id(BROADCAST_ID);
    dxl_set_txpacket_instruction(INST_SYNC_WRITE);
    dxl_set_txpacket_parameter(0, DXL_GOAL_POSITION_WORD);
    dxl_set_txpacket_parameter(1, L);   // L bytes sent to each Dynamixel 
    dxl_set_txpacket_length((L+1)*N+4); // bytes in packet exc. Header
    int i = 0;
    //	std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
    //	std::cout.precision(5);
    //	if (output) {
    //	  cout << endl;
    //	  cout << "t," << setprecision(15) << t << ",";
    //	}
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      int id = k->first;
      if (id>=lower && id<=upper)  {
	if (k->second->enabled) {
	  DynamixelServo *servo = &*k->second;
	  
	  if (servo->curveMode) {
	    double t0=servo->t[0];
	    if (t < t0) {
	      if (t1 > t0+0.0001) {
		t1 = t0+0.0001;
	      }
	    }
	    
	    double dt;
	    if (t < servo->t[1]) {
	      dt = t-servo->t[0];
	    } else {
	      dt = servo->t[1]-servo->t[0];
	      //			  cout << "servo " << id << " stale " << t - servo->t[1] << " seconds" << endl;
	    }
	    double dt2 = dt*dt;
	    float *c = (dt <= 0) ? servo->c0 : servo->c1;
	    float angle = c[0]+c[1]*dt+c[2]*dt2/2.0;
	    float speed = 1.1*(c[1]+c[2]*dt);
	    servo->angle0(angle);
	    servo->speed(speed);
	    //			servo->presentPosition = servo->goalPosition;
	  }
	  int position = servo->goalPosition & 4095;
	  int speed = servo->goalSpeed;
	  //	  int position = 2048;
	  //      int speed = 300;
	  //	      cout << "ID " << id << " sent a position of " << position <<  "  speed " << speed << endl;
	  
	  dxl_set_txpacket_parameter(i*(L+1)+2,id);
	  dxl_set_txpacket_parameter(i*(L+1)+3,dxl_get_lowbyte(position));
	  dxl_set_txpacket_parameter(i*(L+1)+4,dxl_get_highbyte(position));
	  dxl_set_txpacket_parameter(i*(L+1)+5,dxl_get_lowbyte(speed));
	  dxl_set_txpacket_parameter(i*(L+1)+6,dxl_get_highbyte(speed));
	  ++i;
	}
      }
      if (output) {
	DynamixelServo *servo = &*(k->second);
	cout << "id,"<<servo->id << "," << servo->presentPosition << "," << servo->presentSpeed << ",";
      }
      
    }
    //	cout << "Packet " << endl;
    dxl_txrx_packet();
    int result = dxl_get_result(); 
    
    if (result == COMM_RXSUCCESS) {
      io.okSince = now();
    }
  }

  void broadcastTorque(bool output,double t,double t1,int lower,int upper) {
    io.reopen(); // reopen if failing recently...

    int N = countServosInRange(lower,upper);
    int L = 2; // total data payload for torque

    dxl_set_txpacket_id(BROADCAST_ID);
    dxl_set_txpacket_instruction(INST_SYNC_WRITE);
    dxl_set_txpacket_parameter(0, DXL_TORQUE_WORD);
    dxl_set_txpacket_parameter(1, L);   // L bytes sent to each Dynamixel 
    dxl_set_txpacket_length((L+1)*N+4); // bytes in packet exc. Header
    int i = 0;
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      int id = k->first;
      if (id>=lower && id<=upper) {
	DynamixelServo *servo = &*k->second;
	int torque = 1023; // servo->goalTorque;  
	//	    int torque = servo->goalTorque;  
	dxl_set_txpacket_parameter(i*(L+1)+2,id);
	dxl_set_txpacket_parameter(i*(L+1)+3,dxl_get_lowbyte(torque));
	dxl_set_txpacket_parameter(i*(L+1)+4,dxl_get_highbyte(torque));
	//	    cout << "ID " << id << " sent a torque of " << torque << endl;
	++i;
      }
    }
    //	cout << "Packet " << endl;
    dxl_txrx_packet();
    int result = dxl_get_result(); 

    if (result == COMM_RXSUCCESS) {
      io.okSince = now();
    }
  }

  void broadcastTorqueEnable(bool output,double t,double t1,int lower,int upper) {
    io.reopen(); // reopen if failing recently...

    // count servos with changed enable state...
    int N = 0;
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      if (k->first>=lower && k->first<=upper) {
	if (!k->second->enabled || (k->second->goalTorque == 0)) {
	  ++N;
	}
      }
    }

    if (N == 0) return;
	

    int L = 1; // total data payload for torque

    dxl_set_txpacket_id(BROADCAST_ID);
    dxl_set_txpacket_instruction(INST_SYNC_WRITE);
    dxl_set_txpacket_parameter(0, DXL_TORQUE_ENABLE_BYTE);
    dxl_set_txpacket_parameter(1, L);   // L bytes sent to each Dynamixel 
    dxl_set_txpacket_length((L+1)*N+4); // bytes in packet exc. Header
    int i = 0;
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      int id = k->first;
      if (id>=lower && id<=upper) {
	if (!k->second->enabled || (k->second->goalTorque == 0)) {
	  DynamixelServo *servo = &*k->second;
	  int torque = servo->goalTorque;
	  dxl_set_txpacket_parameter(i*(L+1)+2,id);
	  dxl_set_txpacket_parameter(i*(L+1)+3,(torque != 0));
	  k->second->enabled = (torque != 0);
	  ++i;
	}
      }
    }
    //	cout << "Packet " << endl;
    dxl_txrx_packet();
    int result = dxl_get_result(); 

    if (result == COMM_RXSUCCESS) {
      io.okSince = now();
    }
  }
  // Packets are really limited to about 20 servos for three registers.  
  void update() {
    double t1 = now() + 1.0/UPDATE_RATE;
    while (running) {
      int us = (t1-now())*1000000;
      if (us > 0) usleep(us);
      double t = now();
      t1=t+1.0/UPDATE_RATE;
      bool output =(floor(t) != floor(t1));
#if USE_BROADCAST
      {

	//	This makes the servos "tick" everytime it is sent
#if USE_TORQUE_ENABLED
#warning torque enable code enabled
	broadcastTorqueEnable(output,t,t1,0,49); // legs
	broadcastTorqueEnable(output,t,t1,90,99); // head/waist
	broadcastTorqueEnable(output,t,t1,50,59); // Left arms
	broadcastTorqueEnable(output,t,t1,60,69); // arms
#endif



	broadcastTorque(output,t,t1,0,49); // legs
	broadcastTorque(output,t,t1,90,99); // head/waist
	broadcastTorque(output,t,t1,50,59); // Left arms
	broadcastTorque(output,t,t1,60,69); // arms


	broadcastSpeedPosition(output,t,t1,0,49);
	broadcastSpeedPosition(output,t,t1,90,99);
	broadcastSpeedPosition(output,t,t1,50,59);
	broadcastSpeedPosition(output,t,t1,60,69);
      }
#endif
      for (Servos::iterator i = servos.begin(); i != servos.end(); ++i) {
	i->second->update();
      }
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

  DynamixelServoController(Configure &cfg_, int deviceIndex, int baudNum)
    : cfg(cfg_), io(deviceIndex,baudNum)
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

ServoController* CreateDynamixelServoController(Configure &cfg, int deviceIndex,int baudNum)
{
  return new DynamixelServoController(cfg, deviceIndex,baudNum);
}
