#include "mainFoot.h"

void Foot::run() 
	{
	while (running) 
		{
		if((pot.value()-cutoff) != detination)
			{
			enable.value(1);
			
			//Set each motor direction
			boolean dir = destination<pot.value()?0:1;
			direction_l.value(dir);
			direction_r.value(dir);
			
			//Pulse at the specified speed
			pulse_l.value(speed);
			pulse_r.value(speed);
			}
		else
			{
			enable.value(0);
			}
			
		usleep(1000);
		}
	}

Foot::Foot():pot(2), enable(3),
			 direction_l(4), direction_r(5),
			 pulse_l(6), pulse_r(7), 
			 speed(0), destination(512), direction(1);
			 cutoff(0), running(true), 
			 thread(&Foot::run,this){}
	
void Foot::drive(int speed, boolean direction)
	{
	if(speed > 0)
		{
		enable.value(1);
			
		//Set each motor direction
		boolean dir = destination<pot.value()?0:1;
		direction_l.value(dir);
		direction_r.value(!dir);
		
		//Pulse at the specified speed
		pulse_l.value(speed);
		pulse_r.value(speed);
		}
	else
		{
		enable.value(0);
		}
	}
	
void Foot::setSpeed(int _speed)
	{
	this.speed = _speed;
	}
	
void Foot::setDirection(boolean _direction)
	{
	this.direction = _direction;
	}
void Foot::setDestination(int _destination)
	{
	this.destination = _destination;
	}

Foot::~Foot()
	{
	running = false;
	thread.join();
	}
