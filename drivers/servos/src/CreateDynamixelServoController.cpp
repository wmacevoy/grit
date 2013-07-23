#include "ServoController.h"
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
  int goalPosition;
  int goalSpeed;
  int goalTorque;

  DynamixelServo(DXLIO &io_, int id_) 
    : io(io_),id(id_), presentPosition(0), goalPosition(0) 
  {
    goalSpeed = 0;
    goalTorque = .70;
    goalPosition = 0;
    io.writeWord(id,DXL_TORQUE_WORD,int(goalTorque*1023));
    update();
  }

  float angle() const { return (180.0/2048)*(presentPosition-2048); }

  void angle(float value) {
    goalPosition = value*(2048/180.0)+2048;
  }

  void speed(float value) {
    // speed for MX-110t (not MX-28t)
    //    cout << "id=" << id << " set speed = " << value << endl;
    goalSpeed = fabs(value)*(60.0/360.0)*(1023/117.07);
    if (goalSpeed > 480) goalSpeed = 480;
  }

  void torque(float value) {
    //    cout << "id=" << id << " set torque = " << value << endl;
    goalTorque = fabs(value)*(1023);
    if (goalTorque > 1023) goalTorque = 1023;
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
      cout << "comm error" << endl;
    }
  }

  void update()
  {
    rx();
    tx();
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
