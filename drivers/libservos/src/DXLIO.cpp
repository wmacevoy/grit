#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <iostream>

#include "now.h"

#include "DXLIO.h"
#define USE_DXL 1

#if USE_DXL
#include <dynamixel.h>
#endif

using namespace std;

#define OK_TIMEOUT 1.0

DXLIO::DXLIO(int deviceIndex_, int baudNum_)
{
  char tmp[32];
  snprintf(tmp,sizeof(tmp),"/dev/ttyUSB%d",deviceIndex);
  dev=tmp;
  baud = 2000000.0/(baudNum + 1)+0.5;
  deviceIndex=deviceIndex_;
  baudNum=baudNum_;

  fd = -1;
  open();
}

DXLIO::DXLIO(const char *dev_, size_t baud_)
{
  dev=dev_;
  baud=baud_;

  deviceIndex=0;
  sscanf(dev_,"/dev/ttyUSB%d",&deviceIndex);
  baudNum=2000000.0/baud -0.5;
  fd=-1;
  open();
}

void DXLIO::open()
{
#if USE_DXL
  cout << "dxl_initialize(" << deviceIndex << "," << baudNum << ")" << endl;
  dxl_initialize(deviceIndex,baudNum);
#else

  struct termios newtio;
  struct serial_struct serinfo;

  close();
  fd = ::open(dev.c_str(),O_RDWR|O_NOCTTY);
  if (fd == -1) {
    cerr << "DXLIO::open(): cannot open device " << dev << endl;
  }

  memset(&newtio, 0, sizeof(newtio));

  newtio.c_cflag	= B38400|CS8|CLOCAL|CREAD;
  newtio.c_iflag	= IGNPAR;
  newtio.c_oflag	= 0;
  newtio.c_lflag	= 0;
  newtio.c_cc[VTIME]	= 1;
  newtio.c_cc[VMIN]	= 1;

  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &newtio);

  if(ioctl(fd, TIOCGSERIAL, &serinfo) < 0) {
    cerr << "DXLIO::open() cannot get serial info" << endl;
    close();
    return;
  }

  serinfo.flags &= ~ASYNC_SPD_MASK;
  serinfo.flags |= ASYNC_SPD_CUST;
  serinfo.custom_divisor = serinfo.baud_base / baud;
	
  if(ioctl(fd, TIOCSSERIAL, &serinfo) < 0) {
    cerr << "DXLIO::open() cannot set serial info" << endl;
    close();
    return;
  }

  okSince = now();

  FD_ZERO(&fds);
  FD_SET(fd,&fds);

  double dt = 0.01;
  timeout.tv_sec=dt;
  timeout.tv_nsec=1e9*(dt-timeout.tv_sec);
#endif
}

void DXLIO::close()
{
#if USE_DXL
  cout << "dxl_terminate()" << endl;
  dxl_terminate();
#else
  if (fd != -1) {
    ::close(fd);
    fd=-1;
  }
#endif
}

bool DXLIO::write(ssize_t size, const unsigned char *data)
{
#if USE_DXL
  return false;
#else
  ssize_t ans = ::write(fd,data,size);
  if (ans == size) {
    okSince = now();
  } else if (now()-okSince > 0.500) {
    open();
  }
  if (ans != size) {
    cout << "wrote " << ans << " of " << size << " bytes" << endl;
  }
  return ans == size;
#endif
}

ssize_t DXLIO::read0(size_t size, unsigned char *data)
{
#if USE_DXL
  return 0;
#else
  ssize_t total = 0;
  while (size > 0) {
    if (pselect(fd+1,&fds,0,0,&timeout,0) != 1) break;
    ssize_t ans = ::read(fd,data,size);
    if (ans == -1) break;
    data += ans;
    size -= ans;
    total += ans;
  }
  return total;
#endif
}

bool DXLIO::read(ssize_t size, unsigned char *data)
{
#if USE_DXL
  return false;
#else
  ssize_t ans = read0(size,data);
  if (ans != size) {
    cout << "read " << ans << " of " << size << " bytes" << endl;
  }
  return (ans == size);
#endif
}

