#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "AnalogIn.h"

int main(int argc, char *argv[])
{
  int inPin = 2;

  for (int argi = 1; argi < argc; ++argi) {
    if (strcmp(argv[argi],"-i") == 0 || strcmp(argv[argi],"--inpin") == 0) {
      ++argi;
      inPin = atoi(argv[argi]);
      continue;
    }
  }

  AnalogIn ain(inPin);

  for (;;) {
    int value = ain.value();
    printf("%d (%5.1lf%%)\n", value,100*double(value)/ain.maximum);
    usleep(1000);
  }
}
