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

#include "now.h"

#include <dynamixel.h>

#define DXL_MODEL_WORD_BYTE      0
#define DXL_FIRMWARE_BYTE        2
#define DXL_ID_BYTE              3
#define DXL_BAUD_BYTE            4
#define DXL_RETURN_DELAY_BYTE    5
#define DXL_CW_ANGLE_LIMIT_WORD  6
#define DXL_CCW_ANGLE_LIMIT_WORD 8
#define DXL_DRIVE_BYTE           10
#define DXL_HIGHEST_TEMP_BYTE    11
#define DXL_LOWEST_VOLT_BYTE     12
#define DXL_HIGHEST_VOLT_BYTE    13
#define DXL_MAX_TORQUE_WORD      14
#define DXL_STATUS_BYTE          16
#define DXL_ALARM_LED_BYTE       17
#define DXL_ALARM_SHUTDOWN_BYTE  18
#define DXL_TORQUE_ENABLE_BYTE   24
#define DXL_LED_BYTE             25
#define DXL_D_GAIN_BYTE          26
#define DXL_I_GAIN_BYTE          27
#define DXL_P_GAIN_BYTE          28
#define DXL_GOAL_POSITION_WORD   30
#define DXL_MOVING_SPEED_WORD    32
#define DXL_TORQUE_WORD          34
#define DXL_PRESENT_POSITION_WORD 36
#define DXL_PRESENT_SPEED_WORD   38
#define DXL_PRESENT_LOAD_WORD    40
#define DXL_PRESENT_VOLT_BYTE    42
#define DXL_PRESENT_TEMP_BYTE    43
#define DXL_REGISTERED_BYTE      44
#define DXL_MOVING_BYTE          46
#define DXL_LOCK_BYTE            47
#define DXL_PUNCH_WORD           48
#define DXL_CURRENT_WORD         68
#define DXL_TORQUE_MODE_BYTE     70
#define DXL_GOAL_TORQUE_WORD     71
#define DXL_GOAL_ACCEL_BYTE      73

using namespace std;

const int UPDATE_RATE = 100;

struct DynamixelServo : Servo
{
  int id;
  bool enabled;
  int presentPosition;
  int goalPosition;

  DynamixelServo(int id_) 
    : id(id_), presentPosition(0), goalPosition(0) 
  {
    dxl_write_word(id,DXL_CCW_ANGLE_LIMIT_WORD,4095);
    //    io->write_word(id,DXL_CCW_ANGLE_LIMIT_WORD,4095);
    dxl_write_word(id,DXL_TORQUE_WORD,1);
    update();
  }

  float angle() const { return (180.0/2048)*(presentPosition-2048); }

  void angle(float value) {
    goalPosition = value*(2048/180.0)+2048;
  }

  void tx()
  {
    dxl_write_word(id,DXL_GOAL_POSITION_WORD,(goalPosition & 4095));
    // io->write_word(id,DXL_GOAL_POSITION_WORD,(goalPosition & 4095));
  }

  void rx()
  {
    int inp = dxl_read_word(id,DXL_PRESENT_POSITION_WORD);
    if (dxl_get_result() == COMM_RXSUCCESS) {
      presentPosition = inp;
      //      cout << "pos = " << presentPosition << endl;
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
    dxl_write_word(id,DXL_TORQUE_WORD,0);
  }
};


struct DynamixelServoController : ServoController
{
  typedef std::map < int , std::shared_ptr <DynamixelServo> > Servos;
  Servos servos;
  bool running;
  Servo* servo(int id) {
    Servos::iterator i = servos.find(id);
    if (i != servos.end()) return &*i->second;
    assert(running == false);

    return &*(servos[id] = 
	      std::shared_ptr <DynamixelServo> (new DynamixelServo(id)));
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
  {
    running = false;
    assert(dxl_initialize(deviceIndex,baudNum)==1);
  }

  ~DynamixelServoController()
  {
    if (running) {
      running = false;
      go->join();
      delete go;
    }
    dxl_terminate();
  }
};

ServoController* CreateDynamixelServoController(int deviceIndex,int baudNum)
{
  return new DynamixelServoController(deviceIndex,baudNum);
}
