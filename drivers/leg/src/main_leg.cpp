#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <thread>


const char* GPIO_MODE_PATH = "/sys/devices/virtual/misc/gpio/mode/";
const char* GPIO_PIN_PATH = "/sys/devices/virtual/misc/gpio/pin/";
const char* ANALOG_PATH = "/proc/adc";

class Analog
{
public: const int pin;
public: const int maximum;
public: int fd;

  Analog(int _pin) : pin(_pin), maximum(_pin > 1 ? 4095 :  63)
  {
    char path[256];
    sprintf(path, "%s%d", ANALOG_PATH, pin);
    fd = open(path, O_RDWR); 
  }

  int value() const {
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

  ~Analog()
  {
    close(fd);
  }
};

class GPIO
{
public: const int pin;
private: int fdMode;
private: int fdPin;


private: void write(int fd, char value)
  {
    lseek(fd, 0, SEEK_SET);
    int retval=::write(fd,&value,1);
  }

private: int read(int fd) const
  {
    char value;
    lseek(fd, 0, SEEK_SET);
    if (::read(fd,&value,1) != 1) {
      return -1;
    } else {
      return value;
    }
  }

public: enum { INPUT='0', OUTPUT='1', INPUT_PULLUP='8', HIGH='1', LOW='0' };


private: void init()
  {
    char path[256];
    sprintf(path, "%s%s%d", GPIO_MODE_PATH, "gpio", pin);
    fdMode = open(path, O_RDWR);
    assert(fdMode >= 0);
    sprintf(path, "%s%s%d", GPIO_PIN_PATH, "gpio", pin);
    fdPin = open(path, O_RDWR);
    assert(fdPin >= 0);
  }

public: GPIO(int _pin) 
  : pin(_pin) 
  {
    init();
  }

public: GPIO(int _pin, char _mode)
  : pin(_pin)
  {
    init();
    mode(_mode);
  }
  

private: void mode(char value)
  {
    write(fdMode,value);

  }

public: void modeInput() 
  { 
    mode(INPUT); 
  }

public: void modeInputPullup() 
  { 
    mode(INPUT_PULLUP); 
  }

public: void modeOutput() 
  { 
    mode(OUTPUT); 
  }


  void value(bool value)
  {
    write(fdPin,value ? HIGH : LOW);
  }

  bool value() const {
    return read(fdPin) == HIGH;
  }

  ~GPIO()
  {
    close(fdMode);
    close(fdPin);
  }
};

struct Frequency
{
  GPIO io;
  volatile unsigned delay;
  volatile bool running;
  std::thread thread;

  void run() {
    while (running) {
      unsigned _delay = delay;
      if (_delay != ~0) {
	io.value(0);
	usleep(_delay);
	io.value(1);
	usleep(_delay);
      } else {
	usleep(1000);
      }
    }
  }

  Frequency(int pin) 
    : io(pin,GPIO::OUTPUT), delay(~0), running(true), thread(&Frequency::run,this)
  {
    struct sched_param params;
    memset(&params,0,sizeof(params));
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(thread.native_handle(), SCHED_FIFO, &params);
  }

  void value(double frequency)
  {
    if (frequency > 0) {
      delay = 1e6/(2.0*frequency)+0.5;
    } else {
      delay = ~0;
    }
  }


  ~Frequency()
  {
    running = false;
    thread.join();
  }
};

/* 1000 -> 951 */

void test1()
{
  Analog a(2);

  Frequency f(4);
  f.value(5000.0);
  for (;;) {
    f.value(a.value()*4000.0/a.maximum);
    usleep(10000);
  }
}

/*
Frequency f2(2),f3(3),f4(4),f5(5), f6(6);

Frequency *frequencies[] = { 0, 0, 0, 0, &f4 }; 

*/

int main(int argc, char *argv[])
{
  test1();
  return 0;
}
