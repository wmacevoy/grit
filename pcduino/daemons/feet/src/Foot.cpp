#include <math.h>
#include <iostream>
#include "Foot.h"


std::shared_ptr<Foot> Foot::factory(const std::string &id,
				    const Configure &cfg, 
				    int dev)
{
  std::string dev_id = id + ".dev" + (char)('0'+dev);
  
  int       positionPin = cfg.num(dev_id + ".pins.position");
  int         enablePin = cfg.num(dev_id + ".pins.enable");
  int  leftDirectionPin = cfg.num(dev_id + ".pins.direction.left");
  int rightDirectionPin = cfg.num(dev_id + ".pins.direction.right");
  int       leftStepPin = cfg.num(dev_id + ".pins.step.left");
  int      rightStepPin = cfg.num(dev_id + ".pins.step.right");

  Foot *foot = new Foot(positionPin,enablePin,
			leftDirectionPin,rightDirectionPin,
			leftStepPin,rightStepPin);

  foot->kp = cfg.num(dev_id + ".kp",cfg.num("feet.kp",foot->kp));
  foot->kturn = cfg.num(dev_id + ".kturn",cfg.num("feet.kturn",foot->kturn));
  foot->kmove = cfg.num(dev_id + ".kmove",cfg.num("feet.kmove",foot->kmove));
  foot->verbose = cfg.flag(dev_id + ".verbose",
			   cfg.flag("feet.verbose",foot->verbose));

  if (foot->verbose) {
    std::cout << "id=" << dev_id 
	      << " at=" << positionPin
	      << " en=" << enablePin
	      << " ld=" << leftDirectionPin
	      << " rd=" << rightDirectionPin
	      << " ls=" << leftStepPin
	      << " rs=" << rightStepPin
	      << " kp=" << foot->kp
	      << " kt=" << foot->kturn
	      << " km=" << foot->kmove
	      << std::endl;
  }
      
  return std::shared_ptr <Foot> (foot);
}

Foot::Foot(int positionPin, int enablePin, 
	   int leftDirectionPin,  int rightDirectionPin,
	   int leftStepPin, int rightStepPin)
  : position(positionPin), enable(enablePin),
    leftDirection(leftDirectionPin), rightDirection(rightDirectionPin), 
    leftStep(leftStepPin), rightStep(rightStepPin),
    cutoff(0.1),reversed(1),goalHeading(0.5),goalSpeed(0),running(false),
    thread(0)
{
  kturn=0.025;
  kmove=0.050;
  kp=40.0/TO_RPS;
  verbose=false;
}

void Foot::start()
{
  if (thread == 0) {
    running = true;
    thread = new std::thread(&Foot::run,this);
  }
}

void Foot::stop()
{
  if (thread != 0) {
    running = false;
    thread->join();
    delete thread;
    thread=0;
  }
}

bool Foot::isRunning() 
{ 
  return (thread != 0);
}

// freq 2 feet per second
const double Foot::TO_FPS=(2*M_PI*2.0/12.0)/(4*200); // freq 2 feet per second
// freq 2 radians per second
const double Foot::TO_RPS=(2*M_PI*3.0/12.0)/(4*200); 


double Foot::getCurrentHeading()
{
  // 10% resistor on +VDD
  double value=position.value() / (position.maximum*0.90); 

  return value;
}

void Foot::setGoalHeading(double value)
{
  value = value - floor(value);

  if (reversed == -1) {
    if (0.3 < value && value  < 0.70) {
      reversed = 1;
      goalSpeed = -goalSpeed;
    }
  } else {
    if (value < 0.2 || 0.8 < value) {
      reversed = -1;
      goalSpeed = -goalSpeed;
    }
  }

  if (reversed == -1) {
    value += 0.5;
    value = value - floor(value);
  }

  goalHeading=value;
}

void Foot::setGoalSpeed(double value)
{
  goalSpeed = reversed*value/TO_FPS;
}

void Foot::run()
{
  while (running) {
    double currentHeading=getCurrentHeading();
    double delta = currentHeading-goalHeading;
    if (delta < -0.25) {
      delta = -0.25;
    } else if (delta > 0.25) {
      delta = 0.25;
    }
    int move = fabs(delta) < kmove;
    int turn = fabs(delta) > kturn;

    double leftFrequency  = -turn*kp*delta+move*goalSpeed;
    double rightFrequency =  turn*kp*delta+move*goalSpeed;

    enable.value((leftFrequency != 0) || (rightFrequency != 0));
    leftDirection.value(leftFrequency >= 0);
    leftStep.value(abs(leftFrequency));
    rightDirection.value(rightFrequency <= 0);
    rightStep.value(abs(rightFrequency));

    if (verbose) {
      std::cout << "reversed=" << reversed << " currentHeading = " << currentHeading << " goalHeading=" << goalHeading << " left=" << leftFrequency << " right=" << rightFrequency << std::endl; 
    }
    usleep(100000);
  }
}

Foot::~Foot()
{
  stop();
}
