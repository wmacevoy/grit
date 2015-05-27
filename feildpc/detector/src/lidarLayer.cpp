#include "lidarLayer.h"

//Helper functions
std::string convstr(const float t) {
	std::stringstream ftoa;
	ftoa << std::setprecision(3) << std::setw(4) << t;
	return ftoa.str();
}
//End helper functions

LidarLayer::LidarLayer() {
	context_lidar = zmq_ctx_new ();
	sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);

	hwm = 1;
	linger = 25;
	index = 0;	
	timeOut = 3.0;

	fontFace = FONT_HERSHEY_SIMPLEX;
	fontScale = 0.35;
	thickness = 1;

	lidarLine = 55;

	t1 = 0;
	t2 = 0;
}

bool LidarLayer::setup(std::string _address, bool _calibration = false, bool _verbose = false) {
	address = _address;
	calibration = _calibration;
	verbose = _verbose;
}

int LidarLayer::recvData() {
	int rc = zmq_recv(sub_lidar, &lidarMessage, sizeof(LidarMessage), ZMQ_DONTWAIT);		
	if(rc > 0) {
		if(verbose) std::cout << "Lidar data received " << rc << " bytes" << std::endl;
		t1 = time(0);
	}
	if (verbose) {
		std::cout << "t=" << lidarMessage.t << " waist=" <<
			lidarMessage.waist << " neckud=" << lidarMessage.neckud << " necklr=" << lidarMessage.necklr << 
			" data[0]=" << lidarMessage.data[0] << std::endl;
	}

	t2 = time(0);
	if(t2 - t1 > timeOut) {
		zmq_close(sub_lidar);
		sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);
	
		if(zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0) == 0) {
			if(zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
				if(zmq_setsockopt(sub_lidar, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
					rc = zmq_connect(sub_lidar, address.c_str());
				}
			}
		}
		if(rc == 0) {
			std::cout << "Connection successfully set/reset" << std::endl;				
			t1 = time(0);
		}
		else std::cout << "Connection un-successfully set/reset" << std::endl;
	}	

	return rc;
}

std::string LidarLayer::at(int index)
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

void LidarLayer::writeDistance(Mat& drawable, int mx) {
	index = ind_max - ((mx - x_min) * (ind_max - ind_min) / (x_max - x_min));
	//index = 480 + mx;
	std::string text = at(index);
	putText(drawable, text, textOrg, fontFace, fontScale, Scalar::all(0), thickness, 8);
	if(calibration) std::cout << "Pixel: " << mx << "   Index: " << index << std::endl;
}

void LidarLayer::drawGraph(Mat& drawable, int width, int height) {
	line(drawable, pt1, pt2, Scalar(50, 50, 50));
	line(drawable, tick5l1, tick5l2, Scalar(0, 0, 0));
	line(drawable, tick5r1, tick5r2, Scalar(0, 0, 0));
	line(drawable, tick10l1, tick10l2, Scalar(0, 0, 0));
	line(drawable, tick10r1, tick10r2, Scalar(0, 0, 0));

	for(int i = 0; i < width; ++i) {
		int ft = lidarMessage.data[ind_max - ((i - x_min) * (ind_max - ind_min) / (x_max - x_min))]/12.0;
		if(ft <= 10) {
			int y = lidarLine - ft;
			if(y <= height && y >= 0) { 							
				line(drawable, Point(i, y), Point(i, y), Scalar(0, 0, 0));
			}
		}
	}				
}

void LidarLayer::setBounds(int _width, int _height) {
	lidarLine = _height * 0.43;	
	pt1.x = 0;
	pt1.y = lidarLine;
	pt2.x = _width;
	pt2.y = lidarLine;

	tick5l1.x = 0;
	tick5l1.y = lidarLine - 5;
	tick5l2.x = 10;
	tick5l2.y = lidarLine - 5;

	tick5r1.x = _width;
	tick5r1.y = lidarLine - 5;
	tick5r2.x = _width - 10;
	tick5r2.y = lidarLine - 5;

	tick10l1.x = 0;
	tick10l1.y = lidarLine - 10;
	tick10l2.x = 5;
	tick10l2.y = lidarLine - 10;

	tick10r1.x = _width;
	tick10r1.y = lidarLine - 10;
	tick10r2.x = _width - 5;
	tick10r2.y = lidarLine - 10;

	textOrg.x = 5;
	textOrg.y = 10;
}

int LidarLayer::getLine() {
	return lidarLine;
}

LidarLayer::~LidarLayer() {
	std::cout << "Quitting Lidar..." << std::endl;
	std::cout << "closing and destroying zmq..." << std::endl;
	zmq_close(sub_lidar);
	zmq_ctx_destroy(context_lidar);
	std::cout << "--done!" << std::endl;
}
