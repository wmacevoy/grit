#include <chrono>
#include <thread>
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "Configure.h"

Configure cfg;
bool verbose;
volatile bool die;
int rc;

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
	int sleep_time = (int)cfg.num("heartbeat.requester.sleep_time", 1000);

	char strTime[80];
	die = false;
	rc = 0;

	void* context = zmq_ctx_new ();
	void* req = zmq_socket(context, ZMQ_REQ);
	if(zmq_connect(req, address.c_str()) != 0)
	{
		printf("Error connecting zmq...");
		die = true;
	}

	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);

	while(!die)
	{
		if(verbose) printf("Sending time request...\n");
		zmq_send (req, "t", sizeof(char), 0);
		zmq_recv (req, strTime, 80, 0);
		printf ("Time: %s\n", strTime);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	zmq_close(req);
	zmq_ctx_destroy(context);
	return 0;
}
