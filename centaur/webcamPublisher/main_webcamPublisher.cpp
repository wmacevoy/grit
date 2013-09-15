#include "opencv2/opencv.hpp"
#include <iostream>
#include <signal.h>

using namespace cv;

bool die = false;

void quitproc(int param)
{
	std::cout << "Quitting..." << std::endl;
	die = true;
}

int main(int argc, char** argv)
{
	Mat frame;
	std::string winName = "ICU";
	namedWindow(winName, WINDOW_KEEPRATIO);
	
	VideoCapture capture(1);
	if(!capture.isOpened())
	{
		std::cout << "ERROR: capture is NULL \n";
		return 1;
	}

	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);

	while(!die)
	{
		capture >> frame;
		imshow(winName, frame);
		char c = waitKey(100);
		if(c == 'q') die = 1;
		std::cout << frame.size() << std::endl;
	}

	//Cleanup
	destroyWindow(winName);
	return 0;
}
