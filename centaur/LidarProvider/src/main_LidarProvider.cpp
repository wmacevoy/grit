/*
 * Provider for the lidar
 * Written by Michaela Ervin and based off of c_urg samples
*/

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
int sz_lidar_data = 0;

void quitproc(int param)
{
	die = true;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("lidar.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("lidar.provider.verbose", false);
	if (verbose) cfg.show();
	
	std::string lidar_path = cfg.str("lidar.provider.dev_path").c_str();
	int sleep_time = (int)cfg.num("lidar.provider.sleep_time");

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	urg_t urg;
	int hwm = 1;
	int linger = 25;
	int rcl = 0;
	int retries = 5;
	bool connected = false;
	int64_t* lidar_data = NULL;

	//Initialize ZMQ and LIDAR connection
	void* context_lidar = zmq_ctx_new ();
	void* pub_lidar = zmq_socket(context_lidar, ZMQ_PUB);

	while(!connected && retries--) {
		if(zmq_setsockopt(pub_lidar, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0) {
			if(zmq_setsockopt(pub_lidar, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
				if(zmq_bind(pub_lidar, "tcp://*:9997") == 0) {
					retries = 5;
					connected = true;
				}
			}
		}
		if(retries <= 0) {
			std::cout << "Could not bind to tcp://*:9997..." << std::endl;
			die = true;
		}
	}

	lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
	if(!lidar_data) {
		std::cout << "Could not allocate memory for lidar data..." << std::endl;
		die = true;
	}

	if (verbose) std::cout << "Publishing on tcp://*:9997" << std::endl;
	while(!die)
	{
		std::cout << retries << std::endl;
		if(urg_isConnected(&urg) >= 0) {
			rcl = urg_requestData(&urg, URG_GD, URG_FIRST, URG_LAST);
			rcl = urg_receiveData(&urg, lidar_data, sz_lidar_data);

			if(verbose) printf("sending lidar data...\n");
			int rc = zmq_send(pub_lidar, lidar_data, sizeof(int64_t) * sz_lidar_data, ZMQ_DONTWAIT);
			if(verbose && rc > 0) printf("sent lidat data!\n");
		}
		else {
			if(urg_connect(&urg, lidar_path.c_str(), 115200) >= 0) {
				sz_lidar_data = urg_dataMax(&urg);
				if(verbose) printf("Max size of lidar data: %d\n", sz_lidar_data);
				retries = 5;			
			}
			else if(retries-- <= 0) {
				std::cout << "Retry limit reached and lidar cannot connect..." << std::endl;
				die = true;			
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	std::cout << std::endl << "Quitting..." << std::endl;
	printf("freeing memory for data array...\n");
	free(lidar_data);
	printf("--done\n");
	printf("closing and destroying zmq\n");
	zmq_close(pub_lidar);
	zmq_ctx_destroy(context_lidar);
	printf("-- done!\n");
	printf("closing urg...\n");
	if(urg_isConnected(&urg) >= 0) urg_disconnect(&urg);
	printf("--done\n");

	return 0;
}
