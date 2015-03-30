#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "AnalogIn.h"

const char* const AnalogIn::PATH = "/proc/adc";

AnalogIn::AnalogIn(int _pin) 
  : pin(_pin), maximum(_pin > 1 ? 4095 :  63)
{
  char path[256];
  sprintf(path, "%s%d", PATH, pin);
  fd = open(path, O_RDWR); 
  //  assert(fd >= 0);
}

int AnalogIn::value() const {
  char msg[32];
  size_t msglen;
  lseek(fd, 0, SEEK_SET);
  msglen=::read(fd,msg,sizeof(msg));
  if (msglen >0 && msglen < sizeof(msg)) {
    msg[msglen]=0;
  } else if (msglen <= 0) {
    msg[0]=0;
  } else {
    msg[sizeof(msg)-1]=0;
  }
  return (msglen >= 5) ? atoi(msg+5) : -1;
}

AnalogIn::~AnalogIn()
{
  close(fd);
}


