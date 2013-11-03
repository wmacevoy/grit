/*
 * Provider for the lidar
 * Written by Michaela Ervin and based off of c_urg samples
*/

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <zmq.h>
#include "urg_ctrl.h"
#include "Configure.h"

Configure cfg;
bool verbose;

volatile int die = 0;

int sz_lidar_data;
int sz_lidar_data_max;

void quitproc(int param)
{
	std::cout << "\nQuitting..." << std::endl;
	die = true;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("lidar.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("lidar.provider.verbose", false);
	if (verbose) cfg.show();
	
	std::string lidar_path = cfg.str("lidar.provider.dev_path");
	int sleep_time = (int)cfg.num("lidar.provider.sleep_time");

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	urg_t urg;
	int hwm = 1;
	int linger = 25;
	int rcl = 0, ret;
	int retry = 0;
	int64_t* lidar_data = NULL;
	
	std::string msg = "";

	//Initialize ZMQ and LIDAR connection
	void* context_lidar = zmq_ctx_new ();
	void* pub_lidar = zmq_socket(context_lidar, ZMQ_PUB);
	rcl = zmq_setsockopt(pub_lidar, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	assert(rcl == 0);

	rcl = zmq_setsockopt(pub_lidar, ZMQ_LINGER, &linger, sizeof(linger));
	assert(rcl == 0);

	rcl = zmq_bind(pub_lidar, "tcp://*:9997");
	assert(rcl == 0);

	//Connect lidar
	rcl = urg_connect(&urg, lidar_path.c_str(), 115200);
	if (rcl != 0) {
	  std::cout << "failed urg_connect()" << std::endl;
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
		if ((rcl = urg_isConnected(&urg)) == 0)
		{
			if((rcl = urg_requestData(&urg, URG_GD, URG_FIRST, URG_LAST)) == 0)
			{
				if((rcl = urg_receiveData(&urg, lidar_data, sz_lidar_data)) == 0)
				{
					if(verbose) printf("sending lidar data...\n");
					int rc = zmq_send(pub_lidar, lidar_data, sizeof(int64_t) * sz_lidar_data, ZMQ_DONTWAIT);
					if(verbose && rc > 0) printf("sent lidat data!\n");
				}
				else
				{	
					std::cout << "failed urg_receiveData() with value: " << rcl << std::endl;
					die = true;
				}
			}
			else 
			{
				std::cout << "failed urg_requestData() with value: " << rcl << std::endl;
				die = true;
			}

		}
		else 
		{
			std::cout << "failed urg_isConnected() with value: "<< rcl << std::endl;
			++retry;
			if(retry >= 25) {				
				die = true;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	printf("freeing memory for data array...\n");
	free(lidar_data);
	printf("--done\n");
	printf("closing and destroying zmq\n");
	zmq_close(pub_lidar);
	zmq_ctx_destroy(context_lidar);
	printf("-- done!\n");
	printf("closing urg...\n");
	urg_disconnect(&urg);
	printf("--done\n");

	return 0;
}
