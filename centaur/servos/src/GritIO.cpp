#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <iostream>
#include <bitset>

#include "now.h"
#include "GritIO.h"

using namespace std;

#define OK_TIMEOUT 1.0

GritIO::GritIO(int deviceIndex_, int baudNum_)
{
  baudNum=baudNum_;
  deviceIndex=deviceIndex_;
  char tmp[32];
  snprintf(tmp,sizeof(tmp),"/dev/ttyUSB%d",deviceIndex);
  dev=tmp;
  baud = 2000000.0/(baudNum + 1)+0.5;

  fd = -1;
  open();
}

GritIO::GritIO(const char *dev_, size_t baud_)
{
  dev=dev_;
  baud=baud_;

  deviceIndex=0;
  sscanf(dev_,"/dev/ttyUSB%d",&deviceIndex);
  baudNum=2000000.0/baud -0.5;
  fd=-1;
  open();
}

void GritIO::open()
{
  //  struct termios newtio;
  //  struct serial_struct serinfo;

  close();
  fd = ::open(dev.c_str(),O_RDWR|O_NOCTTY|O_NONBLOCK);
  if (fd == -1) {
    cerr << "GritIO::open(): cannot open device " << dev << endl;
  }

#if 0
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
    cerr << "GritIO::open() cannot get serial info" << endl;
    close();
    return;
  }

  serinfo.flags &= ~ASYNC_SPD_MASK;
  serinfo.flags |= ASYNC_SPD_CUST;
  serinfo.custom_divisor = serinfo.baud_base / baud;
	
  if(ioctl(fd, TIOCSSERIAL, &serinfo) < 0) {
    cerr << "GritIO::open() cannot set serial info" << endl;
    close();
    return;
  }
#endif

  okSince = now();

  FD_ZERO(&fds);
  FD_SET(fd,&fds);

  double dt = 1.0;
  timeout.tv_sec=dt;
  timeout.tv_nsec=1e9*(dt-timeout.tv_sec);
}

void GritIO::close()
{
  if (fd != -1) {
    ::close(fd);
    fd=-1;
  }
}

void GritIO::reopen()
{
  if (now()-okSince > OK_TIMEOUT) {
    cerr << "GritIO::reopen() timeout" << endl;
    close();
    open();
  }
}

bool GritIO::write(ssize_t size, const unsigned char *data)
{
  ssize_t ans = ::write(fd,data,size);
  if (ans == size) {
    okSince = now();
  } else {
    reopen();
  }
  if (ans != size) {
    cerr << "GritIO::write() wrote " 
	 << ans << " of " << size << " bytes" << endl;
  }
  return ans == size;
}

ssize_t GritIO::read0(size_t size, unsigned char *data)
{
  ssize_t total = 0;
  int wait = 0;
  int waitlimit = 1000000*1;
  while (size > 0) {
    //    if (pselect(fd+1,&fds,0,0,&timeout,0) != 1) break;
    ssize_t ans = ::read(fd,data,size);
    if (ans <= 0) { 
      if (wait > waitlimit) break;
      usleep(1000);
      wait += 1000;
      ans = 0;
    }
    data += ans;
    size -= ans;
    total += ans;
  }
  return total;
}

bool GritIO::read(ssize_t size, unsigned char *data)
{
  ssize_t ans = read0(size,data);
  if (ans != size) {
    cerr << "GritIO::read() read " << ans << " of " << size << " bytes" << endl;
  }
  return (ans == size);
}

GritIO::~GritIO() { close(); }
