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

#include "now.h"

// http://support.robotis.com/en/product/dynamixel/ax_series/dxl_ax_actuator.htm
#include "DXLIO.h"

using namespace std;

const int UPDATE_RATE = 50;

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
    goalTorque = 0;
    goalPosition = 0;
    //    io.writeWord(id,DXL_TORQUE_WORD,int(0.70*1023));
    update();
  }

  float angle() const { return (180.0/2048)*(presentPosition-2048); }

  void angle(float value) {
    goalPosition = value*(2048/180.0)+2048;
  }

  void speed(float value) {
    goalSpeed = fabs(value)*(60.0/360.0)*(1023/117.07);
  }

  void torque(float value) {
    goalTorque = fabs(value)*(1023);
  }


  void tx()
  {
    io.writeWord(id,DXL_GOAL_POSITION_WORD,(goalPosition & 4095));
    io.writeWord(id,DXL_MOVING_SPEED_WORD,(goalSpeed & 4095));
    io.writeWord(id,DXL_TORQUE_WORD,(goalTorque & 4095));
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
