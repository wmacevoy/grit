#ifndef LIDARLAYER_H_
#define LIDARLAYER_H_

#include <iostream>
#include <sstream>
#include <iomanip>
#include <zmq.h>
#include <time.h>
#include "opencv2/opencv.hpp"
#include "LidarMessage.h"
#include "fk_lidar.h"

using namespace cv;

const int x_min = 6;
const int x_max = 146;
const int ind_min = 494;
const int ind_max = 588;

class LidarLayer {
	LidarMessage lidarMessage;
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
	bool calibration;
	int t1, t2;

	int lidarLine;	

	Point pt1;
	Point pt2;

	Point tick5l1;
	Point tick5l2;
	Point tick5r1;
	Point tick5r2;

	Point tick10l1;
	Point tick10l2;
	Point tick10r1;
	Point tick10r2;

	Point textOrg;

public:
	LidarLayer();
	bool setup(std::string _address, bool _calibration, bool _verbose);
	int recvData();
	std::string at(int index);
	void writeDistance(Mat& drawable, int mx);
	void drawGraph(Mat& drawable, int width, int height);
	void setBounds(int _width, int _height);
	int getLine();
	~LidarLayer();
};

#endif //LIDARLAYER_H_
