//Robocam Receiver
//1-26-14
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include "lidarLayer.h"

const int normalWidth = 320;
const int normalHeight = 240;

class RobotWatcher 
{
 private:
  cv::Mat decoded;
  std::vector<uchar> buff;
  int port;
  int MAX_SIZE;
  bool receiving;
  bool die;
  bool inside;
  bool hasLidar;
  bool verbose;
  int mx;
  int my;
  int currentWidth;
  int currentHeight;
  std::string winName;

  boost::asio::io_service my_io_service;
  boost::asio::ip::udp::socket *my_socket;
  boost::asio::ip::udp::endpoint sender_endpoint;

  LidarLayer d;

 public:
  RobotWatcher();
  ~RobotWatcher();
  bool setup(int port_, bool _hasLidar, bool _verbose);
  bool setupLidar(std::string _address, bool _calibration, bool _verbose);
  cv::Mat grab_image();
  void run();
  void kill();
  void setMouse(int _x, int _y);
};
