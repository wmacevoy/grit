#include "opencv2/opencv.hpp"
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <signal.h>
#include <assert.h>
#include <zmq.h>
#include "Configure.h"

Configure cfg;

using namespace cv;

bool die = false;
bool verbose = false;

void publish(Mat& gray, void* zmq_pub)
{
	int rc = zmq_send(zmq_pub, gray.data, gray.total() * gray.elemSize(), ZMQ_DONTWAIT);
	if(verbose) std::cout << "Sent: " << rc << std::endl;
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

	int index = cfg.num("webcam.provider.index", 1);

	int hwm = 1;
	int rc = 0;
	Mat frame;
	Mat gray;
	
	VideoCapture capture(index);
	if(!capture.isOpened())
	{
		if(verbose) std::cout << "ERROR: capture is NULL \n";
		return 1;
	}

	//Setup ZMQ
	//tcp://*:9993
	void* context_mat = zmq_ctx_new ();	

	void* pub_mat = zmq_socket(context_mat, ZMQ_PUB);
	rc = zmq_setsockopt(pub_mat, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	assert(rc == 0);

	rc = zmq_bind(pub_mat, "tcp://*:9993");
	assert(rc == 0);

	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);

	while(!die)
	{
		capture >> frame;
		cvtColor(frame, gray, CV_RGB2GRAY);
		gray.reshape(0,1);		
		publish(gray, pub_mat);
		waitKey(200);
		if(verbose) std::cout << frame.channels() << " " << frame.depth() << std::endl;
		if(verbose) std::cout << gray.channels() << " " << gray.depth() << std::endl;
	}

	//Cleanup
	zmq_close(pub_mat);
	zmq_ctx_destroy(context_mat);
	return 0;
}