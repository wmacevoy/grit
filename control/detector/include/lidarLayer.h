#ifndef LIDARLAYER_H_
#define LIDARLAYER_H_

#include <iostream>
#include <sstream>
#include <iomanip>
#include <zmq.h>
#include <time.h>
#include "opencv2/opencv.hpp"

using namespace cv;


class LidarLayer {
	void* context_lidar;
	void* sub_lidar;
	std::string address;
	
	int hwm;
	int linger;
	int index;
	float timeOut;
	
	int fontFace;
	double fontScale;
	int thickness;

	bool verbose;
	int t1, t2;

public:
	LidarLayer();
	bool setup(std::string _address, bool _verbose);
	int recvData();
	~LidarLayer();
};

#endif //LIDARLAYER_H_
