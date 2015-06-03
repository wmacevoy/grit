/*
 * Provider for the lidar
 * Written by Michaela Ervin
*/

#include <boost/asio.hpp>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/select.h>

#include "Configure.h"
#include "now.h"

Configure cfg;
bool verbose;

volatile int die = 0;

void quitproc(int param) {
	printf("quitting...");
	die = true;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("lidar.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("lidar.provider.verbose", false);
	if (verbose) cfg.show();
	
	std::string address = cfg.str("lidar.provider.c_ip").c_str();
	std::string port = cfg.str("lidar.provider.port").c_str();
	std::string lidar_path = cfg.str("lidar.provider.dev_path").c_str();
	int sleep_time = (int)cfg.num("lidar.provider.sleep_time");

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);
	
	bool connected = false;
	
	char buffer[5];
    int fd=-1;


	std::cout <<address<<", "<<port<<std::endl;
	//Boost items
	boost::asio::io_service io_service;
	boost::asio::ip::udp::resolver resolver(io_service);
	boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), address, port);
	boost::asio::ip::udp::endpoint receiver_endpoint = *resolver.resolve(query);
	boost::asio::ip::udp::socket socket(io_service);
	socket.open(boost::asio::ip::udp::v4());
	connected = false;	

	//fd = open(lidar_path.c_str(),O_RDWR|O_NONBLOCK);
	fd = open(lidar_path.c_str(),O_RDWR);
	 if (verbose) {
		std::cout << "open(" << lidar_path << ")=" << fd << std::endl;
	 }
	 
	int dist = -1;
	while(!die)
		{
		if(fd >= 0) 
		 {
              struct timeval ts;
			  ts.tv_sec = 1;
			  ts.tv_usec = 0;

			  fd_set set;
			  int rv;

			  FD_ZERO(&set);
			  FD_SET(fd, &set);

			  rv = select(fd+1, &set, NULL, NULL, &ts);
			  std::cout << "rv: " << rv <<std::endl;
			  if (rv > 0) {
				 memset(buffer, '\0', 5);
				 int rec = read(fd,buffer,5);
				 //buffer[0] = '6';
				 //buffer[1] = '9';
				 //buffer[2] = '\0';
				 std::cout << "read in: " << rec << ", buffer: " << buffer << std::endl;
				 socket.send_to(boost::asio::buffer(buffer, 5), receiver_endpoint);
			  }
		 }
		else 
			{
              fd = open(lidar_path.c_str(),O_RDWR|O_NONBLOCK);
			  if (verbose) {
				std::cout << "open(" << lidar_path << ")=" << fd << std::endl;
			  }
			}

		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	std::cout << std::endl << "Quitting..." << std::endl;
	printf("closing lidar file..\n");
	if (fd >= 0) close(fd);
	printf("--done\n");
	printf("closing and destroying zmq\n");
    socket.close();
	printf("-- done!\n");
	
	return 0;
}
