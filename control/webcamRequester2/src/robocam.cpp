#include "robocam.h"
#include <iostream>

using namespace cv;
using namespace boost::asio::ip;

cv::Mat rotate(cv::Mat& src, double angle){
	cv::Mat dst;
	cv::Point2f src_center(src.cols/2.0f,src.rows/2.0f);
	cv::Mat rot_mat = cv::getRotationMatrix2D(src_center,angle,1.0);
	cv::warpAffine(src, dst, rot_mat, src.size());
	return dst;
}

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
  my_socket->non_blocking(true);
  
  boost::asio::socket_base::receive_buffer_size option(8192);
  my_socket->set_option(option);

  return true; //get this better
}
/*
bool RobotWatcher::setupLidar(std::string _address, bool _calibration, bool _verbose) {
	if(hasLidar) {
		d.setup(_address, _calibration, _verbose);
		d.setBounds(currentWidth, currentHeight);
	}
}
*/

Mat RobotWatcher::grab_image()
{
    boost::system::error_code ec;

  	buff.resize(MAX_SIZE);
  	size_t length = my_socket->receive_from(boost::asio::buffer(buff, MAX_SIZE), sender_endpoint, 0, ec);
    std::cout << "lenght = " << length << std::endl;
  	
    if(length > 0)
     {
		 buff.resize(length);
  	     decoded = imdecode(Mat(buff),CV_LOAD_IMAGE_COLOR);
		 if(verbose) std::cout << decoded.cols << "  " << decoded.rows << std::endl;
	     decoded = rotate(decoded, 90);
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

/*
void RobotWatcher::setMouse(int _x, int _y) {
	mx = _x;
	my = _y;
	if(mx >=0 && mx <= currentWidth && my >= d.getLine() - 5 && my <= d.getLine() + 5) {
		inside = true;
	} else {
		inside = false;
	}
}
*/





