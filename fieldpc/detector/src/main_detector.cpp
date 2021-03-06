#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Configure.h"
#include "robocam.h"
#include "BodyMessage.h"
#include "CSVRead.h"
#include "ZMQHub.h"
#include "Lock.h"
#include "now.h"

#include <iostream>
#include <signal.h>
#include <string>
#include <mutex>
#include <string.h>
#include <sstream>
#include <fcntl.h> 
#include <iomanip>

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
}

std::string convstr(const float t) {
  std::stringstream ftoa;
  ftoa << std::setprecision(3) << std::setw(4) << t << " CM";
  return ftoa.str();
}
std::vector<int> getHpHy(Mat frame, float fovx, float fovy, int fx, int fy)
{
  std::vector<int> ret;
  int centreX = (float)frame.cols / 2.0;
  int centreY = (float)frame.rows / 2.0;
 
  float degPerPixelX = fovx / (float)(frame.cols);
  float degPerPixelY = fovy / (float)frame.rows;

  float diffX = centreX - fx;/*detected object x centre*/ 
  float diffY = centreY - fy;/*detected object y centre*/

  int thetaX = degPerPixelX * diffX;
  int thetaY = degPerPixelY * diffY * -1.0;
 
  if(verbose)std::cout <<"frame.rows: "<< frame.rows<<" frame.cols: "<< frame.cols<<", centerx: " << centreX << ", centery: " << centreY << ", dppx: " << degPerPixelX << ", dppy: " << degPerPixelY << ", diffX: " << 
	       diffX << ", diffY: " << diffY << ", thetaX: " << thetaX << ", thetaY: " << thetaY << ", fx: " << fx << ", fy: " << fy << std::endl;
 
  ret.push_back(thetaX/2);
  ret.push_back(thetaY/4);
 
  return ret;
}
std::vector<int> home(Mat frame)
{
  std::vector<int>  ret;

  int thetaX = 0;
  int thetaY = 0;
 
  ret.push_back(thetaX);
  ret.push_back(thetaY);
 
  return ret;
}
void moveHome(cv::Mat& tmpframe, float fovx, float fovy, int x, int y, std::string commands[]){
  std::stringstream format;
  std::vector<std::string> commandsToSend;
  std::vector<int> headmove = home(tmpframe);
  std::cout<<x<< " "<<y<<std::endl;
	
  if(headmove.size() <= 2)
    {
      if(headmove[0] != 0)
	{
	  format << commands[2] << headmove[0];
	  commandsToSend.push_back(format.str());
	  //if(verbose) 
	  std::cout << "sending command: " << format.str() << std::endl;
	}
      format.str( std::string() );
      format.clear();
      if(headmove[1] != 0)
	{
	  format << commands[1] << headmove[1];
	  commandsToSend.push_back(format.str());
	  //if(verbose) 
	  std::cout << "sending command: " << format.str() << std::endl;
	}
    }
	
  //Send commands to body
  std::string recv;
  for(int i=0; i<commandsToSend.size(); ++i)
    {
      commander->send(commandsToSend[i]);
      commander->recv(recv);
      if(verbose) std::cout << "recv: " + recv << std::endl;
		
    }
  commandsToSend.resize(0);
}
void move(cv::Mat& tmpframe, float fovx, float fovy, int x, int y, std::string commands[]){
  std::stringstream format;
  std::vector<std::string> commandsToSend;
  std::vector<int> headmove = getHpHy(tmpframe, fovx, fovy, x, y);
  std::cout<<x<< " "<<y<<std::endl;
	
  if(headmove.size() <= 2)
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
      commander->recv(recv);
      if(verbose) std::cout << "recv: " + recv << std::endl;
		
    }
  commandsToSend.resize(0);
}
void circleDetect(cv::Mat& src, cv::Mat& grayframe,cv::Mat& tmpframe, int Intensity, int Min, int Max, int& x, int& y, bool& found){
  cvtColor(src,grayframe,CV_BGR2GRAY);
  GaussianBlur(grayframe,grayframe,Size(9,9),2,2);
  vector<Vec3f> circles;
  HoughCircles( grayframe, circles, CV_HOUGH_GRADIENT, 2,40,Intensity,100,Min,Max);//(inverting,spaceBetweenCenter,Circleresolution,centerResolution,minDia,maxDia)
  if(circles.size() > 0)
    {
	  found=true;	
      x = cvRound(circles[0][0]);
      y = cvRound(circles[0][1]);
      Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));//<---- this is the coords for center
      tmpframe = cv::Mat(src);
      int radius = cvRound(circles[0][2]);
      // circle center
      circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      circle( src, center, radius, Scalar(0,0,255), 2, 8, 0 );
      //putText(rot_frame,"Lidar Data",cv::Point(50,50), CV_FONT_HERSHEY_SIMPLEX, 0.5,cv::Scalar(0,0,255),1,8,false);
    }	
}
int main(int argc, char** argv)
{
	
  cfg.path("../../setup");
  cfg.args("detector.", argv);
  if (argc == 1) cfg.load("config.csv");
  verbose = cfg.flag("detector.verbose", true);
  if (verbose) cfg.show();

  int sleep_time = (int)cfg.num("detector.sleep_time");
  int port = (int)cfg.num("webcam.provider.port");
  int lidarPort = (int)cfg.num("lidar.provider.port");
  float fovx = (float)cfg.num("detector.camfovx");
  float fovy = (float)cfg.num("detector.camfovy");
  float camLeftIntensity = (float)cfg.num("detector.camLeftIntensity");
  float LcircleMin = (float)cfg.num("detector.LcircleMin");
  float LcircleMax = (float)cfg.num("detector.LcircleMax");
  float RcircleMin = (float)cfg.num("detector.RcircleMin");
  float RcircleMax = (float)cfg.num("detector.RcircleMax");
  float camRightIntensity = (float)cfg.num("detector.camRightIntensity");
	
  commander = std::shared_ptr < Commander > (new Commander());
  commander->publish = cfg.str("detector.publish");
  commander->subscribers = cfg.list("detector.subscribers");
  commander->rxTimeout = 1e6;
  commander->start();
    
    
  int t1 = 0, t2 = 0, timeOut = 2;
    
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
  LidarLayer lidarLayer;

  std::string windowNameL = "Left";
  std::string imageName = "";
  namedWindow(windowNameL, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO |CV_WINDOW_AUTOSIZE);

  std::string windowNameR = "Right";
  namedWindow(windowNameR, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO |CV_WINDOW_AUTOSIZE);
	
  std::string windowDisparity = "Disparity";
  namedWindow(windowDisparity, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_WINDOW_AUTOSIZE);
	
  signal(SIGINT, quitproc);
  signal(SIGTERM, quitproc);
  signal(SIGQUIT, quitproc);
	
  //Initialize watcher
  my_watcher.setup(port, verbose);
  lidarLayer.setup(lidarPort, verbose);
	

  int x = 0, y = 0;
  Mat grayframe;
  Mat tmpframe;
  Mat imgRight;
  Mat imgLeft;
  bool found = false;
	
  char lr = '\0';
  std::pair<char, cv::Mat> decoded;
  while(!die) 
    {
      //Grab image
      if(receiving) 
	  {
	  int dist = lidarLayer.recvData();
			
	  try
	   {
		decoded = my_watcher.grab_image();

		lr = decoded.first;
		if (lr == 'L')
			{
			imgLeft = decoded.second;
			//center of frame
			Point fcenter(imgLeft.cols/2, imgLeft.rows/2);
			circle( imgLeft, fcenter, 3, Scalar(0,255,0), -1, 8, 0 ); 
			}
		else if (lr == 'R'/* && !found*/)
			{
			imgRight = decoded.second;
			//center of frame
			Point fcenter(imgRight.cols/2, imgRight.rows/2);
			circle( imgRight, fcenter, 3, Scalar(0,255,0), -1, 8, 0 ); 
			}
	    }
	  catch(int e)
	    {
	    std::cout << "No data available!" << std::endl;
	    }  
	 
	    if(!imgLeft.empty() && lr == 'L') {
			circleDetect(imgLeft,grayframe,tmpframe,camLeftIntensity,LcircleMin,LcircleMax,x,y, found);
			imshow(windowNameL, imgLeft);
	      }			
	    if(!imgRight.empty() && lr == 'R'){
			circleDetect(imgRight,grayframe,tmpframe,camRightIntensity,RcircleMin,RcircleMax,x,y, found);
			imshow(windowNameR, imgRight);
	      }

		  t2=now();
			
		  if(t2-t1>timeOut)
			if(lr == 'R' && !tmpframe.empty())
				{
				move(tmpframe,fovx,fovy,x,y,commands);
				t1=now();
				}		
		}

      //Sleep and allow user interaction
      //usleep(5);
      char c = waitKey(sleep_time); 
      if(c == 'q') die = true;
      else if(c == 'c') {
		imageName = "image";
		imageName += itoa(imgNum++);
		imageName += ".jpg";
		imwrite(imageName,  imgLeft);
	  }
	  else if(c == 'p') {
		receiving = !receiving;
		  }
	  else if(c == 'h') {
	//check this befor using bf
	//moveHome(rot_imageR,fovx,fovy,x,y,commands);

      }
       else if(c == 'd') {
		    if(!imgLeft.empty() && ! imgRight.empty())
				{/*
				Mat g1, g2;
				cvtColor(rot_imageL, g1, CV_RGB2GRAY);
				cvtColor(rot_imageR, g2, CV_RGB2GRAY);
				
				std::cout << "R: " << g1.total()*g1.elemSize() << ", L: " << g2.total()*g2.elemSize() << std::endl;
				
				Mat imgDisparity16S = Mat( rot_imageL.rows, rot_imageL.cols, CV_16S );
				Mat imgDisparity8U = Mat( rot_imageL.rows, rot_imageL.cols, CV_8UC1 );

				if( !rot_imageL.data || !rot_imageR.data ) {
				  std::cout<< " --(!) Error reading images " << std::endl; 
				}

				//-- 2. Call the constructor for StereoBM
				int ndisparities = 16*5;  // < Range of disparity 
				int SADWindowSize = 21; // < Size of the block window. Must be odd 

				StereoBM sbm( StereoBM::BASIC_PRESET,ndisparities,SADWindowSize );

				
				//-- 3. Calculate the disparity image
				sbm( g1, g2, imgDisparity16S, CV_16S );

				//-- Check its extreme values
				double minVal; double maxVal;

				minMaxLoc( imgDisparity16S, &minVal, &maxVal );

				printf("Min disp: %f Max value: %f \n", minVal, maxVal);

				//-- 4. Display it as a CV_8UC1 image
				imgDisparity16S.convertTo( imgDisparity8U, CV_8UC1, 255/(maxVal - minVal));

						  
				imshow( windowDisparity, imgDisparity8U );

				//-- 5. Save the image
				// imwrite("SBM_sample.png", imgDisparity16S);
				g1.release();
				g2.release();*/
			    }
		  }

      imgRight.release();
      imgLeft.release();
      tmpframe.release();
    }


  std::cout << std::endl << "Quitting main_Detector..." << std::endl;
  destroyAllWindows();
  commander.reset();
  imgLeft.release();
  imgRight.release();
  tmpframe.release();
  std::cout << "--Detector done!" << std::endl;
  return 0;
}
