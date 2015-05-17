#include <iostream>
#include <fstream>
#include <math.h>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <iomanip>

#include "GPIO.h"
#include "AnalogIn.h"
#include "Tone.h"
#include "now.h"
#include "Configure.h"

std::string grit_id()
{
  std::string ans;

  char *HOME=getenv("HOME");
  std::string id_path=HOME;
  id_path += "/grit/id";
  std::ifstream id_file(id_path.c_str());
  std::getline(id_file,ans);
  return ans;
}

const std::string id(grit_id());

Configure cfg;

bool verbose;

class StepperServo
{
public:
  AnalogIn position;
  DigitalOut direction;
  Tone pulse;

  double minPosition;
  double maxPosition;
  double minFrequency;
  double maxFrequency;
  double goalPosition;
  double KP,KI,KD;
  double cutoff;

  double t;
  bool running;
  std::thread *thread;
public: 
  StepperServo(int _position, int _direction, int _pulse)
    : position(_position), 
      direction(_direction), 
      pulse(_pulse),
      minPosition(-180.0),
      maxPosition( 180.0),
      minFrequency(  0.0),
      maxFrequency(4000.0),
      goalPosition(0.5),
      KP(1.0),
      KI(0.0),
      KD(0.0),
      cutoff(0.02),
      t(now()), 
      running(false),
      thread(0)
  {
    start();
  }

  void start()
  {
    if (thread == 0) {
      running = true;
      thread = new std::thread(&StepperServo::run,this);
    }
  }

  void stop()
  {
    if (thread != 0) {
      running = false;
      thread->join();
      delete thread;
      thread = 0;
    }
  }
  
  void run()
  {
    double p,g,frequency;

    frequency=0;

    while (running) {
      double dt = now()-t;
      if (floor(t+dt) != floor(t)) {
	std::cout << "p=" << p << " f=" << frequency << std::endl;
      }
      t += dt;

      p=position.value()/double(position.maximum);
      double error = p-goalPosition;
      if (fabs(error) > cutoff) {
	double g=-KP*maxFrequency*error;
	double df=g-frequency;
	if (fabs(df) > dt*maxFrequency) {
	  df=copysign(dt*maxFrequency,df);
	}
	frequency += df;
      } else {
	frequency = 0;
      }

      if (frequency > maxFrequency) {
	frequency=maxFrequency;
      } else if (frequency < -maxFrequency) {
	frequency=-maxFrequency;
      }

      direction.value(frequency < 0);
      pulse.value(fabs(frequency));
      usleep(10000);
    }
  }

  ~StepperServo()
  {
    stop();
  }
};

std::vector < std::shared_ptr < StepperServo > > steppers;

int main(int argc, char *argv[])
{
  system("sudo modprobe adc");

  std::string id=grit_id();
  cfg.path("../../../setup");
  cfg.args(id + ".",argv);
  if (argc == 1) cfg.load("config.csv");
  cfg.servos();
  verbose = cfg.flag(id + ".verbose",false);
  if (verbose) cfg.show();

  for (int dev=0; dev<3; ++dev) {
    std::shared_ptr < StepperServo > stepper;
    switch(dev) {
    case 0: 
      stepper = std::shared_ptr < StepperServo > (new StepperServo(5,2,3)); 
      break;
    case 1: 
      stepper = std::shared_ptr < StepperServo > (new StepperServo(4,4,5)); 
      break;
    case 2: 
      stepper = std::shared_ptr < StepperServo > (new StepperServo(3,6,7)); 
      break;
    }
    stepper->KP=cfg.num("steppers.kp",100);
    stepper->KD=cfg.num("steppers.kd",0);
    stepper->KI=cfg.num("steppers.ki",0);
    stepper->minFrequency=cfg.num("steppers.minfrequency",0);
    stepper->maxFrequency=cfg.num("steppers.maxfrequency",40000);
    stepper->maxFrequency=cfg.num("steppers.maxfrequency",40000);
    stepper->cutoff=cfg.num("steppers.cutoff",0.02);
    steppers.push_back(stepper);
  }

  for (;;) {
    usleep(1000);
  }

  //  test1();

  return 0;
}
o
