#ifndef LIDARLAYER_H_
#define LIDARLAYER_H_

#include <iostream>
#include <sstream>
#include <iomanip>
#include <zmq.h>
#include <time.h>
#include <ctype.h>
#include "opencv2/opencv.hpp"
#include <boost/asio.hpp>

using namespace cv;


class LidarLayer {
	std::vector<uchar> buff;
    int port;
    int MAX_SIZE;
	
    boost::asio::io_service my_io_service;
    boost::asio::ip::udp::socket *my_socket;
    boost::asio::ip::udp::endpoint sender_endpoint;
	
	int index;
	float timeOut;
	
	int fontFace;
	double fontScale;
	int thickness;

	bool verbose;
	int t1, t2;

public:
	LidarLayer();
	bool setup(int _port, bool _verbose);
	int recvData();
	~LidarLayer();
};

#endif
