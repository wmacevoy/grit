#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Configure.h"
#include "LidarMessage.h"
#include "fk_lidar.h"
#include "robocam.h"
#include "BodyMessage.h"
#include "CSVRead.h"
#include "ZMQHub.h"
#include "Lock.h"

#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <mutex>
#include <string.h>
#include <sstream> 

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
    std::cout << "send: " << content << std::endl;
    sends.push_back(content);
  }

  bool recv(std::string &content)
  {
    Lock lock(recvsMutex);
    if (!recvs.empty()) {
      content=recvs.front();
      recvs.pop_front();
      std::cout << "revd: " << content << std::endl;
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
	commander.reset();
	ghost->kill();
}

void mouseEvent(int evt, int x, int y, int flags, void* param) {
	if(evt == CV_EVENT_MOUSEMOVE) {
		ghost->setMouse(x, y);
	}
}

std::vector<int> getHpHy(Mat frame, float fovx, float fovy, int fx, int fy)
 {
 std::vector<int> ret;
 int centreX = (float)frame.cols / 2.0;
 int centreY = (float)frame.rows / 2.0;
 
 float degPerPixelX = fovx / (float)frame.cols;
 float degPerPixelY = fovy / (float)frame.rows;

 float diffX = centreX - fx;/*detected object x centre*/ 
 float diffY = centreY - fy;/*detected object y centre*/

 int thetaX = degPerPixelX * diffX;
 int thetaY = degPerPixelY * diffY * -1.0;
 
 if(verbose) std::cout << "centerx: " << centreX << ", centery: " << centreY << ", dppx: " << degPerPixelX << ", dppy: " << degPerPixelY << ", diffX: " << 
                          diffX << ", diffY: " << diffY << ", thetaX: " << thetaX << ", thetaY: " << thetaY << ", fx: " << fx << ", fy: " << fy << std::endl;
 
 ret.push_back(thetaX/2);
 ret.push_back(thetaY/4);
 
 return ret;
 }

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("detector.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("detector.verbose", false);
	if (verbose) cfg.show();

	int sleep_time = (int)cfg.num("detector.sleep_time");
	int port = (int)cfg.num("webcam.provider.port");
	std::string lidarAddress = cfg.str("lidar.provider.subscribe");
	bool lidarCalibration = cfg.flag("detector.calibration", false);
	float fovx = (float)cfg.num("detector.camfovx");
	float fovy = (float)cfg.num("detector.camfovy");
	
	commander = std::shared_ptr < Commander > (new Commander());
    commander->publish = cfg.str("guicmdr.publish");
    commander->subscribers = cfg.list("guicmdr.subscribers");
    commander->rxTimeout = 1e6;
    commander->start();
    
    std::string commands[20];
    commands[0] = "dhome";
    commands[1] = "hp "; //Head Pitch
    commands[2] = "hy "; //Head Yaw
    commands[3] = "bf ";
    commands[4] = "";
    commands[5] = "";
    commands[6] = "";
    commands[7] = "";
    commands[8] = "";
    commands[9] = "";
    commands[10] = "";
    commands[11] = "";
    commands[12] = "";
    commands[13] = "";
    commands[14] = "";
    commands[15] = "";
    commands[16] = "";
    commands[17] = "";
    commands[18] = "";
    commands[19] = "speed "; //Set cogburn sim-time speed

	RobotWatcher my_watcher;
	ghost = &my_watcher;
	
	int imgNum = 0;
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

    int x = 0, y = 0;
    Mat grayframe;
    Mat tmpframe;
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
			cvtColor(frame,grayframe,CV_BGR2GRAY);
			GaussianBlur(grayframe,grayframe,Size(9,9),2,2);
			vector<Vec3f> circles;
			HoughCircles( grayframe, circles, CV_HOUGH_GRADIENT, 2,30,80,100,1,200 );
			if(circles.size() > 0)
			{
				tmpframe = frame;
				x = cvRound(circles[0][0]);
				y = cvRound(circles[0][1]);
				Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));//<---- this is the coords for center
				//cv::cvSaveImage(convert.str().c_str(),text)
				//cv::putText(frame, text,cv::Point(50,50), CV_FONT_HERSHEY_SIMPLEX, 0.5,cv::Scalar(255),1,8,false);
				int radius = cvRound(circles[0][2]);
				// circle center
				circle( frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
				// circle outline
				circle( frame, center, radius, Scalar(0,0,255), 3, 8, 0 );
			}
		 imshow(windowName, frame);
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
		else if(c == 'd') {
			//Detect object and populate list of commands
			if(!tmpframe.empty())
			  {
				std::stringstream format;
				std::vector<std::string> commandsToSend;
				std::vector<int> headmove = getHpHy(frame, fovx, fovy, x, y);
				if(headmove.size() == 2)
				{
				 if(headmove[0] != 0)
				  {
				  format << commands[2] << headmove[0];
				  commandsToSend.push_back(format.str());
				  if(verbose) std::cout << "sending command: " << format.str() << std::endl;
				  }
				 format.str( std::string() );
				 format.clear();
				 if(headmove[1] != 0)
				  {
				  format << commands[1] << headmove[1];
				  commandsToSend.push_back(format.str());
				  if(verbose) std::cout << "sending command: " << format.str() << std::endl;
				  }
				 }
				
				//Send commands to body
				std::string recv;
				for(int i=0; i<commandsToSend.size(); ++i)
				 {
				  commander->send(commandsToSend[i]);
				  /*commander->recv(recv);
				  if(recv.find("ok") == std::string::npos)
				   {
					if(verbose) std::cout << "Error processing commands. " + recv << std::endl;
					break;  
				   }*/
				 }
				 commandsToSend.resize(0);
			 }
			 
		}
	}

	std::cout << std::endl << "Quitting..." << std::endl;
	std::cout << "destroying window and freeing mat memory..." << std::endl;
	destroyWindow(windowName);
	frame.release();
	std::cout << "--done!" << std::endl;
	return 0;
}
