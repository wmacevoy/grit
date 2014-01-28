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
  decoded.release();
  my_socket->close();
	free(my_socket);
  std::cout << "--done!" << std::endl;
}

bool RobotWatcher::setup(int port_, bool _hasLidar = true, bool _verbose = false)
{
  port = port_;
  MAX_SIZE = 10000;
  die = false;
  receiving = true;
	inside = false;
	hasLidar = _hasLidar;
	verbose = _verbose;
	mx = 0;
	my = 0;
	currentWidth = normalWidth;
	currentHeight = normalHeight;

  my_socket = new udp::socket(my_io_service, udp::endpoint(udp::v4(), port));

  return true; //get this better
}

bool RobotWatcher::setupLidar(std::string _address, bool _calibration, bool _verbose) {
	d.setup(_address, _calibration, _verbose);
	d.setBounds(currentWidth, currentHeight);
}

Mat RobotWatcher::grab_image()
{
  	buff.resize(MAX_SIZE);
  	size_t length = my_socket->receive_from(boost::asio::buffer(buff, MAX_SIZE), sender_endpoint);
  	buff.resize(length);
  	decoded = imdecode(Mat(buff),CV_LOAD_IMAGE_COLOR);
		if(verbose) std::cout << decoded.cols << "  " << decoded.rows << std::endl;
		d.setBounds(decoded.cols, decoded.rows);
		if(hasLidar) {
			d.recvData();
			d.drawGraph(decoded, decoded.cols, decoded.rows);
			if(inside) {
				d.writeDistance(decoded, mx);			
			}
		}
  	return decoded;
}

void RobotWatcher::run()
{
  namedWindow("Camera", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_NORMAL);

  while(!die) {
    if (receiving) {
      grab_image();
      imshow("Camera",decoded);
      waitKey(20);
    }
  }
  destroyWindow("Camera");
}

void RobotWatcher::kill()
{
  receiving = false;
  die = true;
}

void RobotWatcher::setMouse(int _x, int _y) {
	mx = _x;
	my = _y;
	if(mx >=0 && mx <= currentWidth && my >= d.getLine() - 5 && my <= d.getLine() + 5) {
		inside = true;
	} else {
		inside = false;
	}
}






