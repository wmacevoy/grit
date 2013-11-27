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

using namespace cv;

Configure cfg;

volatile bool die = false;
bool inside = false;
bool verbose = false;

LidarMessage lidarMessage;
//int64_t* lidar_data;
//const int sz_lidar_data  = 1081;

volatile int mx = 0;
volatile int my = 0;

const int normalWidth = 320;
const int normalHeight = 240;

const int x_min = 34;
const int x_max = 285;
const int ind_min = 479;
const int ind_max = 605;

const int lidarLine = 105;

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

std::string at(const LidarMessage &lidarMessage, int index)
{
  if (index >= 0 && index < LidarMessage::SIZE) {
    ::Mat3d pose = fk_lidar(lidarMessage.waist,lidarMessage.necklr,lidarMessage.neckud);
    float r = lidarMessage.data[index];
    float theta = (M_PI/180.0)*(270.0/(LidarMessage::SIZE/2))*(index-LidarMessage::SIZE/2);
    ::Vec3d p(r*sin(theta),r*sin(theta),0.0);
    ::Vec3d q=pose*p;
    
    std::ostringstream oss;
    oss << q;
    return oss.str();
  } else {
    return "";
  }
}

void mouseEvent(int evt, int x, int y, int flags, void* param) {
	if(evt == CV_EVENT_MOUSEMOVE) {
		if(x >=0 && x <= normalWidth && y >= 100 && y <= 110) {
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

	int sleep_time_color = cfg.num("webcam.requester.sleep_time_color");
	int sleep_time_gray = cfg.num("webcam.requester.sleep_time_gray");
	bool calibration = cfg.flag("webcam.requester.calibration", false);
	
	int hwm = 1;
	int linger = 25;
	int rcc = 0;
	int rcl = 0;
	int index = 0;
	int imgNum = 0;
	int sleep_time = sleep_time_gray;
	int t1 = 0, t2 = 0;
	bool CorG  = true;
	float timeOut = 3.0;
	Mat color(normalHeight, normalWidth, CV_8UC3);
	Mat gray(normalHeight, normalWidth, CV_8UC1);

	std::string winName = "ICU";
	std::string text = "0";
	std::string imageName = "";
	namedWindow(winName, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_NORMAL);
	
	int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
	double fontScale = 0.60;
	int thickness = 2;

	std::string ip1 = cfg.str("webcam.requester.addressW");
	std::string ip2 = cfg.str("webcam.requester.addressL");

	void* context_mat = zmq_ctx_new ();
	void* context_lidar = zmq_ctx_new ();

	void* req_mat = zmq_socket(context_mat, ZMQ_REQ);
	void* sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);	

	//	lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
	//	assert(lidar_data != NULL);

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
	
	Point textOrg(0, 40);

	cvSetMouseCallback(winName.c_str(), mouseEvent, 0);

	zmq_msg_t msg;
	while(!die) {
		rcc = zmq_send(req_mat, &CorG, sizeof(bool), ZMQ_DONTWAIT);
		if (rcc) {
			rcc = zmq_msg_init (&msg);
			if(rcc == 0) {			
				switch(CorG) {
				case false:
					rcc = zmq_recvmsg(req_mat, &msg, ZMQ_DONTWAIT);
					//					zmq_recv(sub_lidar, lidar_data, sz_lidar_data * sizeof(int64_t), ZMQ_DONTWAIT);	
					zmq_recv(sub_lidar, &lidarMessage, sizeof(LidarMessage), ZMQ_DONTWAIT);	
					if(rcc == color.total() * color.elemSize()) {
						t1 = time(0);
						memcpy(color.data, zmq_msg_data(&msg), zmq_msg_size(&msg));
						line(color, pt1, pt2, Scalar(50, 50, 50));
						line(color, tick5l1, tick5l2, Scalar(0, 0, 0));
						line(color, tick5r1, tick5r2, Scalar(0, 0, 0));
						line(color, tick10l1, tick10l2, Scalar(0, 0, 0));
						line(color, tick10r1, tick10r2, Scalar(0, 0, 0));

						for(int i = 0; i < normalWidth; ++i) {
						  int ft = lidarMessage.data[ind_max - ((i - x_min) * (ind_max - ind_min) / (x_max - x_min))]/12.0;
							if(ft <= 10) {
								int y = lidarLine - ft;
								if(y <= normalHeight && y >= 0) { 							
									line(color, Point(i, y), Point(i, y), Scalar(0, 0, 0));
								}
							}
						}	

						if(inside) {
							index = ind_max - ((mx - x_min) * (ind_max - ind_min) / (x_max - x_min));
							text = at(lidarMessage,index);
							// text = convstr(lidar_data[index] * 0.00328084);
							putText(color, text, textOrg, fontFace, fontScale, Scalar::all(0), thickness, 8);
							if(calibration) std::cout << "Pixel: " << mx << "   Index: " << index <<
									"  sleep_time: " << sleep_time << std::endl;
						}
						imshow(winName, color);
					}
					break;
				case true:
					rcc = zmq_recvmsg(req_mat, &msg, ZMQ_DONTWAIT);
					zmq_recv(sub_lidar, &lidarMessage, sizeof(LidarMessage), ZMQ_DONTWAIT);		
					//					zmq_recv(sub_lidar, lidar_data, sz_lidar_data * sizeof(int64_t), ZMQ_DONTWAIT);		
					if(rcc == gray.total() * gray.elemSize()) {
						t1 = time(0);
						memcpy(gray.data, zmq_msg_data(&msg), zmq_msg_size(&msg));
						line(gray, pt1, pt2, Scalar(50, 50, 50));
						line(gray, tick5l1, tick5l2, Scalar(0, 0, 0));
						line(gray, tick5r1, tick5r2, Scalar(0, 0, 0));
						line(gray, tick10l1, tick10l2, Scalar(0, 0, 0));
						line(gray, tick10r1, tick10r2, Scalar(0, 0, 0));

						for(int i = 0; i < normalWidth; ++i) {
						  //							int ft = lidar_data[ind_max - ((i - x_min) * (ind_max - ind_min) / (x_max - x_min))]  * 0.00328084;
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
							//index = 380 + mx;
							text = at(lidarMessage,index);
							//							text = convstr(lidar_data[index] * 0.00328084);
							putText(gray, text, textOrg, fontFace, fontScale, Scalar::all(0), thickness, 8);
							if(calibration) std::cout << "Pixel: " << mx << "   Index: " << index << 
									"  sleep_time: " << sleep_time << std::endl;
						}
						imshow(winName, gray);
					}
					break;
				}
			}
		}
		zmq_msg_close(&msg);
		char c = waitKey(sleep_time);
		if(c == 't') {
			if(CorG == false) {
				CorG = true;
				sleep_time = sleep_time_gray;
			} else {
				CorG = false;
				sleep_time = sleep_time_color;
			}
		}
		else if(c == 'q') die = true;
		else if(c == 's') {
			imageName = "image";
			imageName += itoa(imgNum++);
			imageName += ".jpg";
			if(CorG == true) {
				imwrite(imageName,  gray);
			}
			else {
				imwrite(imageName,  color);
			}
		}
		t2 = time(0);
		if(t2 - t1 > timeOut) {
			zmq_close(req_mat);
			zmq_close(sub_lidar);
			req_mat = zmq_socket(context_lidar, ZMQ_REQ);
			sub_lidar = zmq_socket(context_mat, ZMQ_SUB);
		
			if(zmq_setsockopt(req_mat, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
				if(zmq_setsockopt(req_mat, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0) {
					if(zmq_setsockopt(req_mat, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
						rcc = zmq_connect(req_mat, ip1.c_str());
					}
				}
			}
			if(zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0) == 0) {
				if(zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
					if(zmq_setsockopt(sub_lidar, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
						rcc = zmq_connect(sub_lidar, ip2.c_str());
					}
				}
			}
			if(rcc == 0) {
				std::cout << "Connection successfully set/reset" << std::endl;				
				t1 = time(0);
			}
			else std::cout << "Connection un-successfully set/reset" << std::endl;
			
		}
	}

	std::cout << std::endl << "Quitting..." << std::endl;
	std::cout << "destroying window and freeing mat memory..." << std::endl;
	destroyWindow(winName);
	color.release();
	gray.release();
	std::cout << "--done!" << std::endl;
	std::cout << "closing and destroying zmq..." << std::endl;
	zmq_close(req_mat);
	zmq_close(sub_lidar);
	zmq_ctx_destroy(context_mat);
	zmq_ctx_destroy(context_lidar);
	std::cout << "--done!" << std::endl;

	return 0;
}
