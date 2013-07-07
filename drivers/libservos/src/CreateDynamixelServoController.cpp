//#include "DynamixelDriver.h"
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

struct dxio
{
  string dev;
  int baud;

  int fd;
  fd_set read_fds;
  struct timespec currentTimeout;

  double okSince;

  int state;

  unsigned char outb[32];
  unsigned char inb[32];

  void reopen()
  {
    okSince = now();
    if (fd != -1) close(fd);
    fd = open(dev.c_str(),O_RDWR|O_NONBLOCK);
    if (fd == -1) {
      cout << "reopen " << dev << " failed" << endl;
      return;
    }
    FD_ZERO(&read_fds);
    FD_SET(fd,&read_fds);
    
    currentTimeout.tv_sec=0;
    currentTimeout.tv_nsec=100000000*0.010;
  }

  dxio(const char *dev_, size_t baud_) 
    : dev(dev_), baud(baud_)
  {
    fd = -1;
    reopen();
  }

  ~dxio()
  {
    if (fd != -1) close(fd);
  }

  void timeout(double t)
  {
    ssize_t tsec=t;
    currentTimeout.tv_sec = tsec;
    currentTimeout.tv_nsec = 1000000000*(t-tsec);
  }

  bool write(ssize_t size, const unsigned char *data)
  {
    ssize_t ans = ::write(fd,data,size);
    if (ans == size) {
      okSince = now();
    } else if (now()-okSince > 0.500) {
      reopen();
    }
    //    cout << "wrote " << ans << " of " << size << " bytes" << endl;
    return ans == size;
  }

  ssize_t read0(size_t size, unsigned char *data)
  {
    ssize_t total = 0;
    while (size > 0) {
      currentTimeout.tv_sec=0;
      currentTimeout.tv_nsec=100000000*0.010;
      int status = pselect(1,&read_fds,0,0,&currentTimeout,0);
      if (status == 1) {
	ssize_t ans = ::read(fd,data,size);
	if (ans == -1) break;
	data += ans;
	size -= ans;
	total += ans;
      } else {
	break;
      }
    }
    return total;
  }

  bool read(ssize_t size, unsigned char *data)
  {
    ssize_t ans = read0(size,data);
    //    cout << "read " << ans << " bytes" << endl;
    return (ans == size);
  }

  bool write_word(int id, int address, unsigned value)
  {
    unsigned char obuf[9] /*,ibuf[6] */;
    obuf[0]=0xFF;
    obuf[1]=0xFF;
    obuf[2]=id;
    obuf[3]=5; /* length */;
    obuf[4]=3; /* write */
    obuf[5]=address;
    obuf[6]=value;
    obuf[7]=(value >> 8);
    obuf[8]=~(obuf[2]+obuf[3]+obuf[4]+obuf[5]+obuf[6]+obuf[7]);
    return write(sizeof(obuf),obuf) == sizeof(obuf);
#if 0
    if (write(sizeof(obuf),obuf) && read(sizeof(ibuf),ibuf)) {
      if (~(ibuf[2]+ibuf[3]+ibuf[4]) == ibuf[5]) {
	return ibuf[4] == 0;
      }
    }
    return false;
#endif
  }

  bool write_byte(int id, int address, unsigned value)
  {
    unsigned char obuf[8],ibuf[6];
    obuf[0]=0xFF;
    obuf[1]=0xFF;
    obuf[2]=id;
    obuf[3]=4; /* length */;
    obuf[4]=3; /* write */
    obuf[5]=address;
    obuf[6]=value;
    obuf[7]=~(obuf[2]+obuf[3]+obuf[4]+obuf[5]+obuf[6]);
    return write(sizeof(obuf),obuf) && read(sizeof(ibuf),ibuf);
  }

  int read_word(int id, int address)
  {
    unsigned char obuf[8],ibuf[9];

    obuf[0]=0xFF;
    obuf[1]=0xFF;
    obuf[2]=id;
    obuf[3]=4; /* length*/
    obuf[4]=2; /* read */
    obuf[5]=address;
    obuf[6]=2;
    obuf[7]=~(obuf[2]+obuf[3]+obuf[4]+obuf[5]+obuf[6]);
    if (write(sizeof(obuf),obuf) && read(sizeof(ibuf),ibuf)) {
      if (~(ibuf[0]+ibuf[1]+ibuf[2]+ibuf[3]+ibuf[4]+ibuf[5]+ibuf[6]) == ibuf[7]) {
	if (ibuf[4] == 0) {
	  return ibuf[5]+(ibuf[6]<<8);
	}
      }
    }
    return -1;
  }
};



#define DXL_OK(cmd) { if ((cmd) 

struct DynamixelServo : Servo
{
  dxio *io;

  int id;
  int presentPosition;
  int goalPosition;

  DynamixelServo(dxio *io_,int id_) 
    : io(io_), id(id_), presentPosition(0), goalPosition(0) 
  {
    //    dxl_write_word(id,DXL_CCW_ANGLE_LIMIT_WORD,4095);
    io->write_word(id,DXL_CCW_ANGLE_LIMIT_WORD,4095);
    update();
  }

  float angle() const { return (180.0/2048)*(presentPosition-2048); }

  void angle(float value) {
    goalPosition = value*(2048/180.0)+2048;
  }

  void tx()
  {
    //    dxl_write_word(id,DXL_GOAL_POSITION_WORD,(goalPosition & 4095));
    io->write_word(id,DXL_GOAL_POSITION_WORD,(goalPosition & 4095));
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
    //    rx();
    tx();
  }
};


struct DynamixelServoController : ServoController
{
  typedef std::map < int , DynamixelServo* > Servos;
  Servos servos;
  bool running;
  dxio io;
  Servo* servo(int id) {
    Servos::iterator i = servos.find(id);
    if (i != servos.end()) return i->second;
    assert(running == false);

    return servos[id] = new DynamixelServo(&io,id);
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
    : io("/dev/ttyUSB0",57600)
  {
    running = false;
    //    assert(dxl_initialize(deviceIndex,baudNum)==1);
  }

  ~DynamixelServoController()
  {
    if (running) {
      running = false;
      go->join();
      delete go;
    }
    //    dxl_terminate();
  }
};

ServoController* CreateDynamixelServoController(int deviceIndex,int baudNum)
{
  return new DynamixelServoController(deviceIndex,baudNum);
}
