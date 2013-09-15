#include "opencv2/opencv.hpp"
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <signal.h>
#include <assert.h>
#include <string>
#include <zmq.h>

//#include "Configure.h"

using namespace cv;

bool die = false;
bool verbose = false;

const int sz_mat = 640*480*1;

void subscribe(Mat& mat, void* zmq_sub)
{
	int rc = zmq_recv(zmq_sub, mat.data, sz_mat, ZMQ_DONTWAIT);
	if(verbose) std::cout << "Received: " << rc << std::endl;
}

void quitproc(int param)
{
	std::cout << "Quitting..." << std::endl;
	die = true;
}

int main(int argc, char** argv)
{
	//cfg.path("../../setup");
	//cfg.args("webcam.provider.", argv);
	//if (argc == 1) cfg.load("config.csv");
	//verbose = cfg.flag("webcam.provider.verbose", false);
	//if (verbose) cfg.show();
	
	int hwm = 1;
	int rc = 0;
	Mat gray(480, 640, 0);
	std::string winName = "ICU";
	namedWindow(winName, WINDOW_KEEPRATIO);
	std::string ip = "tcp://";

	void* context_mat = zmq_ctx_new ();
	
	void* sub_mat = zmq_socket(context_mat, ZMQ_PUB);
	rc = zmq_setsockopt(sub_mat, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	assert(rc == 0);

	sub_mat = zmq_socket(context_mat, ZMQ_SUB);

	rc = zmq_setsockopt(sub_mat, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	assert (rc == 0);

	rc = zmq_setsockopt(sub_mat, ZMQ_SUBSCRIBE, "", 0);
	assert (rc == 0);

	rc = zmq_connect(sub_mat, "tcp://localhost:9993");
	assert(rc == 0);	

	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);

	std::cout << gray.total() << std::endl;

	while(!die)
	{
		subscribe(gray, sub_mat);
		imshow(winName, gray);
		char c = waitKey(100);
		if(c == 'q') die = true;
	}

	destroyWindow(winName);
	zmq_close(sub_mat);
	zmq_ctx_destroy(context_mat);
	return 0;
}
