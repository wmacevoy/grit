#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/include/opencv2/highgui/highgui.hpp"
#include "/usr/include/opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include "SDL/SDL_net.h"
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

	int index = (int)cfg.num("webcam.provider.index", 0);
	int sleep_time = (int)cfg.num("webcam.provider.sleep_time");
	bool detect = cfg.flag("webcam.provider.detect");
	std::string address = cfg.str("webcam.provider.c_ip").c_str();
	int port = (int)cfg.num("webcam.provider.port");

	UDPsocket sd;
	UDPpacket* p;
	IPaddress ip;

	uint8_t areaOfFrame = 1;
	bool connected = false;
	std::string cascadeName;
	Mat frame;
	Mat gray;

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	//Initialize SDL_net
	SDLNet_Init();
		
	while(!(sd = SDLNet_UDP_Open(port)) && !die) {
		printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	while((SDLNet_ResolveHost(&ip, address.c_str(), port) == -1) && !die) {
		fprintf(stderr, "SDLNet_ResolveHost(%s %d): %s\n", address.c_str(), port, SDLNet_GetError());
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	p = SDLNet_AllocPacket(4801);
	p->address.host = ip.host;
	p->address.port = ip.port;
	p->len = 4800 + sizeof(uint8_t);

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

	capture.set(CV_CAP_PROP_FRAME_WIDTH, 160);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 120);
	
	if(verbose) std::cout << "Color: " << frame.channels() << " " << frame.depth() << std::endl;
	if(verbose) std::cout << "Gray: " << gray.channels() << " " << gray.depth() << std::endl;

	int count = 0;
	while(!die)
	{
		capture >> frame;
		gray = frame.clone();
		cvtColor(frame, gray, CV_RGB2GRAY);

		if(detect) {
			detectObjects(gray, gray);
		}

		memcpy(p->data, &areaOfFrame, sizeof(uint8_t));
		
		switch(areaOfFrame) {
		case 1:
			for(int i = 0; i < gray.cols / 2; ++i)
				for(int j = 0; j < gray.rows / 2; ++j)
					p->data[1 + count++] = gray.at<uchar>(Point(i,j));
			break;
		case 2:
			for(int i = gray.cols / 2; i < gray.cols; ++i)
				for(int j = 0; j < gray.rows / 2; ++j)
					p->data[1 + count++] = gray.at<uchar>(Point(i,j));
			break;
		case 3:
			for(int i = 0; i < gray.cols / 2; ++i)
				for(int j = gray.rows / 2; j < gray.rows; ++j)
					p->data[1 + count++] = gray.at<uchar>(Point(i,j));
			break;
		case 4:
			for(int i = gray.cols / 2; i < gray.cols; ++i)
				for(int j = gray.rows / 2; j < gray.rows; ++j)
					p->data[1 + count++] = gray.at<uchar>(Point(i,j));
			break;
		}
		count = 0;

		SDLNet_UDP_Send(sd, -1, p);

		++areaOfFrame;
		if(areaOfFrame > 4 ) {
			areaOfFrame = 1;
		}

		waitKey(sleep_time);
	}

	//Cleanup
	std::cout << "releasing capture and freeing mat memory..." << std::endl;
	capture.release();
	frame.release();
	gray.release();
	std::cout << "--done!" << std::endl;
	std::cout << "closing SDL and freeing packet..." << std::endl;
	SDLNet_FreePacket(p);
	SDLNet_Quit();
	std::cout << "--done!" << std::endl;
	return 0;
}
