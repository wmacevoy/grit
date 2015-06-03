#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include "Configure.h"
#include "robocam.h"

using namespace cv;

Configure cfg;

RobotWatcher *ghost;

volatile bool die = false;
bool inside = false;
bool verbose = false;

template<class T>
std::string itoa(const T& t) {
	std::stringstream itoa;
	itoa << t;
	return itoa.str();
}

void quitproc(int param) {
	die = true;
	ghost->kill();
}

void mouseEvent(int evt, int x, int y, int flags, void* param) {
	/*if(evt == CV_EVENT_MOUSEMOVE) {
		ghost->setMouse(x, y);
	}*/
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("webcam.requester.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("webcam.requester.verbose", false);
	if (verbose) cfg.show();

	int sleep_time_gray = cfg.num("webcam.requester.sleep_time");
	int port = (int)cfg.num("webcam.provider.port");
	//std::string lidarAddress = cfg.str("lidar.provider.subscribe");
	//bool lidarCalibration = cfg.flag("detector.webcam.requester.calibration", false);


	RobotWatcher my_watcher;
	ghost = &my_watcher;
	
	int imgNum = 0;
	int sleep_time = sleep_time_gray;
	bool receiving = true;
	Mat frame;

	std::string windowName = "ICU (HIGH - Right Eye)";
	std::string imageName = "";
	namedWindow(windowName, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_NORMAL);

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);
	
	//Initialize watcher
	my_watcher.setup(port, true, verbose);
	//my_watcher.setupLidar(lidarAddress, lidarCalibration, verbose);

	//cvSetMouseCallback(windowName.c_str(), mouseEvent, 0);

	while(!die) {
		if(receiving) {
        try
          {
		  frame = my_watcher.grab_image();
          }
        catch(int e)
          {
          std::cout << "No data available!" << std::endl;
          }

        if(!frame.empty())
         {
		 imshow(windowName, frame);
         }
		}

		char c = waitKey(sleep_time);
		if(c == 'q') die = true;
		else if(c == 's') {
			imageName = "image";
			imageName += itoa(imgNum++);
			imageName += ".jpg";
			imwrite(imageName,  frame);
		}
		else if(c == 'p') {
			receiving = !receiving;
		}
		else if(c =='u') {
			if(sleep_time < 10000) sleep_time += 100;
		}
		else if(c =='t') {
			if(sleep_time > 100)  sleep_time -= 100;
		}
	}

	std::cout << std::endl << "Quitting..." << std::endl;
	std::cout << "destroying window and freeing mat memory..." << std::endl;
	destroyWindow(windowName);
	frame.release();
	std::cout << "--done!" << std::endl;
	return 0;
}
