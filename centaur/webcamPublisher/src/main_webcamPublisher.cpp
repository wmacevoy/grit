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
	std::cout << "\nQuitting..." << std::endl;
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
	int sleep_time = cfg.num("webcam.provider.sleep_time", 200);

	int hwm = 1;
	int rc = 0;
	Mat frame;
	Mat gray;
	
	VideoCapture capture(index);
	if(!capture.isOpened())
	{
		std::cout << "ERROR: capture is NULL \n";
		return 1;
	}

	capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

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

	
	if(verbose) std::cout << frame.channels() << " " << frame.depth() << std::endl;
	//if(verbose) std::cout << gray.channels() << " " << gray.depth() << std::endl;

	while(!die)
	{
		capture >> frame;
		std::cout << frame.size() << std::endl;
		//cvtColor(frame, gray, CV_RGB2GRAY);
		frame.reshape(0,1);		
		publish(frame, pub_mat);
		waitKey(sleep_time);
	}

	//Cleanup
	zmq_close(pub_mat);
	zmq_ctx_destroy(context_mat);
	return 0;
}
