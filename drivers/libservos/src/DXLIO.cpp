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

using namespace std;

DXLIO::DXLIO(const char *dev_, size_t baud_)
{
  dev=dev_;
  baud=baud_;
  fd=-1;
  open();
}

void DXLIO::open()
{
  struct termios newtio;
  struct serial_struct serinfo;

  close();
  fd = ::open(dev.c_str(),O_RDWR|O_NOCTTY|O_NONBLOCK);
  if (fd == -1) {
    cerr << "DXLIO::open(): cannot open device " << dev << endl;
  }

  memset(&newtio, 0, sizeof(newtio));

  newtio.c_cflag	= B38400|CS8|CLOCAL|CREAD;
  newtio.c_iflag	= IGNPAR;
  newtio.c_oflag	= 0;
  newtio.c_lflag	= 0;
  newtio.c_cc[VTIME]	= 0;
  newtio.c_cc[VMIN]	= 0;

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
    
  timeout.tv_sec=0;
  timeout.tv_nsec=1e9*1000.0*12.0/baud;
}

void DXLIO::close()
{
  if (fd != -1) {
    ::close(fd);
    fd=-1;
  }
}

bool DXLIO::write(ssize_t size, const unsigned char *data)
{
  ssize_t ans = ::write(fd,data,size);
  if (ans == size) {
    okSince = now();
  } else if (now()-okSince > 0.500) {
    open();
  }
  //    cout << "wrote " << ans << " of " << size << " bytes" << endl;
  return ans == size;
}

ssize_t DXLIO::read0(size_t size, unsigned char *data)
{
  ssize_t total = 0;
  while (size > 0) {
    int status = pselect(1,&fds,0,0,&timeout,0);
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

bool DXLIO::read(ssize_t size, unsigned char *data)
{
  ssize_t ans = read0(size,data);
  //    cout << "read " << ans << " bytes" << endl;
  return (ans == size);
}

bool DXLIO::writeWord(int id, int address, unsigned value)
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
#if 0
  return write(sizeof(obuf),obuf) == sizeof(obuf);
#endif
#if 1
  if (write(sizeof(obuf),obuf) && read(sizeof(ibuf),ibuf)) {
    if (~(ibuf[2]+ibuf[3]+ibuf[4]) == ibuf[5]) {
      return ibuf[4] == 0;
    }
  }
  return false;
#endif
}

#if 0
bool DXLIO::writeByte(int id, int address, unsigned value)
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

int DXLIO::readWord(int id, int address)
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

