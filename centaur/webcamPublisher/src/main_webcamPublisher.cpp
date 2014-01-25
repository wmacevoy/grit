#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/include/opencv2/highgui/highgui.hpp"
#include "/usr/include/opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "boost/filesystem.hpp"

#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include "SDL/SDL_net.h"
#include "Configure.h"

Configure cfg;

using namespace cv;
using namespace std;
namespace fs = boost::filesystem;

volatile bool die = false;
bool verbose = false;

void quitproc(int param)
{
	std::cout << "\nQuitting..." << std::endl;
	die = true;
}

bool findObjectSURF( Mat img_1, Mat img_2 )
{
   //-- Step 1: Detect the keypoints using SURF Detector
  static int minHessian = 600;
	static int minGoodMatches = 7;

  SurfFeatureDetector detector( minHessian );

  std::vector<KeyPoint> keypoints_1, keypoints_2;

  detector.detect( img_1, keypoints_1 );
  detector.detect( img_2, keypoints_2 );

  //-- Step 2: Calculate descriptors (feature vectors)
  SurfDescriptorExtractor extractor;

  Mat descriptors_1, descriptors_2;

  extractor.compute( img_1, keypoints_1, descriptors_1 );
  extractor.compute( img_2, keypoints_2, descriptors_2 );

	if(!descriptors_1.empty() && !descriptors_2.empty()) {
		//-- Step 3: Matching descriptor vectors using FLANN matcher
		FlannBasedMatcher matcher;
		std::vector< DMatch > matches;
		matcher.match( descriptors_1, descriptors_2, matches );

		double max_dist = 0; double min_dist = 100;

		//-- Quick calculation of max and min distances between keypoints
		for( int i = 0; i < descriptors_1.rows; i++ )
		{ double dist = matches[i].distance;
		  if( dist < min_dist ) min_dist = dist;
		  if( dist > max_dist ) max_dist = dist;
		}

		if(verbose) printf("-- Max dist : %f \n", max_dist );
		if(verbose) printf("-- Min dist : %f \n", min_dist );

		//-- Draw only "good" matches (i.e. whose distance is less than 1.2*min_dist )
		//-- p.s.- radiusMatch can also be used here.
		std::vector< DMatch > good_matches;

		for( int i = 0; i < descriptors_1.rows; i++ ) { 
			if( matches[i].distance < 1.2*min_dist ) { 
				good_matches.push_back( matches[i] ); 
			}
		}

		if(good_matches.size() >= minGoodMatches) {
			//Find distances between keypoints and make sure there are enough in a given area

			//-- Draw only "good" matches
			Mat img_matches;
			drawMatches( img_1, keypoints_1, img_2, keypoints_2,
				           good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
				           vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

			//-- Show detected matches
			imshow( "Good Matches", img_matches );
		}
	}
	
	return true;
}

void loadImagesAndKeypoints(std::string _path, std::vector<Mat>& detectableObjects, std::vector<KeyPoint>& detectableKeypoints) {
	_path = _path.substr(0, _path.find_last_of('/', _path.size())) + "/detectables";
	fs::path p(_path);

	if(fs::exists(p)) {
		if(fs::is_directory(p)) {
			typedef std::vector<fs::path> vec;
			vec v, w;

			copy(fs::directory_iterator(p), fs::directory_iterator(), back_inserter(v));

			sort(v.begin(), v.end());

			for (vec::const_iterator it(v.begin()); it != v.end(); ++it) {
				//Load images
				if(fs::extension(*it) == ".png") {
					std::string load = it->string();
					detectableObjects.push_back(imread(load, CV_LOAD_IMAGE_GRAYSCALE));
				}
				//Load keypoint data
				else if(fs::extension(*it) == ".kpd") {
				
				}				
			}
		}
	}
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("webcam.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("webcam.provider.verbose", false);
	if (verbose) cfg.show();

	int index = (int)cfg.num("webcam.provider.index", 0);
	int sleep_time = (int)cfg.num("webcam.provider.sleep_time");
	bool detect = cfg.flag("webcam.provider.detect");
	std::string address = cfg.str("webcam.provider.c_ip").c_str();
	int port = (int)cfg.num("webcam.provider.port");

	//SDL items
	UDPsocket sd;
	UDPpacket* p;
	IPaddress ip;

	//SURF items
	std::vector<Mat> detectableObjects;
	std::vector<KeyPoint> detectableKeypoints;

	//Image items
	uint8_t areaOfFrame = 1;
	bool connected = false;
	std::string cascadeName;
	Mat frame;
	Mat gray;	

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	//Initialize SDL_net
	SDLNet_Init();
		
	while(!(sd = SDLNet_UDP_Open(0)) && !die) {
		printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	while((SDLNet_ResolveHost(&ip, address.c_str(), port) == -1) && !die) {
		fprintf(stderr, "SDLNet_ResolveHost(%s %d): %s\n", address.c_str(), port, SDLNet_GetError());
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	p = SDLNet_AllocPacket(4801);
	p->address.host = ip.host;
	p->address.port = ip.port;
	p->len = 4800 + sizeof(uint8_t);
	
	VideoCapture capture(index);
	if(!capture.isOpened())
	{
		std::cout << "ERROR: capture is NULL \n";
		capture.release();
		return 1;
	}

	capture.set(CV_CAP_PROP_FRAME_WIDTH, 160);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 120);
	
	if(verbose) std::cout << "Color: " << frame.channels() << " " << frame.depth() << std::endl;
	if(verbose) std::cout << "Gray: " << gray.channels() << " " << gray.depth() << std::endl;

	//Load detectableObjects and detectableKeypoints here
	loadImagesAndKeypoints(fs::system_complete(argv[0]).string(), detectableObjects, detectableKeypoints);
	
	//If something went wrong loading the images or keypoints, turn off detection	
	if(detectableObjects.empty()) { // || detectableObjects.size() != detectableKeypoints.size()) {
		detect = false;
		std::cout << "detecting is off...\n";
	}

	int count = 0;
	while(!die)
	{
		capture >> frame;
		gray = frame.clone();
		cvtColor(frame, gray, CV_RGB2GRAY);

		if(detect) {
			findObjectSURF(detectableObjects[0], gray);
		}

		memcpy(p->data, &areaOfFrame, sizeof(uint8_t));
		
		switch(areaOfFrame) {
		case 1:
			for(int i = 0; i < gray.cols / 2; ++i)
				for(int j = 0; j < gray.rows / 2; ++j)
					p->data[1 + count++] = gray.at<uchar>(Point(i,j));
			break;
		case 2:
			for(int i = gray.cols / 2; i < gray.cols; ++i)
				for(int j = 0; j < gray.rows / 2; ++j)
					p->data[1 + count++] = gray.at<uchar>(Point(i,j));
			break;
		case 3:
			for(int i = 0; i < gray.cols / 2; ++i)
				for(int j = gray.rows / 2; j < gray.rows; ++j)
					p->data[1 + count++] = gray.at<uchar>(Point(i,j));
			break;
		case 4:
			for(int i = gray.cols / 2; i < gray.cols; ++i)
				for(int j = gray.rows / 2; j < gray.rows; ++j)
					p->data[1 + count++] = gray.at<uchar>(Point(i,j));
			break;
		}
		count = 0;

		SDLNet_UDP_Send(sd, -1, p);

		++areaOfFrame;
		if(areaOfFrame > 4 ) {
			areaOfFrame = 1;
		}

		waitKey(sleep_time);
	}

	//Cleanup
	std::cout << "releasing capture and freeing mat memory..." << std::endl;
	capture.release();
	frame.release();
	gray.release();
	for(int i = 0; i < detectableObjects.size(); ++i) {
		detectableObjects[i].release();	
	}
	std::cout << "--done!" << std::endl;
	std::cout << "closing SDL and freeing packet..." << std::endl;
	SDLNet_FreePacket(p);
	SDLNet_Quit();
	std::cout << "--done!" << std::endl;
	return 0;
}
