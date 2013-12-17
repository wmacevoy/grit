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
#include <time.h>
#include <zmq.h>
#include "urg_ctrl.h"
#include "Configure.h"
#include "LidarMessage.h"
#include "CreateZMQServoListener.h"
#include "now.h"

Configure cfg;
bool verbose;
SPServoController servos;
Servo *waist;
Servo *neckud;
Servo *necklr;

LidarMessage message;

volatile int die = 0;
int sz_lidar_data = 1081;

void quitproc(int param) {
	die = true;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("lidar.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("lidar.provider.verbose", false);
	if (verbose) cfg.show();

	servos = std::shared_ptr < ServoController > (CreateZMQServoListener(cfg.str("servos.subscribe")));
	waist=servos->servo(91);
	neckud=servos->servo(94);
	necklr=servos->servo(93);
	servos->start();
	
	std::string address = cfg.str("lidar.provider.publish").c_str();
	std::string lidar_path = cfg.str("lidar.provider.dev_path").c_str();
	int sleep_time = (int)cfg.num("lidar.provider.sleep_time");

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	urg_t urg;
	int hwm = 1;
	int linger = 25;
	int rcl = 0;
	int t1 = 0, t2 = 0, timeOut = 3;
	bool connected = false;
	int64_t* lidar_data = NULL;

	//Initialize ZMQ and LIDAR connection
	void* context_lidar = zmq_ctx_new ();
	void* pub_lidar = zmq_socket(context_lidar, ZMQ_PUB);

	lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
	if(!lidar_data) {
	  std::cout << "Could not allocate memory for lidar data..." << std::endl;
	  die = true;
	}
	
	urg_connect(&urg, lidar_path.c_str(), 115200);

	if (verbose) std::cout << "Publishing on tcp://*:9997" << std::endl;
	while(!die)
	{
		if(connected) {
			if((rcl = urg_isConnected(&urg)) > 0) {
				rcl = urg_requestData(&urg, URG_GD, URG_FIRST, URG_LAST);
				rcl = urg_receiveData(&urg, lidar_data, sz_lidar_data);

				message.t=now();
				message.waist=waist->angle();
				message.neckud=neckud->angle();
				message.necklr=necklr->angle();
				for (int i=0; i != LidarMessage::SIZE; ++i) {
				  message.data[i]=lidar_data[i]/25.4;
				}

				if (verbose) {
				  std::cout << "t=" << message.t << " waist=" << message.waist << " neckud=" << message.neckud << " necklr=" << message.necklr << " data[0]=" << message.data[0] << std::endl; 
				}
				//				if(verbose) printf("sending lidar data...\n");
				//				int rc = zmq_send(pub_lidar, lidar_data, sizeof(int64_t) * sz_lidar_data, ZMQ_DONTWAIT);
				int rc = zmq_send(pub_lidar, &message,sizeof(message), ZMQ_DONTWAIT);
				if(verbose && rc > 0) printf("sent lidar data!\n");

				if(rc > 0) {
					t1 = time(0);
				}
			}
			else {
				if(urg_connect(&urg, lidar_path.c_str(), 115200) >= 0) {
					sz_lidar_data = urg_dataMax(&urg);
					if(verbose) printf("Max size of lidar data: %d\n", sz_lidar_data);
					std::cout << "Connected lidar..." << std::endl;			
				}
			}
		}

		t2 = time(0);
		if(t2 - t1 > timeOut) {
			connected = false;
			zmq_close(pub_lidar);
			pub_lidar = zmq_socket(context_lidar, ZMQ_PUB);
			while(!connected) {
				if(zmq_setsockopt(pub_lidar, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0) {
					if(zmq_setsockopt(pub_lidar, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
						if(zmq_bind(pub_lidar, address.c_str()) == 0) {
							connected = true;
						}
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
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
	urg_disconnect(&urg);
	printf("--done\n");

	return 0;
}
