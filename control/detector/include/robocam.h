//Robocam Receiver
//1-26-14
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include "lidarLayer.h"

const int normalWidth = 320;//const int normalWidth = 256;
const int normalHeight = 240;//const int normalHeight = 144;

class RobotWatcher 
{
 private:
  std::pair<char, cv::Mat> decoded;
  std::vector<uchar> buff;
  int port;
  int MAX_SIZE;
  bool receiving;
  bool die;
  bool inside;
  bool verbose;
  int currentWidth;
  int currentHeight;
  std::string winName;

  boost::asio::io_service my_io_service;
  boost::asio::ip::udp::socket *my_socket;
  boost::asio::ip::udp::endpoint sender_endpoint;

 public:
  RobotWatcher();
  ~RobotWatcher();
  bool setup(int port_, bool _verbose);
  int getWidth();
  int getHeight();
  std::pair<char, cv::Mat> grab_image();
  void kill();
};
