#include <iostream>
#include <zmq.h>
#include <assert.h>
#include <chrono>
#include <thread>
#include <signal.h>
#include "leapStruct.h"
#include "Configure.h"

leapData leapD;
Configure cfg;
bool verbose;
volatile int die = 0;
int sleep_time = 100000;

void subscribe(leapData* data, void* zmq_sub)
{
	if(verbose) std::cout << "Receiving leap data..." << std::endl;
	int rc = zmq_recv(zmq_sub, data, sizeof(leapData), ZMQ_DONTWAIT);
	if(verbose && rc > 0) std::cout << "Leap data received!" << std::endl;
}

void quitproc(int sig)
{
	if(verbose) printf("\nQuitting...\n");
	die = 1;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("leap.requester.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("leap.requester.verbose", false);
	if (verbose) cfg.show();
	sleep_time = (int)cfg.num("leap.requester.sleep_time", 400);

	std::string address = "tcp://";
	address += cfg.str("leap.requester.address", "localhost");
	address += ":9990";

	int hwm = 1;
	int rc = 0;

	void* context = zmq_ctx_new ();
	void* sub = zmq_socket(context, ZMQ_SUB);
	rc = zmq_setsockopt(sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	assert(rc == 0);

	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
	assert(rc == 0);

	rc = zmq_connect(sub, address.c_str());
	assert(rc == 0);

	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);

	while(!die)
	{
		subscribe(&leapD, sub);
		if(verbose) std::cout << "LEFT: " << leapD.lx << " " << leapD.ly << " " << leapD.lz << " " << leapD.lroll << std::endl;
		if(verbose) std::cout << "RIGHT: " << leapD.rx << " " << leapD.ry << " " << leapD.rz << " " << leapD.rroll << std::endl;
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
	}

	zmq_close(sub);
	zmq_ctx_destroy(context);

	return 0;
}
