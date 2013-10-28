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

void quitproc(int param)
{
	printf("\nQuitting...\n");
	die = true;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("heartbeat.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("heartbeat.provider.verbose", false);
	if (verbose) cfg.show();

	int sleep_time = (int)cfg.num("heartbeat.provider.sleep_time", 1000);

	int hwm = 1;
	int linger = 25;
	int rc = 0;

	char strTime[80];
	time_t t;
	struct tm timeinfo;

	die = false;	

	void* context = zmq_ctx_new ();
	void* pub = zmq_socket(context, ZMQ_PUB);
	rc = zmq_setsockopt(pub, ZMQ_LINGER, &linger, sizeof(linger));
	assert(rc == 0);
	if(zmq_bind(pub, "tcp://*:9800") != 0)
	{
		printf("Could not connect zmq...\n");
		die = true;
	}

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	while(!die)
	{
		t = time(0);
		timeinfo = *localtime(&t);
		strftime(strTime, sizeof(strTime), "%Y-%m-%d(%X)", &timeinfo);

		zmq_send (pub, strTime, strlen(strTime) * sizeof(char), ZMQ_DONTWAIT);

		if(verbose){ printf ( "%s\n", strTime ); fflush(stdout);}

		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));	
	}

	printf("closing and destroying zmq...\n");
	zmq_close(pub);
	zmq_ctx_destroy(context);
	printf("--done!");

	return 0;
}
