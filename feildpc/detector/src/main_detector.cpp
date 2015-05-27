#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <mutex>
#include <string.h>
#include "Configure.h"
#include "LidarMessage.h"
#include "fk_lidar.h"
#include "robocam.h"
#include "BodyMessage.h"
#include "CSVRead.h"
#include "ZMQHub.h"
#include "Lock.h"

using namespace cv;

Configure cfg;

RobotWatcher *ghost;

class Commander : public ZMQHub
{
public:
  std::mutex sendsMutex;
  std::mutex recvsMutex;
  std::list < std::string > sends;
  std::list < std::string > recvs;
  
  void send(const std::string &content)
  {
    Lock lock(sendsMutex);
    cout << "send: " << content << endl;
    sends.push_back(content);
  }

  bool recv(std::string &content)
  {
    Lock lock(recvsMutex);
    if (!recvs.empty()) {
      content=recvs.front();
      recvs.pop_front();
      cout << "revd: " << content << endl;
      return true;
    } else {
      return false;
    }
  }

  bool rx(ZMQSubscribeSocket &socket)
  {
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    char *data = (char*) msg.data();
    size_t size = *(uint16_t*)data;
    std::string reply(data+2,size);
    { 
      Lock lock(recvsMutex);
      recvs.push_back(reply);
    }
    return true;
  }

  bool tx(ZMQPublishSocket &socket)
  {
    Lock lock(sendsMutex);
    bool ok=true;

    while (!sends.empty()) {
      std::string &message = *sends.begin();
      uint16_t size = (message.size() < BODY_MESSAGE_MAXLEN) ? message.size() : BODY_MESSAGE_MAXLEN;
      ZMQMessage msg(size+2);
      char *data = (char*)msg.data();
      *(uint16_t*)data = size;
      memcpy(data+2,&message[0],size);
      if (msg.send(socket) == 0) ok=false;
      sends.pop_front();
    }
    return ok;
  }
};

std::shared_ptr < Commander > commander;

volatile bool die = false;
bool inside = false;
bool verbose = false;

template<class T>
std::string itoa(const T& t) {
	std::stringstream itoa;
	itoa << t;
	return itoa.str();
}

void quitproc(int param) {
	die = true;
	ghost->kill();
}

void mouseEvent(int evt, int x, int y, int flags, void* param) {
	if(evt == CV_EVENT_MOUSEMOVE) {
		ghost->setMouse(x, y);
	}
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("detector.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("detector.verbose", false);
	if (verbose) cfg.show();

	int sleep_time_gray = cfg.num("detector.sleep_time");
	int port = (int)cfg.num("webcam.provider.port");
	std::string lidarAddress = cfg.str("lidar.provider.subscribe");
	bool lidarCalibration = cfg.flag("detector.calibration", false);
	float fov = (float)cfg.num("detector.camfov");


	RobotWatcher my_watcher;
	ghost = &my_watcher;
	
	int imgNum = 0;
	int sleep_time = sleep_time_gray;
	bool receiving = true;
	Mat frame;

	std::string windowName = "ICU";
	std::string imageName = "";
	namedWindow(windowName, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_NORMAL);

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);
	
	//Initialize watcher
	my_watcher.setup(port, true, verbose);
	my_watcher.setupLidar(lidarAddress, lidarCalibration, verbose);

	cvSetMouseCallback(windowName.c_str(), mouseEvent, 0);

	std::vector<std::string> commandsToSend;
	while(!die) {
		//Grab image
		if(receiving) {
        try
          {
		  frame = my_watcher.grab_image();
          }
        catch(int e)
          {
          std::cout << "No data available!" << std::endl;
          }

        if(!frame.empty())
         {
		 imshow(windowName, frame);
         }
		}
		
		//Detect object and populate list of commands
		float degPerPixel = fov / (float)frame.cols;
		commandsToSend.push_back(cmd);
		
		//Send commands to body
		std::string recv;
		for(int i=0; i<commandsToSend.size(); ++i)
		 {
		  commander->send(commandsToSend[i]);
		  commander->recv(recv);
		  if(recv.find("ok") == std::string::npos)
		   {
			if(verbose) std::cout << "Error processing commands. " + recv << std::endl;
			break;  
		   }
		 }
		
		//Sleep and allow user interaction
		char c = waitKey(sleep_time);
		if(c == 'q') die = true;
		else if(c == 's') {
			imageName = "image";
			imageName += itoa(imgNum++);
			imageName += ".jpg";
			imwrite(imageName,  frame);
		}
		else if(c == 'p') {
			receiving = !receiving;
		}
	}

	std::cout << std::endl << "Quitting..." << std::endl;
	std::cout << "destroying window and freeing mat memory..." << std::endl;
	destroyWindow(windowName);
	frame.release();
	std::cout << "--done!" << std::endl;
	return 0;
}
