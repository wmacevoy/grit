#include "robocam.h"
#include <iostream>

using namespace cv;
using namespace boost::asio::ip;

RobotWatcher::RobotWatcher()
{

}

RobotWatcher::~RobotWatcher()
{

}

bool RobotWatcher::setup(int port_)
{
  port = port_;
  MAX_SIZE = 10000;
  die = false;
  receiving = true;

  my_socket = new udp::socket(my_io_service, udp::endpoint(udp::v4(), port));
  namedWindow("Camera", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_NORMAL);

  return true; //get this better
}

Mat RobotWatcher::grab_image()
{
  buff.resize(MAX_SIZE);
  size_t length = my_socket->receive_from(boost::asio::buffer(buff, MAX_SIZE), sender_endpoint);
  buff.resize(length);
  decoded = imdecode(Mat(buff),CV_LOAD_IMAGE_COLOR);
  return decoded;
}

void RobotWatcher::run()
{

  while(!die) {
    if (receiving) {
      grab_image();
      imshow("Camera",decoded);
      waitKey(20);
    }
  }
}

void RobotWatcher::kill()
{
  receiving = false;
  die = true;

  std::cout << "releasing capture and freeing mat memory..." << std::endl;
  decoded.release();
  my_socket->close();
  destroyWindow("Camera");

  std::cout << "--done!" << std::endl;
}







