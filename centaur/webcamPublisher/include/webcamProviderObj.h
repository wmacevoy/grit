#ifndef _webcamObject_H_
#define _webcamObject_H_

#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/include/opencv2/highgui/highgui.hpp"
#include "/usr/include/opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "boost/filesystem.hpp"
#include <boost/asio.hpp>

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <atomic>
#include "now.h"

using namespace cv;
using namespace std;
using namespace std::chrono;
namespace fs = boost::filesystem;

typedef std::chrono::duration<int,std::milli> millisecs_t;

const int IMAGE_QUALITY= 40;
const int DEFAULT_WIDTH = 320;
const int DEFAULT_HEIGHT = 240;

class webcamProvider {
	std::atomic<bool> die;
	bool verbose;
	int indexR, indexL;
	int sleep_time; 
	std::string addressf;
	std::string addressc;
	std::string port;
	std::string port2;
	std::string path;

	//Boost items
	boost::asio::io_service io_service;
	boost::asio::ip::udp::resolver resolver;
	boost::asio::ip::udp::resolver::query *query;
	boost::asio::ip::udp::resolver::query *query2;
	boost::asio::ip::udp::resolver::query *query3;
	boost::asio::ip::udp::endpoint receiver_endpoint;
	boost::asio::ip::udp::endpoint receiver_endpoint2;
	boost::asio::ip::udp::endpoint receiver_endpoint3;
	boost::asio::ip::udp::socket *socket;
	boost::asio::ip::udp::socket *socket2;
	boost::asio::ip::udp::socket *socket3;
	bool connected;

	//Image items
	VideoCapture captureR;
	VideoCapture captureL;
	int width, height;
	int image_quality;
	int lowsend;
	Mat frameR, frameL;
	Mat gray;	
	std::vector<uchar> buff;
	std::vector<int> param;
	std::string output_type;

public:
	webcamProvider(int, int, int, int, bool, const char*, std::string, std::string, std::string, std::string);
	bool init();
	void provide();
	int getwidth();
	int getheight();
	bool setResolution(int _width, int _height);
	bool setQuality(int _quality);
	bool setFramerate(int _framerate);
	bool kill();
	~webcamProvider();
};


#endif //webcamObject_H_
