#include "robocam.h"
#include <iostream>

using namespace cv;
using namespace boost::asio::ip;

RobotWatcher::RobotWatcher()
{

}

RobotWatcher::~RobotWatcher()
{
	std::cout << "Quitting Robowatcher..." << std::endl;
	std::cout << "releasing capture and freeing mat memory..." << std::endl;
    decoded.second.release();
    my_socket->close();
	free(my_socket);
    std::cout << "--done!" << std::endl;
}

bool RobotWatcher::setup(int port_, bool _verbose = false)
{
    port = port_;
    MAX_SIZE = 50000;
    die = false;
    receiving = true;
	inside = false;
	//hasLidar = _hasLidar;
	verbose = _verbose;
	currentWidth = normalWidth;
	currentHeight = normalHeight;

  my_socket = new udp::socket(my_io_service, udp::endpoint(udp::v4(), port));
  my_socket->non_blocking(true);

  return true; //get this better
}

int RobotWatcher::getWidth(){
	return currentWidth;
}
int RobotWatcher::getHeight(){
	return currentHeight;
}

std::pair<char, cv::Mat> RobotWatcher::grab_image()
{
    boost::system::error_code ec;

  	buff.resize(MAX_SIZE);
  	size_t length = my_socket->receive_from(boost::asio::buffer(buff, MAX_SIZE), sender_endpoint, 0, ec);
   // if(verbose) std::cout << "Robocam recv length = " << length << std::endl;
  	
    if(length > 0)
     {
		 char lr = buff[0];
		// if(verbose) std::cout << "left or right: " << lr << std::endl;
		 decoded.first = lr;
		 buff.erase(buff.begin());
		 
		 buff.resize(length);
  	     decoded.second = imdecode(Mat(buff),CV_LOAD_IMAGE_COLOR);
		 //if(verbose) std::cout << "width: " << decoded.second.cols << ", height: " << decoded.second.rows << std::endl;
		 //d.setBounds(decoded.cols, decoded.rows);
		/*if(hasLidar) {
			 d.recvData();
			 d.drawGraph(decoded, decoded.cols, decoded.rows);
			 if(inside) {
				 d.writeDistance(decoded, mx);			
			 }
		  } */
     }
  	return decoded;
}

void RobotWatcher::kill()
{
  receiving = false;
  die = true;
}






