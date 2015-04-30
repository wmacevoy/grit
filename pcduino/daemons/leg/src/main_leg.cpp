#include <iostream>
#include <fstream>
#include <math.h>
#include <thread>
#include <chrono>
#include <memory>
#include <iomanip>

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
public:
  AnalogIn position;
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
  

  StepperServo(int _position, int _direction, int _pulse)
    : position(_position), 
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
      std::cout << "p=" << p << std::endl;
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
      pulse.value(1000*fabs(frequency));
      std::cout << "freq=" << frequency << std::endl;
      usleep(10000);
    }
  }

  ~StepperServo()
  {
    stop();
  }
};

std::map < int , std::shared_ptr < StepperServo > > servos;

void test2()
{
  DigitalOut enable(9);
  StepperServo servo(3,5,6);
  AnalogIn pot(4);

  enable.value(1);
  for (;;) {
    double value = double(pot.value()) / double(pot.maximum);
    servo.goalPosition = value;
    //    std::cout << value << std::endl;
    usleep(100000);
  }
}

void test0()
{
  DigitalOut enable(8); 
  enable.value(0);
  DigitalOut dir(6);
  
  
  std::cout << "test0" << std::endl;
  //DigitalOut enable(8);
  Tone pin(7);
  pin.value(25000);
  for (;;) {
	 dir.value(0);
    sleep(5);
    dir.value(1);
    sleep(5);
  }
}

void test1()
{
  AnalogIn pot3(3);
  AnalogIn pot2(4);
  AnalogIn pot1(5);
  
  DigitalOut direction3(6);
  DigitalOut direction2(4);
  DigitalOut direction1(2);
  Tone pulse3(7);
  Tone pulse2(5);
  Tone pulse1(3);
  DigitalOut enable(8);                                  
  
  for (;;) {
    double position1 = 2.0*double(pot1.value())/double(pot1.maximum)-1.0;
    double frequency1 = 30000.0*position1;
    
    double position2 = 2.0*double(pot2.value())/double(pot2.maximum)-1.0;
    double frequency2 = 30000.0*position2;
    
    double position3 = 2.0*double(pot3.value())/double(pot3.maximum)-1.0;
    double frequency3 = 30000.0*position3;
    
    enable.value(frequency1 == 0.0 || frequency2 == 0.0 || frequency3 == 0.0);
    
    direction1.value(frequency1 > 0);
    direction2.value(frequency2 > 0);
    direction3.value(frequency3 > 0);
    
    std::cout << std::setw(5) << std::fixed << std::setprecision(3) << "p1=" << position1 << "  f1=" << frequency1;
    std::cout << std::setw(5) << std::fixed << std::setprecision(3) << "  p2=" << position2 << "  f2=" << frequency2; 
    std::cout << std::setw(5) << std::fixed << std::setprecision(3) << "  p3=" << position3 << "  f3=" << frequency3 << std::endl;
    //std::cout << "position3=" << position3 << " frequency3=" << frequency3 << std::endl;
    pulse1.value(fabs(frequency1));
    pulse2.value(fabs(frequency2));
    pulse3.value(fabs(frequency3));
    
    usleep(1000);
  }
}
/*
std::string get_id();

const std::string id(get_id());


std::string get_id()
{
  std::string ans;

  char *HOME=getenv("HOME");
  std::string id_path=HOME;
  id_path += "/id";
  std::cout << "id=" << id;
  std::ifstream id_file(id_path.c_str());
  std::getline(id_file,ans);
  return ans;
}
*/

int main(int argc, char *argv[])
{
  //  if (id != "leg1") { exit(1); }
  //  cfg = std::shared_ptr < Configure > ( new Configure() );
  //  cfg->path("../../setup");
  //  cfg->args(id + ".",argv);
  //  if (argc == 1) cfg->load("config.csv");
  //  cfg->servos();
  //  verbose = cfg->flag(id + ".verbose",false);
  //  if (verbose) cfg->show();

  system("sudo modprobe adc");

  test1();

  return 0;
}
