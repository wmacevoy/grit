#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include "GPIO.h"

const char* const GPIO::MODE_PATH = "/sys/devices/virtual/misc/gpio/mode/";
const char* const GPIO::PIN_PATH = "/sys/devices/virtual/misc/gpio/pin/";

GPIO::GPIO(int _pin)
  : pin(_pin) 
{
  init();
}

GPIO::GPIO(int _pin, char _mode)
: pin(_pin)
{
  init();
  mode(_mode);
}


void GPIO::init()
{
  char path[256];
  snprintf(path, sizeof(path), "%s%s%d", MODE_PATH, "gpio", pin);
  fdMode = open(path, O_RDWR);
  assert(fdMode >= 0);
  snprintf(path, sizeof(path), "%s%s%d", PIN_PATH, "gpio", pin);
  fdPin = open(path, O_RDWR);
  assert(fdPin >= 0);
}

void GPIO::write(int fd, char value)
{
  lseek(fd, 0, SEEK_SET);
  int retval=::write(fd,&value,1);
}

int GPIO::read(int fd) const
{
  char value;
  lseek(fd, 0, SEEK_SET);
  if (::read(fd,&value,1) == 1) {
    return value;
  } else {
    return -1;
  }
}

GPIO::~GPIO() 
{ 
  close(fdMode);
  close(fdPin);
}

DigitalIn::DigitalIn(int pin)
  :GPIO(pin,INPUT)
{

}

DigitalInPullup::DigitalInPullup(int pin)
  :GPIO(pin,INPUT_PULLUP)
{

}

DigitalOut::DigitalOut(int pin)
  :GPIO(pin,OUTPUT)
{

}
