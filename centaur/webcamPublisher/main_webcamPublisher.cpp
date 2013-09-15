#include "opencv2/opencv.hpp"
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <signal.h>

using namespace cv;

bool die = false;
bool verbose = false;

void quitproc(int param)
{
	std::cout << "Quitting..." << std::endl;
	die = true;
}

int main(int argc, char** argv)
{
	Mat frame;
	Mat gray;
	std::string winName = "ICU";
	namedWindow(winName, WINDOW_KEEPRATIO);
	
	VideoCapture capture(1);
	if(!capture.isOpened())
	{
		if(verbose) std::cout << "ERROR: capture is NULL \n";
		return 1;
	}

	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);

	while(!die)
	{
		capture >> frame;
		cvtColor(frame, gray, CV_RGB2GRAY);
		imshow(winName, gray);
		char c = waitKey(100);
		if(c == 'q') die = 1;
		if(verbose) std::cout << frame.channels() << " " << frame.depth() << std::endl;
		if(verbose) std::cout << gray.channels() << " " << gray.depth() << std::endl;
	}

	//Cleanup
	destroyWindow(winName);
	return 0;
}
