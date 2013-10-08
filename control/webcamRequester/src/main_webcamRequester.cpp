#include "opencv2/opencv.hpp"
#include <iostream>
#include <signal.h>
#include <assert.h>
#include <string>
#include <zmq.h>
#include <sstream>
#include <iomanip>

#include "Configure.h"

using namespace cv;

Configure cfg;

bool die = false;
bool inside = false;
bool verbose = false;

int64_t* lidar_data;
const int sz_lidar_data  = 1081;

volatile int mx = 0;
volatile int my = 0;

const int x_min = 34;
const int x_max = 285;
const int ind_min = 479;
const int ind_max = 605;

std::string convstr(const float t)
{
	std::stringstream ftoa;
	ftoa << std::setprecision(3) << std::setw(4) << t;
	return ftoa.str();
}

void subscribe_cam(Mat& mat, void* zmq_sub)
{
	int rc = zmq_recv(zmq_sub, mat.data, mat.total() * mat.elemSize(), ZMQ_DONTWAIT);
	if(verbose) std::cout << "Received: " << rc << std::endl;
}

void subscribe_lidar(int64_t* data, void* zmq_sub)
{
	if(verbose) printf("waiting for lidar data...\n");
	int rc = zmq_recv(zmq_sub, lidar_data, sz_lidar_data * sizeof(int64_t), ZMQ_DONTWAIT);
	if(verbose && rc > 0) printf("received lidar data!\n");
}

void mouseEvent(int evt, int x, int y, int flags, void* param)
{
	if(evt == CV_EVENT_MOUSEMOVE)
	{
		if(x >=0 && x <= 320 && y >= 45 && y <= 65)
		{
			mx = x;
			my = y;
			inside = true;
		}
		else
			inside = false;
	}
}

void quitproc(int param)
{
	std::cout << "Quitting..." << std::endl;
	die = true;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("webcam.requester.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("webcam.requester.verbose", false);
	if (verbose) cfg.show();

	int sleep_time_color = cfg.num("webcam.requester.sleep_time_color", 450);
	int sleep_time_gray = cfg.num("webcam.requester.sleep_time_gray", 100);
	bool calibration = cfg.flag("webcam.requester.calibration", false);
	
	int hwm = 1;
	int rcc = 0;
	int rcg = 0;
	int rcl = 0;
	int index = 0;
	int sleep_time = sleep_time_gray;
	bool CorG  = false;
	Mat color(240, 320, CV_8UC3);
	Mat gray(240, 320, CV_8UC1);

	std::string winName = "ICU";
	std::string text = "0";
	namedWindow(winName, CV_WINDOW_NORMAL);
	int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
	double fontScale = 0.75;
	int thickness = 2;
	std::string ip1 = "tcp://";
	std::string ip2 = "tcp://";
	std::string ip3 = "tcp://";

	ip1 += cfg.str("webcam.requester.address", "localhost");
	ip2 += cfg.str("webcam.requester.address", "localhost");
	ip3 += cfg.str("webcam.requester.address", "localhost");
	
	ip1 += ":9993";
	ip2 += ":9997";
	ip3 += ":9994";

	void* context_color = zmq_ctx_new ();
	void* context_gray = zmq_ctx_new ();
	void* context_lidar = zmq_ctx_new ();

	void* sub_color = zmq_socket(context_color, ZMQ_SUB);
	void* sub_gray = zmq_socket(context_gray, ZMQ_SUB);
	void* sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);

	rcc = zmq_setsockopt(sub_color, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	rcg = zmq_setsockopt(sub_gray, ZMQ_RCVHWM, &hwm, sizeof(hwm)); 
	rcl = zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	assert(rcc == 0 && rcg == 0 && rcl == 0);

	rcc = zmq_setsockopt(sub_color, ZMQ_SUBSCRIBE, "", 0);
	rcg = zmq_setsockopt(sub_gray, ZMQ_SUBSCRIBE, "", 0);
	rcl = zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0);
	assert(rcc == 0 && rcg == 0 && rcl == 0);

	rcc = zmq_connect(sub_color, ip1.c_str());
	rcg = zmq_connect(sub_gray, ip3.c_str());
	rcl = zmq_connect(sub_lidar, ip2.c_str());
	assert(rcc == 0 && rcg == 0 && rcl == 0);	

	lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
	assert(lidar_data != NULL);

	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);

	//Line on screen needs to be calibrated with lidar
	Point pt1(0, 55);
	Point pt2(320, 55);
	Point textOrg(1, 30);

	cvSetMouseCallback(winName.c_str(), mouseEvent, 0);

	while(!die)
	{
		switch(CorG)
		{
		case true:
			subscribe_cam(color, sub_color);
			line(color, pt1, pt2, Scalar(0, 0, 0));
			if(inside)
			{	
				subscribe_lidar(lidar_data, sub_lidar);	
				index = ind_max - ((mx - x_min) * (ind_max - ind_min) / (x_max - x_min));
				//index = 380 + mx;
				text = convstr(lidar_data[index] * 0.00328084);
				putText(gray, text, textOrg, fontFace, fontScale, Scalar::all(0), thickness, 8);
				if(calibration) std::cout << "Pixel: " << mx << "   Index: " << index << " " << sleep_time << std::endl;
			}
			imshow(winName, color);
			break;
		case false:
			subscribe_cam(gray, sub_gray);
			line(gray, pt1, pt2, Scalar(0, 0, 0));
			if(inside)
			{	
				subscribe_lidar(lidar_data, sub_lidar);	
				index = ind_max - ((mx - x_min) * (ind_max - ind_min) / (x_max - x_min));
				//index = 380 + mx;
				text = convstr(lidar_data[index] * 0.00328084);
				putText(gray, text, textOrg, fontFace, fontScale, Scalar::all(0), thickness, 8);
				if(calibration) std::cout << "Pixel: " << mx << "   Index: " << index << " " << sleep_time << std::endl;
			}
			imshow(winName, gray);
			break;
		}
		char c = waitKey(sleep_time);
		if(c == 't') 
		{
			if(CorG == false)
			{
				CorG = true;
				sleep_time = sleep_time_color;
			}
			else
			{
				CorG = false;
				sleep_time = sleep_time_gray;
			}
		}
		else if(c == 'q') die = true;
	}

	destroyWindow(winName);
	zmq_close(sub_color);
	zmq_close(sub_gray);
	zmq_close(sub_lidar);
	zmq_ctx_destroy(context_color);
	zmq_ctx_destroy(context_gray);
	zmq_ctx_destroy(context_lidar);

	return 0;
}
