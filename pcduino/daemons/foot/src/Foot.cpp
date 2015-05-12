#include <math.h>
#include <iostream>
#include "Foot.h"


Foot::Foot(int positionPin, int enablePin, 
	   int leftDirectionPin, int rightDirectionPin,
	   int leftPulsePin,int rightPulsePin)
  : position(positionPin), enable(enablePin),
    leftDirection(leftDirectionPin), rightDirection(rightDirectionPin), 
    leftPulse(leftPulsePin), rightPulse(rightPulsePin),
    cutoff(0.1),reversed(1),goalHeading(0.5),goalSpeed(0),running(true),
    thread(&Foot::run,this)
{
}



// freq 2 feet per second
const double Foot::TO_FPS=(2*M_PI*2.0/12.0)/(4*200); // freq 2 feet per second
// freq 2 radians per second
const double Foot::TO_RPS=(2*M_PI*3.0/12.0)/(4*200); 

const double Foot::KP=10.0/TO_RPS;

double Foot::getCurrentHeading()
{
  double value=position.value() / (position.maximum*0.90); // 10% resistor on +VDD
  if (reversed == -1) {
    value = value+0.5;
  }
  value=value-floor(value);
  return value;
}

void Foot::setGoalHeading(double value)
{
  value = value - floor(value);

  if (reversed == -1) {
    if (0.3 < value && value  < 0.70) {
      reversed = 1;
    }
  } else {
    if (value < 0.2 || 0.8 < value) {
      reversed = -1;
    }
  }
  goalHeading=value;
}

void Foot::setGoalSpeed(double value)
{
  goalSpeed = value/TO_FPS;
}

void Foot::run()
{
  while (running) {
    double currentHeading=getCurrentHeading();
    double delta = currentHeading-goalHeading;
    //    if (delta < -0.5) delta += 1.0;
    // else if (delta > 0.5) delta -= 1.0;

    double leftFrequency  = -KP*delta+reversed*goalSpeed;
    double rightFrequency =  KP*delta+reversed*goalSpeed;

    enable.value(1);
    leftDirection.value(leftFrequency >= 0);
    leftPulse.value(abs(leftFrequency));
    rightDirection.value(rightFrequency <= 0);
    rightPulse.value(abs(rightFrequency));

    std::cout << "reversed=" << reversed << " currentHeading = " << currentHeading << " goalHeading=" << goalHeading << " left=" << leftFrequency << " right=" << rightFrequency << std::endl; 
    usleep(100000);
  }
}

Foot::~Foot()
{
  running = false;
  thread.join();
}
