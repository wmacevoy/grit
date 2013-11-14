#include "opencv2/opencv.hpp"
#include <iostream>
#include <signal.h>
#include <assert.h>
#include <sstream>
#include <string>
#include <zmq.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include "Configure.h"

using namespace cv;

Configure cfg;

volatile bool die = false;
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

template<class T>
std::string itoa(const T& t) {
	std::stringstream itoa;
	itoa << t;
	return itoa.str();
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
		if(x >=0 && x <= 320 && y >= 100 && y <= 110)
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
	die = true;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("webcam.requester.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("webcam.requester.verbose", false);
	if (verbose) cfg.show();

	int sleep_time_color = cfg.num("webcam.requester.sleep_time_color");
	int sleep_time_gray = cfg.num("webcam.requester.sleep_time_gray");
	bool calibration = cfg.flag("webcam.requester.calibration", false);
	
	int hwm = 1;
	int linger = 25;
	int rcc = 0;
	int rcl = 0;
	int index = 0;
	int imgNum = 0;
	int sleep_time = sleep_time_gray;
	int t1 = 0, t2 = 0;
	bool CorG  = true;
	Mat color(240, 320, CV_8UC3);
	Mat gray(240, 320, CV_8UC1);

	std::string winName = "ICU";
	std::string text = "0";
	std::string imageName = "";
	namedWindow(winName, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
	
	int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
	double fontScale = 0.75;
	int thickness = 2;

	std::string ip1 = cfg.str("webcam.requester.address", "localhost");
	std::string ip2 = cfg.str("webcam.requester.address", "localhost");
	
	ip1 += ":9993";
	ip2 += ":9997";

	void* context_mat = zmq_ctx_new ();
	void* context_lidar = zmq_ctx_new ();

	void* req_mat = zmq_socket(context_mat, ZMQ_REQ);
	void* sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);

	rcl = zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	assert(rcl == 0);

	rcl = zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0);
	assert(rcl == 0);

	rcl = zmq_setsockopt(req_mat, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	assert(rcl == 0);

	rcl = zmq_setsockopt(req_mat, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	assert(rcl == 0);

	rcl = zmq_setsockopt(req_mat, ZMQ_LINGER, &linger, sizeof(linger));
	assert(rcl == 0);

	rcc = zmq_connect(req_mat, ip1.c_str());
	rcl = zmq_connect(sub_lidar, ip2.c_str());
	assert(rcc == 0 && rcl == 0);	

	lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
	assert(lidar_data != NULL);

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	//Line on screen needs to be calibrated with lidar
	Point pt1(0, 105);
	Point pt2(320, 105);
	Point textOrg(1, 30);

	cvSetMouseCallback(winName.c_str(), mouseEvent, 0);

	zmq_msg_t msg;
	t1 = time(0);
	while(!die)
	{
		rcc = zmq_send(req_mat, &CorG, sizeof(bool), ZMQ_DONTWAIT);
		if (rcc)
		{
			rcc = zmq_msg_init (&msg);
			if(rcc == 0)
			{			
				switch(CorG)
				{
				case false:
					rcc = zmq_recvmsg(req_mat, &msg, ZMQ_DONTWAIT);
					if(rcc == color.total() * color.elemSize())
					{
						t1 = time(0);
						memcpy(color.data, zmq_msg_data(&msg), zmq_msg_size(&msg));

						line(color, pt1, pt2, Scalar(0, 0, 0));
						if(inside)
						{	
							subscribe_lidar(lidar_data, sub_lidar);	
							index = ind_max - ((mx - x_min) * (ind_max - ind_min) / (x_max - x_min));
							//index = 380 + mx;
							text = convstr(lidar_data[index] * 0.00328084);
							putText(color, text, textOrg, fontFace, fontScale, Scalar::all(0), thickness, 8);
							if(calibration) std::cout << "Pixel: " << mx << "   Index: " << index <<
									"  sleep_time: " << sleep_time << std::endl;
						}
						imshow(winName, color);
					}
					break;
				case true:
					rcc = zmq_recvmsg(req_mat, &msg, ZMQ_DONTWAIT);
					if(rcc == gray.total() * gray.elemSize())
					{
						t1 = time(0);
						memcpy(gray.data, zmq_msg_data(&msg), zmq_msg_size(&msg));
						line(gray, pt1, pt2, Scalar(0, 0, 0));
						if(inside)
						{	
							subscribe_lidar(lidar_data, sub_lidar);	
							index = ind_max - ((mx - x_min) * (ind_max - ind_min) / (x_max - x_min));
							//index = 380 + mx;
							text = convstr(lidar_data[index] * 0.00328084);
							putText(gray, text, textOrg, fontFace, fontScale, Scalar::all(0), thickness, 8);
							if(calibration) std::cout << "Pixel: " << mx << "   Index: " << index << 
									"  sleep_time: " << sleep_time << std::endl;
						}
						imshow(winName, gray);
					}
					break;
				}
			}
		}
		zmq_msg_close(&msg);
		char c = waitKey(sleep_time);
		if(c == 't') 
		{
			if(CorG == false)
			{
				CorG = true;
				sleep_time = sleep_time_gray;
			}
			else
			{
				CorG = false;
				sleep_time = sleep_time_color;
			}
		}
		else if(c == 'q') die = true;
		else if(c == 's') {
			imageName = "image";
			imageName += itoa(imgNum++);
			imageName += ".jpg";
			if(CorG == true) {
				imwrite(imageName,  gray);
			}
			else {
				imwrite(imageName,  color);
			}
		}
		t2 = time(0);
		if(t2 - t1 > 5.0) {
			zmq_close(req_mat);
			req_mat = zmq_socket(context_mat, ZMQ_REQ);
			if(zmq_setsockopt(req_mat, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
				if(zmq_setsockopt(req_mat, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0) {
					if(zmq_setsockopt(req_mat, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
						rcc = zmq_connect(req_mat, ip1.c_str());
					}
				}
			}
			if(rcc == 0) {
				std::cout << "Connection successfully reset" << std::endl;
				t1 = time(0);
			}
			else std::cout << "Connection un-successfully reset" << std::endl;
		}
	}

	std::cout << std::endl << "Quitting..." << std::endl;
	std::cout << "destroying window and freeing mat memory..." << std::endl;
	destroyWindow(winName);
	color.release();
	gray.release();
	std::cout << "--done!" << std::endl;
	std::cout << "closing and destroying zmq..." << std::endl;
	zmq_close(req_mat);
	zmq_close(sub_lidar);
	zmq_ctx_destroy(context_mat);
	zmq_ctx_destroy(context_lidar);
	std::cout << "--done!" << std::endl;

	return 0;
}
