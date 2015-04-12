#include "main_Foot.h"

void Foot::Run() 
	{
	while (running) 
		{
		pos = pot.value();
		tmp_destination = destination<0.5?destination:destination-0.5;
		error = pos - tmp_destination;
		
		//Rotate first, if our error is too great we want to only rotate
		if(abs(error) > cutoff)
			{
			if(pos < tmp_destination)
				{
				move(spd_rotate, spd_rotate, direction, true); //TODO: Check direction
				}
			else if(pos > tmp_destination)
				{
				move(spd_rotate, spd_rotate, !direction, true); //TODO: Check direction
				}
			}
		//Drive forward or backward, we will use the error to make small speed adjustments to try and stay in out goal position.
		else if(spd_move > 0)
			{
			direction = destination<0.5?1:0;
			
			int speed_l = spd_move;
			int speed_r = spd_move;
			if(error < 0) //TODO: Check direction
				{
				speed_r -= 100*error; //TODO: test multiplier
				}
			else
				{
				speed_l -= 100*error; //TODO: test multiplier
				}
			
			move(speed_l, speed_r, direction, false);
			}
		//No movent needed.
		else
			{
			enable.value(0);
			}
		usleep(1000);
		}
	}
	
void Foot::move(int speed_l, int speed_r, int dir, bool rotate)
	{
	enable.value(1);
			
	//Set each motor direction
	direction_l.value(dir);
	direction_r.value(rotate?dir:!dir);
	
	//Pulse at the specified speed
	pulse_l.value(speed_l);
	pulse_r.value(speed_r);
	}

Foot::Foot(int potpin, int enablepin, int dlpin, int drpin, int plpin, int prpin):
			 pot(potpin), enable(enablepin),
			 direction_l(dlpin), direction_r(drpin),
			 pulse_l(plpin), pulse_r(prpin), 
			 cutoff(0.05), error(0), pos(0), tmp_destination(0),
			 spd_rotate(0), spd_move(0), 
			 destination(512), direction(1),
			 running(true), thread(&Foot::Run,this){}
			 
	
void Foot::setDirSpeed(int _speed)
	{
	spd_rotate = _speed;
	}
void Foot::setMovSpeed(int _speed)
	{
	spd_move = _speed;
	}
void Foot::setDirection(int _direction)
	{
	direction = _direction;
	}
void Foot::setDestination(double _destination)
	{
	destination = _destination;
	}
bool Foot::isRunning()
	{
	return running;
	}
Foot::~Foot()
	{
	running = false;
	thread.join();
	}
	
void testFoot()
	{
	printf("In Foot Test\n");
	Foot f(2,3,4,5,6,7);
	
	while(f.isRunning())
		{
		usleep(100000);
		}
	}

int main(int argc, char** argv)
	{
	printf("In Foot\n");
	testFoot();
	return 0;
	}
