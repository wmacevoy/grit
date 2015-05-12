#include "GPIO.h"
#include "AnalogIn.h"
#include "Tone.h"
#include <atomic>

class Foot
{
 private: static const double KP;
 private: static const double TO_FPS;
 private: static const double TO_RPS;

 private: AnalogIn position;
 private: DigitalOut enable;
 private: DigitalOut leftDirection;
 private: DigitalOut rightDirection;
 private: Tone leftPulse;
 private: Tone rightPulse;
 private: double cutoff;
 private: int reversed;
 private: double goalHeading;
 private: double goalSpeed;
 public:  bool running;
 private: std::thread thread;

 private: double getCurrentHeading();

 public: void setGoalHeading(double value); // 0-1 0.5 = away from tab
 public: void setGoalSpeed(double value); // feet per second
  
 private: void run();
 public: Foot(int positionPin, int enablePin, 
	      int leftDirectionPin,  int rightDirectionPin,
	      int leftPulsePin, int rightPulsePin);
 public: bool isRunning();
 public: ~Foot();	
};
