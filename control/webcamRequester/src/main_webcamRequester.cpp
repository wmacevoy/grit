#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <sstream>
#include <string>
#include <zmq.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include "Configure.h"
#include "LidarMessage.h"
#include "fk_lidar.h"
#include "SDL/SDL_net.h"

using namespace cv;

Configure cfg;

volatile bool die = false;
bool inside = false;
bool verbose = false;

LidarMessage lidarMessage;

volatile int mx = 0;
volatile int my = 0;

const int normalWidth = 160;
const int normalHeight = 120;
const int recvSize = 80 * 60;

const int x_min = 32;
const int x_max = 50;
const int ind_min = 517;
const int ind_max = 551;

const int lidarLine = 105 * 0.43;

//////////SDL
UDPsocket sd;
UDPpacket* p;

std::string convstr(const float t) {
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

std::string at(int index)
{
  if (index >= 0 && index < LidarMessage::SIZE) {
    ::Mat3d pose = fk_lidar(lidarMessage.waist,lidarMessage.necklr,lidarMessage.neckud);
    float r = lidarMessage.data[index];
    float theta = -(270.0/(LidarMessage::SIZE))*(index-LidarMessage::SIZE/2);
    ::Vec3d p(r*sin((M_PI/180.0)*theta),r*cos((M_PI/180.0)*theta),0.0);
    ::Vec3d q=pose*p;

    std::ostringstream oss;
    oss << std::setprecision(2) << std::fixed << r/12.0 << "ft " << " " << std::setprecision(1) << q << "in";
    return oss.str();
  } else {
    return "";
  }
}

void mouseEvent(int evt, int x, int y, int flags, void* param) {
	if(evt == CV_EVENT_MOUSEMOVE) {
		if(x >=0 && x <= normalWidth && y >= lidarLine - 5 && y <= lidarLine + 5) {
			mx = x;
			my = y;
			inside = true;
		} else {
			inside = false;
		}
	}
}

void quitproc(int param) {
	die = true;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("webcam.requester.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("webcam.requester.verbose", false);
	if (verbose) cfg.show();

	int sleep_time_gray = cfg.num("webcam.requester.sleep_time_gray");
	bool calibration = cfg.flag("webcam.requester.calibration", false);
	
	int hwm = 10;
	int linger = 25;
	int rc = 0;
	int index = 0;
	int imgNum = 0;
	int sleep_time = sleep_time_gray;
	int t1 = 0, t2 = 0;
	bool receiving = true;
	uint8_t areaOfFrame = 1;

	float timeOut = 3.0;
	Mat gray(normalHeight, normalWidth, CV_8UC1);
	gray.reshape(0,1);

	std::string winName = "ICU";
	std::string text = "0";
	std::string imageName = "";
	namedWindow(winName, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_NORMAL);
	
	int fontFace =FONT_HERSHEY_SIMPLEX;
	double fontScale = 0.35;
	int thickness = 1;

	int port = (int)cfg.num("webcam.provider.port");
	std::string ip2 = cfg.str("lidar.provider.subscribe");
	
	//Initialize SDL_net
	SDLNet_Init();
	sd = SDLNet_UDP_Open(port);
	p = SDLNet_AllocPacket(recvSize + sizeof(uint8_t));
	if(!sd) {
    printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
  }

	void* context_lidar = zmq_ctx_new ();
	void* sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);	

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	Point pt1(0, lidarLine);
	Point pt2(normalWidth, lidarLine);
	
	Point tick5l1(0, lidarLine - 5);
	Point tick5l2(10, lidarLine - 5);
	Point tick5r1(normalWidth, lidarLine - 5);
	Point tick5r2(normalWidth - 10, lidarLine - 5);
	
	
	Point tick10l1(0, lidarLine - 10);
	Point tick10l2(5, lidarLine - 10);
	Point tick10r1(normalWidth, lidarLine - 10);
	Point tick10r2(normalWidth - 5, lidarLine - 10);	
	
	Point textOrg(0, 10);

	cvSetMouseCallback(winName.c_str(), mouseEvent, 0);

	int count = 0;
	while(!die) {
		if(receiving) {
			rc = SDLNet_UDP_Recv(sd, p);
			if(p->len == recvSize + sizeof(uint8_t)) {
				zmq_recv(sub_lidar, &lidarMessage, sizeof(LidarMessage), ZMQ_DONTWAIT);		
				if (verbose) {
					std::cout << "t=" << lidarMessage.t << " waist=" <<
						lidarMessage.waist << " neckud=" << lidarMessage.neckud << " necklr=" << lidarMessage.necklr << 
						
						" data[0]=" << lidarMessage.data[0] << std::endl;
				}	

				t1 = time(0);
				memcpy(&areaOfFrame, p->data, sizeof(uint8_t));

				switch(areaOfFrame) {
				case 1:
					for(int i = 0; i < gray.cols / 2; ++i)
						for(int j = 0; j < gray.rows / 2; ++j)
							gray.at<uchar>(Point(i,j)) = p->data[1 + count++];			
					break;
				case 2:
					for(int i = gray.cols / 2; i < gray.cols; ++i)
						for(int j = 0; j < gray.rows / 2; ++j)
							gray.at<uchar>(Point(i,j)) = p->data[1 + count++];
					break;
				case 3:
					for(int i = 0; i < gray.cols / 2; ++i)
						for(int j = gray.rows / 2; j < gray.rows; ++j)
							gray.at<uchar>(Point(i,j)) = p->data[1 + count++];
					break;
				case 4:
					for(int i = gray.cols / 2; i < gray.cols; ++i)
						for(int j = gray.rows / 2; j < gray.rows; ++j)
							gray.at<uchar>(Point(i,j)) = p->data[1 + count++];
					break;
				}
				count = 0;

				line(gray, pt1, pt2, Scalar(50, 50, 50));
				line(gray, tick5l1, tick5l2, Scalar(0, 0, 0));
				line(gray, tick5r1, tick5r2, Scalar(0, 0, 0));
				line(gray, tick10l1, tick10l2, Scalar(0, 0, 0));
				line(gray, tick10r1, tick10r2, Scalar(0, 0, 0));

				for(int i = 0; i < normalWidth; ++i) {
					int ft = lidarMessage.data[ind_max - ((i - x_min) * (ind_max - ind_min) / (x_max - x_min))]/12.0;
					if(ft <= 10) {
						int y = lidarLine - ft;
						if(y <= normalHeight && y >= 0) { 							
							line(gray, Point(i, y), Point(i, y), Scalar(0, 0, 0));
						}
					}
				}				

				if(inside) {	
					index = ind_max - ((mx - x_min) * (ind_max - ind_min) / (x_max - x_min));
					//index = 500 + mx;
					text = at(index);
					putText(gray, text, textOrg, fontFace, fontScale, Scalar::all(0), thickness, 8);
					if(calibration) std::cout << "Pixel: " << mx << "   Index: " << index << 
							"  sleep_time: " << sleep_time << std::endl;
				}
				imshow(winName, gray);
			}
		}

		char c = waitKey(sleep_time);
		if(c == 'q') die = true;
		else if(c == 's') {
			imageName = "image";
			imageName += itoa(imgNum++);
			imageName += ".jpg";
			imwrite(imageName,  gray);
		}
		else if(c == 'p') {
			receiving = !receiving;
		}

		t2 = time(0);
		if(t2 - t1 > timeOut) {
			zmq_close(sub_lidar);
			sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);
		
			if(zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0) == 0) {
				if(zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
					if(zmq_setsockopt(sub_lidar, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
						rc = zmq_connect(sub_lidar, ip2.c_str());
					}
				}
			}
			if(rc == 0) {
				std::cout << "Connection successfully set/reset" << std::endl;				
				t1 = time(0);
			}
			else std::cout << "Connection un-successfully set/reset" << std::endl;
		}	
	}

	std::cout << std::endl << "Quitting..." << std::endl;
	std::cout << "destroying window and freeing mat memory..." << std::endl;
	destroyWindow(winName);
	gray.release();
	std::cout << "--done!" << std::endl;
	std::cout << "closing and destroying zmq..." << std::endl;
	zmq_close(sub_lidar);
	zmq_ctx_destroy(context_lidar);
	std::cout << "--done!" << std::endl;
	std::cout << "closing SDL and freeing packet..." << std::endl;
	SDLNet_FreePacket(p);
	SDLNet_Quit();
	std::cout << "--done!" << std::endl;
	return 0;
}
