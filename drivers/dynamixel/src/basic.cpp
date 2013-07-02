#include <dynamixel.h>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

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

#define DEFAULT_BAUDNUM		34
#define DEFAULT_ID		0

using namespace std;

double now()
{
  struct timeval tm;
  gettimeofday( &tm, 0 );
  return (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
}

struct dxio
{
  int fd;
  fd_set read_fds;
  struct timespec currentTimeout;

  int state;

  unsigned char outb[32];
  unsigned char inb[32];

  dxio(const char *dev, size_t baud) {
    fd = open(dev,O_RDWR); // |O_NONBLOCK);
    assert(fd != -1);

    FD_ZERO(&read_fds);
    FD_SET(fd,&read_fds);
    
    currentTimeout.tv_sec=0;
    currentTimeout.tv_nsec=100000000*0.010;
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
    cout << "wrote " << ans << " of " << size << " bytes" << endl;
    return ans == size;
  }

  ssize_t read0(size_t size, unsigned char *data)
  {
    ssize_t total = 0;
    while (size > 0) {
      currentTimeout.tv_sec=1;
      currentTimeout.tv_nsec=1000000000*0.100;
      //      int status = pselect(1,&read_fds,0,0,&currentTimeout,0);
      //cout << "status=" << status << endl;
      int status = 1;
      if (status == 1) {
	usleep(int(0.1*1000000));
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
    cout << "read " << ans << " bytes" << endl;
    return (ans == size);
  }

  bool write_word(int id, int address, unsigned value)
  {
    unsigned char obuf[9],ibuf[6];
    obuf[0]=0xFF;
    obuf[1]=0xFF;
    obuf[2]=id;
    obuf[3]=5; /* length */;
    obuf[4]=3; /* write */
    obuf[5]=address;
    obuf[6]=value;
    obuf[7]=(value >> 8);
    obuf[8]=~(obuf[2]+obuf[3]+obuf[4]+obuf[5]+obuf[6]+obuf[7]);
    //    return write(sizeof(obuf),obuf) == sizeof(obuf);
#if 1
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


int main()
{
  const char *dev="/dev/ttyUSB0";
  int baud=57600;
  int id=1;

  dxio dxl(dev,baud);
  double t0=now();

  for (;;) {
    usleep(int(1.0*1000000));
    double t=now()-t0;

    if (t > 100) break;

    int goalPosition=4095*(t/10.0);
    dxl.write_word(id,DXL_GOAL_POSITION_WORD,(goalPosition & 4095));
    // int presentPosition = 0;
    int presentPosition=dxl.read_word(id,DXL_PRESENT_POSITION_WORD);
    //    cout << "t=" << t << " goal=" << goalPosition << " present=" << presentPosition << endl;
  }
  return 0;
}

int _main()
{
  int deviceIndex = 0;
  int baudNum = 34;
  int id=1;

  dxl_initialize(deviceIndex,baudNum);

  double t0=now();

  for (;;) {
    usleep(int(1.0*1000000));
    double t=now()-t0;

    if (t > 100) break;

    int goalPosition=4095*(t/10.0);
    dxl_write_word(id,DXL_GOAL_POSITION_WORD,(goalPosition & 4095));
    int presentPosition=dxl_read_word(id,DXL_PRESENT_POSITION_WORD);
    int result = dxl_get_result();
    cout << "t=" << t << " goal=" << goalPosition << " present=" << presentPosition << " result=" << result << endl;
  }

  dxl_terminate();  

  return 0;
}
