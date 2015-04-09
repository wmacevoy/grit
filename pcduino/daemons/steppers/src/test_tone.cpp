#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "Tone.h"

void fsleep(double t)
{
  while (t > 0.25) {
    usleep(250000);
    t -= 0.25;
  }
  usleep(t*1e6);
}

int main(int argc, char *argv[])
{
  int pin = 2;
  double frequency = 1000.0;
  double time = 1.0;

  for (int argi = 1; argi < argc; ++argi) {
    if (strcmp(argv[argi],"-p") == 0 || strcmp(argv[argi],"--pin") == 0) {
      ++argi;
      pin = atoi(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"-f") == 0 || strcmp(argv[argi],"--frequency") == 0) {
      ++argi;
      frequency = atof(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"-t") == 0 || strcmp(argv[argi],"--time") == 0) {
      ++argi;
      time = atof(argv[argi]);
      continue;
    }
  }

  Tone tone(pin,frequency);
  fsleep(time);

  return 0;
}
