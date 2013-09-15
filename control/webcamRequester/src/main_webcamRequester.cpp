#include "opencv2/opencv.hpp"
#include "cv.h"
#include "highgui.h"
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

const int sz_mat = 640*480*1;

int64_t* lidar_data;



const int sz_lidar_data  = 1081;

volatile int mx = 0;
volatile int my = 0;

std::string convstr(const float t)
{
	std::stringstream ftoa;
	ftoa << std::setprecision(3) << std::setw(4) << t;
	return ftoa.str();
}

void subscribe(Mat& mat, void* zmq_sub)
{
	int rc = zmq_recv(zmq_sub, mat.data, sz_mat, ZMQ_DONTWAIT);
	if(verbose) std::cout << "Received: " << rc << std::endl;
}

void subscribe_lidar(void* zmq_sub)
{
	if(verbose) printf("waiting for lidar data...\n");
	int rc = zmq_recv(zmq_sub, lidar_data, sz_lidar_data * sizeof(int64_t), ZMQ_DONTWAIT);
	if(verbose && rc > 0) printf("received lidar data!\n");
}

void mouseEvent(int evt, int x, int y, int flags, void* param)
{
	if(evt == CV_EVENT_MOUSEMOVE)
	{
		if(x >=0 && x <= 640 && y >= 195 && y <= 205)
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
	cfg.args("webcam.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("webcam.provider.verbose", false);
	if (verbose) cfg.show();
	
	int hwm = 1;
	int rcm = 0;
	int rcl = 0;
	int index = 0;
	Mat gray(480, 640, 0);
	std::string winName = "ICU";
	std::string text = "0";
	namedWindow(winName, CV_WINDOW_AUTOSIZE);
	int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
	double fontScale = 0.75;
	int thickness = 2;
	std::string ip1 = "tcp://";
	std::string ip2 = "tcp://";

	ip1 += cfg.str("webcam.requester.address", "localhost");
	ip2 += cfg.str("webcam.requester.address", "localhost");
	
	ip1 += ":9993";
	ip2 += ":9997";

	void* context_mat = zmq_ctx_new ();
	void* context_lidar = zmq_ctx_new ();

	void* sub_mat = zmq_socket(context_mat, ZMQ_SUB);
	void* sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);

	rcm = zmq_setsockopt(sub_mat, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	rcl = zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	assert(rcm == 0 && rcl == 0);

	rcm = zmq_setsockopt(sub_mat, ZMQ_SUBSCRIBE, "", 0);
	rcl = zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0);
	assert(rcm == 0 && rcl == 0);

	rcm = zmq_connect(sub_mat, ip1.c_str());
	rcl = zmq_connect(sub_lidar, ip2.c_str());
	assert(rcm == 0 && rcl == 0);	

	lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
	assert(lidar_data != NULL);

	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);

	//Line on screen needs to be calibrated with lidar
	Point pt1(0, 200);
	Point pt2(640, 200);
	Point textOrg(1, 30);

	cvSetMouseCallback(winName.c_str(), mouseEvent, 0);

	while(!die)
	{
		subscribe(gray, sub_mat);
		line(gray, pt1, pt2, Scalar(0, 0, 0)); //Needs to be calibrated with lidar
		if(inside)
		{		
			text = "X: " + convstr(mx) + " Y: " + convstr(my);
			putText(gray, text, textOrg, fontFace, fontScale, Scalar::all(0), thickness, 8);
		}
		imshow(winName, gray);
		char c = waitKey(200);
		if(c == 'q') die = true;
	}

	destroyWindow(winName);
	zmq_close(sub_mat);
	zmq_ctx_destroy(context_mat);
	return 0;
}
