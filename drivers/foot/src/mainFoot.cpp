#include "mainFoot.h"

void Foot::run() 
	{
	while (running) 
		{
		direction = destination<0.5?1:0;
		int tmp_destination = destination<0.5?destination:destination-0.5;
		
		//Rotate first
		double pos = pot.value();
		if(pos < (tmp_destination - cutoff) || pos > (tmp_destination + cutoff))
			{
			move(dir_speed, dir, true);
			}
		//Drive forward or backward second.
		else if(mov_speed > 0)
			{
			move(mov_speed, dir, false);
			}
		//No movent needed.
		else
			{
			enable.value(0);
			}
		usleep(1000);
		}
	}
	
private: void move(int speed, int dir, boolean rotate)
	{
	enable.value(1);
			
	//Set each motor direction
	direction_l.value(direction);
	direction_r.value(rotate?direction:!direction);
	
	//Pulse at the specified speed
	pulse_l.value(speed);
	pulse_r.value(speed);
	}

Foot::Foot(int potpin, int enablepin, int dlpin, int drpin, int plpin, int prpin):
			 pot(potpin), enable(enablepin),
			 direction_l(dlpin), direction_r(drpin),
			 pulse_l(plpin), pulse_r(prpin), 
			 dir_speed(0), mov_speed(0), 
			 destination(512), direction(1);
			 cutoff(0), running(true), 
			 thread(&Foot::run,this), cutoff(0.01){}
			 
	
void Foot::setDirSpeed(int _speed)
	{
	this.dir_speed = _speed;
	}
void Foot::setMovSpeed(int _speed)
	{
	this.mov_speed = _speed;
	}
void Foot::setDirection(boolean _direction)
	{
	this.direction = _direction;
	}
void Foot::setDestination(double _destination)
	{
	this.destination = _destination;
	}
Foot::~Foot()
	{
	running = false;
	thread.join();
	}
