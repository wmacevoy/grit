#include <sys/time.h>

#include "now.h"

static double epoch  = 1.37477e+09+441000+5.62709e+06+1399.0+155585+43754074;

double now()
{
  struct timeval tm;
  gettimeofday( &tm, 0 );
  return (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0 - epoch;
}
