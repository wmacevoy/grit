#include <zmq.h>
#include <signal.h>
#include <iostream>
#include <SDL/SDL.h>
#include <assert.h>
#include "joystick.h"
#include "Configure.h"

Configure cfg;
bool verbose = false;
volatile bool die = false;

int last;

void publish(joystick *j,void *zmq_pub) 
{
	int rc = zmq_send(zmq_pub, j, sizeof(joystick), ZMQ_DONTWAIT);
}

void quitproc(int param)
{
	std::cout << "\nQuitting..." << std::endl;
	die = true;
}

int main(int argc,char **argv)
{
	cfg.path("../../setup");
	cfg.args("joystick.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("joystick.provider.verbose", true);
	if (verbose) cfg.show();

	joystick jm;
	SDL_Joystick *joystick;
	int rc;
	int hwm = 1;
	int linger = 25;
	void *context = zmq_ctx_new ();
	void* pub;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0)
	{
		std::cerr<< "Couldn't initialize SDL:"<< SDL_GetError()<<std::endl;
		return 1;
	}
	else 
	{
		std::cout << "Press 1 to terminate"<<std::endl;
		SDL_JoystickEventState(SDL_ENABLE);

		pub = zmq_socket(context, ZMQ_PUB);
	
		rc = zmq_setsockopt(pub, ZMQ_SNDHWM, &hwm, sizeof(hwm));
		assert(rc == 0);

		rc = zmq_setsockopt(pub, ZMQ_LINGER, &linger, sizeof(linger));
		assert(rc == 0);

		rc = zmq_bind(pub, "tcp://*:5556");
		assert(rc == 0);
	}

	joystick = SDL_JoystickOpen(0);
	SDL_Event event;

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	while(!die)
	{  
		SDL_Delay(100); // Don't check faster than 10 times a second
		while (SDL_PollEvent(&event)) 
		{
			switch(event.type)
			{  
			case SDL_JOYAXISMOTION:
				if (event.jaxis.axis==0 && event.jaxis.which==0) 
				{ 
					jm.setX1(event.jaxis.value);
					publish(&jm,pub);
				}
				else if (event.jaxis.axis==1 && event.jaxis.which==0)
				{
					jm.setY1(event.jaxis.value); 
					publish(&jm,pub);
				}
				else if (event.jaxis.axis==2 && event.jaxis.which==0) 
				{ 
					jm.setX2(event.jaxis.value / -1000);
					publish(&jm,pub);
				}
				else if (event.jaxis.axis==3 && event.jaxis.which==0)
				{
					jm.setY2(event.jaxis.value / -1000); 
					publish(&jm,pub);
				} 
				break;
			case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
				if ( event.jbutton.button==0 && event.jbutton.which==0)
				{
					jm.setButtonDown(1);
					publish(&jm,pub); 
					die = true;
				}
				else if ( event.jbutton.button == 1 && event.jbutton.which==0 ) 
				{
					jm.setButtonDown(2);
					publish(&jm,pub);
				}
				break;
			case SDL_JOYBUTTONUP:  /* Handle Joystick Button Presses */
				if ( event.jbutton.button == 1 && event.jbutton.which==0 ) 
				{
					jm.setButtonUp(2);
					publish(&jm,pub);
				}
				break;
			case SDL_QUIT:
				die = true;
				break;
			}
		}
		if(verbose) std::cout << "X1: " << jm.x1 << " Y1: " << jm.y1 << "X2: " << jm.x2 << " Y2: " << jm.y2 << " Button1: " << jm.button1 << " Button2: " << jm.button2 << std::endl;
	}

	std::cout << "closing and destroying zmq..." << std::endl;
	zmq_close(pub);
	zmq_ctx_destroy(context);
	std::cout << "--done!" << std::endl;
	std::cout << "closing SDL..." << std::endl;
	SDL_JoystickClose( joystick );
	SDL_Quit();
	std::cout << "--done!" << std::endl;

	return 0;
}
