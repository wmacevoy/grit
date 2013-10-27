#include "stopwatch.hpp"

namespace utilities {

  Stopwatch::Stopwatch(bool _running) 
  { 
    t=0.0; 
    running = false;
    if (_running) start();
  }

  void Stopwatch::start() 
  { 
    if (!running) {
      gettimeofday(&startTime,0);
      running = true;
    }
  }

  void Stopwatch::stop() 
  { 
    if (running) {
      struct timeval endTime;
      struct timeval diffTime;
      
      gettimeofday(&endTime,0);
      timersub(&endTime,&startTime,&diffTime);
      
      t += (diffTime.tv_sec + 1e-6*diffTime.tv_usec);

      running = false;
    }
  }

  void Stopwatch::reset() 
  { 
    running=false;
    t=0.0;
  }

  double Stopwatch::time()
  {
    if (running) {
      struct timeval endTime;
      struct timeval diffTime;

      gettimeofday(&endTime,0);
      timersub(&endTime,&startTime,&diffTime);
    
      return (t + (diffTime.tv_sec + 1e-6*diffTime.tv_usec));
    } else {
      return t;
    }
  }
}