bool DXLIO::writeWord(int id, int address, int value)
{
#if USE_DXL
  dxl_write_word(id,address,value);
  int result = dxl_get_result();
  bool ok = (result == COMM_RXSUCCESS || result == COMM_RXTIMEOUT || result == COMM_RXCORRUPT);
  if (ok) {
    okSince = now();
  } else {
    cout << "DXLIO::writeWord(" 
	 << id << "," 
	 << address << "," 
	 << value << ")" << "result=" << result << endl;
    if (now()-okSince > OK_TIMEOUT) {
      close(); open();
    }
  }
  return ok;
#else
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
  if (write(sizeof(obuf),obuf)) {
    if (read(sizeof(ibuf),ibuf)) {
      if ((unsigned char)(~(ibuf[2]+ibuf[3]+ibuf[4])) == ibuf[5]) {
	if (ibuf[4] == 0) {
	  return true;
	} else {
	  cerr << "write nonzero reply code = " << ibuf[4] << endl;
	}
      } else {
	char tmp[64];
	sprintf(tmp,"%02x %02x %02x %02x %02x %02x",ibuf[0],ibuf[1],ibuf[2],ibuf[3],ibuf[4],ibuf[5]);
	cerr << "write reply corrupted:" << tmp << endl;
      }
    } else {
      cerr << "write reply timeout" << endl;
    }
  } else {
    cerr << "write failed" << endl;
  }
  return false;
#endif
}

#if 0
bool DXLIO::writeByte(int id, int address, int value)
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
#endif

bool DXLIO::readWord(int id, int address, int *value)
{
#if USE_DXL
  int ans = dxl_read_word(id,address);
  int result = dxl_get_result();
  bool ok = (result == COMM_RXSUCCESS || result == COMM_RXTIMEOUT || result == COMM_RXCORRUPT);
  if (ok) {
    if (value != 0) *value = ans;
    okSince = now();
  } else {
    cout << "DXLIO::readWord(" 
	 << id << "," 
	 << address << ")=" 
	 << ans << ", result=" << result << endl;
    if (now()-okSince > OK_TIMEOUT) {
      close(); open();
    }
  }
  return ok;
#else
  unsigned char obuf[8],ibuf[9];

  obuf[0]=0xFF;
  obuf[1]=0xFF;
  obuf[2]=id;
  obuf[3]=4; /* length*/
  obuf[4]=2; /* read */
  obuf[5]=address;
  obuf[6]=2;
  obuf[7]=~(obuf[2]+obuf[3]+obuf[4]+obuf[5]+obuf[6]);

#if 0  
  char tmp[64];
  sprintf(tmp,"%02x %02x %02x %02x %02x %02x %02x %02x",
	  obuf[0],obuf[1],obuf[2],obuf[3],
	  obuf[4],obuf[5],obuf[6],obuf[7]);
  cerr << "read command: " << tmp << endl;
  
#endif
  if (write(sizeof(obuf),obuf)) {
    if (read(sizeof(ibuf),ibuf)) {
      if ((unsigned char)(~(ibuf[0]+ibuf[1]+ibuf[2]+ibuf[3]+ibuf[4]+ibuf[5]+ibuf[6])) == ibuf[7]) {
	if (ibuf[4] == 0) {
	  if (value != 0) {
	    *value = ibuf[5]+(ibuf[6]<<8);
	  }
	  return true;
	} else {
	  cerr << "read nonzero reply code = " << ibuf[4] << endl;
	}
      } else {
	char tmp[64];
	sprintf(tmp,"%02x %02x %02x %02x %02x %02x %02x %02x",
		ibuf[0],ibuf[1],ibuf[2],ibuf[3],
		ibuf[4],ibuf[5],ibuf[6],ibuf[7]);
	cerr << "read reply corrupted " << tmp << endl;
      }
    } else {
      cerr << "read reply timeout" << endl;
    }
  } else {
    cerr << "read request failed" << endl;
  }
  return false;
#endif
}

DXLIO::~DXLIO() { close(); }
