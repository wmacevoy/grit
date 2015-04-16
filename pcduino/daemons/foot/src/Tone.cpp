#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "Tone.h"

void Tone::run() 
{
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

static unsigned frequency2delay(double frequency)
{
  return (frequency > 0) ? 1e6/(2.0*frequency)+0.5 : ~0;
}

Tone::Tone(int pin, double frequency) 
  : io(pin,GPIO::OUTPUT), 
    delay(frequency2delay(frequency)),
    running(true), 
    thread(&Tone::run,this)
{
  struct sched_param params;
  memset(&params,0,sizeof(params));
  params.sched_priority = sched_get_priority_max(SCHED_FIFO);
  pthread_setschedparam(thread.native_handle(), SCHED_FIFO, &params);
}

void Tone::value(double frequency)
{
  delay = frequency2delay(frequency);
}


Tone::~Tone()
{
  running = false;
  thread.join();
}
