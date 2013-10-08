#include <chrono>
#include <thread>
#include <time.h>
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "Configure.h"

Configure cfg;
bool verbose;
volatile bool die;

void subscribe(void* time, void* zmq_sub)
{
	int rc = zmq_recv(zmq_sub, time, 80, ZMQ_DONTWAIT);
}

void quitproc(int param)
{
	printf("\nQuitting...\n");
	die = true;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("heartbeat.requester.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("heartbeat.requester.verbose", false);
	if (verbose) cfg.show();

	std::string address = cfg.str("heartbeat.requester.address", "tcp://localhost:9800");
	int sleep_time = (int)cfg.num("heartbeat.requester.sleep_time", 10000);

	int hwm = 1;
	char strTime[80];
	die = false;

	void* context = zmq_ctx_new ();
	void* sub = zmq_socket(context, ZMQ_SUB);
	zmq_setsockopt(sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
	if(zmq_connect(sub, address.c_str()) != 0)
	{
		printf("Error connecting zmq...");
		die = true;
	}

	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);

	while(!die)
	{
		subscribe(strTime, sub);
		printf("Time: %s\n", strTime);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	zmq_close(sub);
	zmq_ctx_destroy(context);
	return 0;
}
