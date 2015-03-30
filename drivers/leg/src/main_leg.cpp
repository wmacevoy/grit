#include "GPIO.h"
#include "AnalogIn.h"
#include "Tone.h"

void test1()
{
  AnalogIn pot(2);
  DigitalOut enable(3);
  DigitalOut direction(4);
  Tone pulse(5);

  enable.value(true);
  
  for (;;) {
    double position = 2.0*double(pot.value())/double(pot.maximum)-1.0;
    double frequency = 4000.0*position;
    enable.value(frequency != 0.0);
    direction.value(frequency > 0);
    pulse.value(frequency);
  }
}

int main(int argc, char *argv[])
{
  test1();
  return 0;
}
