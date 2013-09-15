#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;

int main(int argc, char** argv)
{
	bool die = false;
	Mat frame;
	std::string winName = "ICU";
	namedWindow(winName, WINDOW_KEEPRATIO);
	
	VideoCapture capture(1);
	if(!capture.isOpened())
	{
		std::cout << "ERROR: capture is NULL \n";
		return 1;
	}

	while(!die)
	{
		capture >> frame;
		imshow(winName, frame);
		char c = waitKey(5);
		if(c == 'q') die = 1;
	}

	//Cleanup
	destroyWindow(winName);
	return 0;
}
