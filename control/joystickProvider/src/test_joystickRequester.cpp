#include <iostream>
#include <zmq.h>
#include <assert.h>
#include <thread>
#include <chrono>
#include "joystick.h"
#include "Configure.h"

Configure cfg;

bool verbose = false;

void subscribe(joystick* j, void* zmq_sub)
{
	int rc = zmq_recv(zmq_sub, j, sizeof(joystick), ZMQ_DONTWAIT);
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("joystick.requester.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("joystick.requester.verbose", false);
	if (verbose) cfg.show();

	std::string address = cfg.str("joystick.requester.address", "tcp://localhost");
	address += ":5555";	

	joystick jm;
	int rc;
	int hwm = 1;
	bool die = false;

	void* context = zmq_ctx_new();
	void* sub = zmq_socket(context, ZMQ_SUB);

	rc = zmq_setsockopt(sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	assert(rc == 0);
	
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
	assert(rc == 0);

	rc = zmq_connect(sub, address.c_str());
	assert(rc == 0);

	while(!die)
	{
		subscribe(&jm, sub);
		std::cout << "X1: " << jm.x1 << " Y1: " << jm.y1 << "X2: " << jm.x2 << " Y2: " << jm.y2 << " Button1: " << jm.button1 << " Button2: " << jm.button2 << std::endl;
		if(jm.button1) die = true;
		std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}

	zmq_close(sub);
	zmq_ctx_destroy(context);

	return 0;
}
