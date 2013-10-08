#include <chrono>
#include <time.h>
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "Configure.h"

Configure cfg;
bool verbose;
volatile bool die;

void publish(void* time, void* zmq_pub)
{
	int rc = zmq_send(zmq_pub, time, strlen((char*)time) * sizeof(char), ZMQ_DONTWAIT);\
}

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

	int hwm = 1;

	char strTime[80];
	time_t t;
	struct tm timeinfo;

	void* context = zmq_ctx_new ();
	void* pub = zmq_socket(context, ZMQ_PUB);
	zmq_setsockopt(pub, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	if(zmq_bind(pub, "tcp://*:9800") != 0)
	{
		printf("Could not connect zmq...\n");
		return 1;
	}

	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);

	while(!die)
	{
		t = time(0);
		timeinfo = *localtime(&t);
		strftime(strTime, sizeof(strTime), "%Y-%m-%d(%X)", &timeinfo);
		publish(strTime, pub);
		if(verbose) printf ( "Current local time and date: %s", strTime );	
	}

	zmq_close(pub);
	zmq_ctx_destroy(context);

	return 0;
}
