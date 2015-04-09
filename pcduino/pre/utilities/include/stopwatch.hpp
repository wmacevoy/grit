#ifndef UTILITIES_STOPWATCH_HPP
#define UTILITIES_STOPWATCH_HPP

#include <sys/time.h>

namespace utilities {

  class Stopwatch
  {
  private:
    double t;
    bool running;
    struct timeval startTime;
  public:
    Stopwatch(bool _running=true);
    void reset();
    void start();
    void stop();
    double time();
  };

}

#endif
