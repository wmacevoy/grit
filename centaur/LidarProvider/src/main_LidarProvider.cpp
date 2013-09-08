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
//#include "Configure.h"

//Configure cfg;
bool verbose;

volatile int die = 0;

const int sleep_time = 25;

urg_t urg;
//int lidar_data_max;
int64_t * lidar_data  = NULL;
int sz_lidar_data;

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

	ret = urg_receiveData(&urg, lidar_data, sz_lidar_data);
	if(verbose) printf("# n = %d\n", ret);
	if (ret < 0)
	{
		return;
	}
	
	int rc = zmq_send(zmq_pub, lidar_data, sizeof(int64_t) * sz_lidar_data, ZMQ_DONTWAIT);
}

int main(int argc, char** argv)
{
	//cfg.path("../../setup");
	//cfg.args("lidar.provider.", argv);
	//if (argc == 1) cfg.load("config.csv");
	//verbose = cfg.flag("lidar.provider.verbose", false);
	//if (verbose) cfg.show();
	

	int res;
	int hwm = 1;
	int rcl = 0;

	const char lidar_path[] = "/dev/ttyACM0"; /* For Linux */

	//Initialize ZMQ and LIDAR connection
	void* context_lidar = zmq_ctx_new ();
	void* pub_lidar = zmq_socket(context_lidar, ZMQ_PUB);
	rcl = zmq_setsockopt(pub_lidar, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	assert(rcl == 0);

	rcl = zmq_bind(pub_lidar, "tcp://*:9997");

	//Connect lidar
	int ret = urg_connect(&urg, lidar_path, 115200);
	if (ret < 0) {
		return 1;
	}

	//Get max size of lidar data
	sz_lidar_data = urg_dataMax(&urg);
	if(verbose) printf("Max size of lidar data: %d", sz_lidar_data);
	
	lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
	assert(lidar_data != NULL);

	printf("Publishing on tcp://*:9997\n");
	while(!die)
	{
		publish_lidar(lidar_data, pub_lidar);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	printf("freeing memory for data array...\n");

	free(lidar_data);

	printf("--done\n");
	
	printf("closing urg...\n");
	
	urg_disconnect(&urg);
	
	printf("--done\n");
	printf("closing and destroying zmq\n");

	zmq_close(pub_lidar);
	zmq_ctx_destroy(context_lidar);

	printf("-- done!\n");

	return 0;
}
