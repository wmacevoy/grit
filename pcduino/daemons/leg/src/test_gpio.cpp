#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "GPIO.h"

int main(int argc, char *argv[])
{
  int inPin = 2;
  int inMode = GPIO::INPUT;
  int outPin = 3;

  for (int argi = 1; argi < argc; ++argi) {
    if (strcmp(argv[argi],"-i") == 0 || strcmp(argv[argi],"--inpin") == 0) {
      ++argi;
      inPin = atoi(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"-o") == 0 || strcmp(argv[argi],"--outpin") == 0) {
      ++argi;
      outPin = atoi(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"-p") == 0 || strcmp(argv[argi],"--pullup") == 0) {
      ++argi;
      inMode = atoi(argv[argi]) != 0 ? GPIO::INPUT_PULLUP : GPIO::INPUT;
      continue;
    }
  }

  GPIO in(inPin,inMode);
  GPIO out(outPin, GPIO::OUTPUT);

  for (;;) {
    bool status = in.value();
    out.value(status);
    printf("%d\n",status ? 1 : 0);
    usleep(1000);
  }
}
