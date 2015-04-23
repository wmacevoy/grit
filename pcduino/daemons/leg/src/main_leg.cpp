#include <iostream>
#include <fstream>
#include <math.h>
#include <thread>
#include <chrono>
#include <memory>

#include "GPIO.h"
#include "AnalogIn.h"
#include "Tone.h"
#include "now.h"
#include "Configure.h"

std::shared_ptr < Configure > cfg;
bool verbose;

//# 7 - dir
//#   - tone
//# 5 - en

class StepperServo
{
  AnalogIn position;
  DigitalOut enable;
  DigitalOut direction;
  Tone pulse;

  double minPosition;
  double maxPosition;
  double frequency;
  double minFrequency;
  double maxFrequency;
  double goalPosition;
  double KP,KI,KD;
  double cutoff;

  double t;
  bool running;
  std::thread *thread;
public: 
  

  StepperServo(int _position, int _enable, int _direction, int _pulse) 
    : position(_position), 
      enable(_enable), 
      direction(_direction), 
      pulse(_pulse),
      minPosition(-180.0),
      maxPosition( 180.0),
      frequency(0.0),
      minFrequency(  0.0),
      maxFrequency(4000.0),
      goalPosition(0.0),
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
    while (running) {
      double dt = now()-t;
      t += dt;
      
      double p=position.value()/double(position.maximum);
      double error = p-goalPosition;
      if (fabs(error) > cutoff) {
	double g=-KP*maxFrequency/200.0*error;
	double df=g-frequency;
	if (fabs(df) > dt*0.0008*maxFrequency) {
	  if (df < 0) {
	    df=-dt*0.0008*maxFrequency;
	  } else {
	    df=dt*0.0008*maxFrequency;
	  }
	}
	frequency += df;
      } else {
	frequency = 0;
      }

      if (frequency > maxFrequency) {
	frequency=maxFrequency;
      }
      if (frequency < -maxFrequency) {
	frequency=-maxFrequency;
      }

      direction.value(frequency >= 0);
      enable.value(frequency != 0);
      pulse.value(fabs(frequency));
      usleep(10000);
    }
  }

  ~StepperServo()
  {
    stop();
  }
};

std::map < int , std::shared_ptr < StepperServo > > servos;

void test1()
{
  AnalogIn pot(2);
  //  DigitalOut enable(3);
  //  DigitalOut direction(4);
  Tone pulse(4);

  //  enable.value(true);
  
  for (;;) {
    double position = 2.0*double(pot.value())/double(pot.maximum)-1.0;
    double frequency = 4000.0*position;
    //    enable.value(frequency != 0.0);
    //    direction.value(frequency > 0);
    //    std::cout << "position=" << position << " frequency=" << position << std::endl;
    pulse.value(fabs(frequency));
  }
}

std::string get_id()
{
  std::string ans;

  char *HOME=getenv("HOME");
  std::string id_path=HOME;
  id_path += "/id";
  std::ifstream id_file(id_path.c_str());
  std::getline(id_file,ans);
  return ans;
}

const std::string id(get_id());


int main(int argc, char *argv[])
{
  std::cout << "id=" << id << std::endl;
  if (id != "leg1") { exit(1); }
  cfg = std::shared_ptr < Configure > ( new Configure() );
  cfg->path("../../../setup");
  cfg->args(id + ".",argv);
  if (argc == 1) cfg->load("config.csv");
  cfg->servos();
  verbose = cfg->flag(id + ".verbose",false);
  if (verbose) cfg->show();

  system("sudo modprobe adc");

  return 0;
}
