/*
 * Provider for the lidar
 * Written by Michaela Ervin and based off of c_urg samples
*/

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <zmq.h>
#include "urg_ctrl.h"
#include "Configure.h"

Configure cfg;
bool verbose;

volatile int die = 0;

int sleep_time;

urg_t urg;
int64_t* lidar_data  = NULL;
int sz_lidar_data;
int sz_lidar_data_max;

void publish_lidar(void* data, void* zmq_pub)
{
	int ret;
	
	if (urg_isConnected(&urg) < 0) 
	{
		return;
	}

	ret = urg_requestData(&urg, URG_GD, URG_FIRST, URG_LAST);
	if (ret < 0) 
	{
		return;
	}

	ret = urg_receiveData(&urg, lidar_data, sz_lidar_data);;
	if(verbose) printf("# n = %d\n", ret);
	if (ret < 0)
	{
		return;
	}
	
	if(verbose) printf("waiting for lidar data...\n");
	int rc = zmq_send(zmq_pub, lidar_data, sizeof(int64_t) * sz_lidar_data, ZMQ_DONTWAIT);
	if(verbose && rc > 0) printf("received lidat data!\n");
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("lidar.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("lidar.provider.verbose", false);
	if (verbose) cfg.show();
	

	int res;
	int hwm = 1;
	int rcl = 0;

	std::string lidar_path = cfg.str("lidar.provider.path", "/dev/ttyACM0").c_str();
	sleep_time = (int)cfg.num("lidar.provider.sleep_time", 25);

	//Initialize ZMQ and LIDAR connection
	void* context_lidar = zmq_ctx_new ();
	void* pub_lidar = zmq_socket(context_lidar, ZMQ_PUB);
	rcl = zmq_setsockopt(pub_lidar, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	assert(rcl == 0);

	rcl = zmq_bind(pub_lidar, "tcp://*:9997");

	//Connect lidar
	int ret = urg_connect(&urg, lidar_path.c_str(), 115200);
	if (ret < 0) {
		return 1;
	}

	//Get max size of lidar data
	sz_lidar_data = urg_dataMax(&urg);
	if(verbose) printf("Max size of lidar data: %d\n", sz_lidar_data);
	
	lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
	assert(lidar_data != NULL);

	if (verbose) printf("Publishing on tcp://*:9997\n");
	while(!die)
	{
		publish_lidar(lidar_data, pub_lidar);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	if (verbose) printf("freeing memory for data array...\n");

	free(lidar_data);

	if (verbose) printf("--done\n");
	
	if (verbose) printf("closing urg...\n");
	
	urg_disconnect(&urg);
	
	if (verbose) printf("--done\n");
	if (verbose) printf("closing and destroying zmq\n");

	zmq_close(pub_lidar);
	zmq_ctx_destroy(context_lidar);

	if (verbose) printf("-- done!\n");

	return 0;
}
