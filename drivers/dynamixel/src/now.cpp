#include <sys/time.h>

#include "now.h"

double now()
{
  struct timeval tm;
  gettimeofday( &tm, 0 );
  return (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
}
