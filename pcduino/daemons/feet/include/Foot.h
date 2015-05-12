#include "GPIO.h"
#include "AnalogIn.h"
#include "Tone.h"
#include "Configure.h"
#include <atomic>

class Foot
{
 public: double kp;
 public: double kturn;
 public: double kmove;
 public: bool verbose;

 public: static const double TO_FPS;
 public: static const double TO_RPS;

 private: AnalogIn position;
 private: DigitalOut enable;
 private: DigitalOut leftDirection;
 private: DigitalOut rightDirection;
 private: Tone leftStep;
 private: Tone rightStep;
 private: double cutoff;
 private: int reversed;
 private: double goalHeading;
 private: double goalSpeed;
 private: bool running;
 private: std::thread *thread;

 private: double getCurrentHeading();

 public: void setGoalHeading(double value); // 0-1 0.5 = away from tab
 public: void setGoalSpeed(double value); // feet per second
  
 private: void run();
 public: static std::shared_ptr < Foot > 
    factory(const std::string &id,const Configure &cfg, int dev);

 public: Foot(int positionPin, int enablePin, 
	      int leftDirectionPin,  int rightDirectionPin,
	      int leftStepPin, int rightStepPin);
 public: bool isRunning();
 public: void start();
 public: void stop();
 public: ~Foot();	
};
