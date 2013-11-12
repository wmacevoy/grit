#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/include/opencv2/highgui/highgui.hpp"
#include "/usr/include/opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <signal.h>
#include <assert.h>
#include <zmq.h>
#include "Configure.h"

Configure cfg;

using namespace cv;

volatile bool die = false;
bool verbose = false;
CascadeClassifier cc;

void detectObjects(Mat& frame, const Mat& gray_img) {
	std::vector<Rect> objects;	
	Mat temp;

	temp = gray_img.clone();
	
	equalizeHist( temp, temp );
	
	cc.detectMultiScale( temp, objects, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(25, 25) );

	for( size_t i = 0; i < objects.size(); i++ ) {
		Point center( objects[i].x + objects[i].width*0.5, objects[i].y + objects[i].height*0.5 );
		ellipse( frame, center, Size( objects[i].width*0.5, objects[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
	}
}

void publish_mat(Mat& mat, void* zmq_pub)
{
	zmq_msg_t msg;
	int rc = zmq_msg_init_size(&msg, mat.total() * mat.elemSize());
	memcpy(zmq_msg_data(&msg), mat.data, mat.total() * mat.elemSize());
	if(rc == 0)
	{
		int rc = zmq_sendmsg(zmq_pub, &msg, ZMQ_DONTWAIT);
	}

	if(verbose) std::cout << "Sent: " << rc << std::endl;

	zmq_msg_close(&msg);
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

	int index = cfg.num("webcam.provider.index", 0);
	int sleep_time = cfg.num("webcam.provider.sleep_time", 50);

	bool detect = cfg.flag("webcam.provider.detect");

	int linger = 25;
	int rc = 0;
	bool CorG = false;
	std::string cascadeName;
	Mat frame;
	Mat gray;

	if(detect) {
		cascadeName = cfg.str("webcam.provider.cascade");
		if(!cc.load(cascadeName)) {
			std::cout << "Could not load cascade. Object detection mode set to false..." << std::endl;
			detect = false;		
		}
		else {
			std::cout << "Loaded cascade:" << cascadeName << std::endl;
		}
	}
	
	VideoCapture capture(index);
	if(!capture.isOpened())
	{
		std::cout << "ERROR: capture is NULL \n";
		capture.release();
		return 1;
	}

	capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

	//Setup ZMQ
	//tcp://*:9993
	void* context_mat = zmq_ctx_new ();

	void* rep_mat = zmq_socket(context_mat, ZMQ_REP);
	
	rc = zmq_setsockopt(rep_mat, ZMQ_LINGER, &linger, sizeof(linger));
	assert(rc == 0);

	rc = zmq_bind(rep_mat, "tcp://*:9993");
	assert(rc == 0);

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	
	if(verbose) std::cout << "Color: " << frame.channels() << " " << frame.depth() << std::endl;
	if(verbose) std::cout << "Gray: " << gray.channels() << " " << gray.depth() << std::endl;

	while(!die)
	{
		capture >> frame;
		cvtColor(frame, gray, CV_RGB2GRAY);

		if(detect) {
			if(CorG == 0) {
				detectObjects(frame, gray);			
			}
			else {
				detectObjects(gray, gray);
			}
		}

		frame.reshape(0,1);
		gray.reshape(0,1);

		int rv = zmq_recv(rep_mat, &CorG, sizeof(bool), ZMQ_DONTWAIT);
		switch(CorG)
		{
		case 0:		
			publish_mat(frame, rep_mat);
			break;

		case 1:
			publish_mat(gray, rep_mat);
			break;
		}

		waitKey(sleep_time);
	}

	//Cleanup
	
	std::cout << "releasing capture and freeing mat memory..." << std::endl;
	capture.release();
	frame.release();
	gray.release();
	std::cout << "--done!" << std::endl;
	std::cout << "closing and destroying zmq..." << std::endl;
	zmq_close(rep_mat);
	zmq_ctx_destroy(context_mat);
	std::cout << "--done!" << std::endl;
	return 0;
}
