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

void subscribe(void* time, void* zmq_sub)
{
	int rc = zmq_recv(zmq_sub, time, 80 * sizeof(char), 0);
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
	int sleep_time = (int)cfg.num("heartbeat.requester.sleep_time", 1000);

	char strTime[80];
	char strTime1[80];
	time_t t;
	struct tm timeinfo;
	die = false;
	rc = 0;

	void* context = zmq_ctx_new ();
	void* sub = zmq_socket(context, ZMQ_SUB);
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
		if(verbose) printf("Sending time request...\n");
		t = time(0);
		timeinfo = *localtime(&t);
		strftime(strTime1, sizeof(strTime1), "%Y-%m-%d(%X)", &timeinfo);
		
		subscribe(strTime, sub);
		
		printf ("Control time: %s  Centaur time: %s\n", strTime1, strTime);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	zmq_close(sub);
	zmq_ctx_destroy(context);
	return 0;
}
