#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/include/opencv2/highgui/highgui.hpp"
#include "/usr/include/opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include "SDL/SDL_net.h"
#include "Configure.h"

Configure cfg;

using namespace cv;

volatile bool die = false;
bool verbose = false;
CascadeClassifier cc;

void quitproc(int param)
{
	std::cout << "\nQuitting..." << std::endl;
	die = true;
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
	int minHessian = 400;
	int minGoodMatches = 50; //The minimum number of matches necessary to be a detected object.  Needs to be found 40 is just a placeholder.
	double maxDist = 0.0, minDist = 100.0;
	std::vector<Mat> detectableObjects;
	std::vector<KeyPoint> sceneKeypoints;
	std::vector<KeyPoint> detectableKeypoints;
	SurfFeatureDetector detector(minHessian);
	SurfDescriptorExtractor extractor;
  	Mat descriptors_object, descriptors_scene;
	Mat img_matches;
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	std::vector< DMatch > good_matches;
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

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
		
	while(!(sd = SDLNet_UDP_Open(port)) && !die) {
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
	//Testing
		detectableObjects.push_back(imread("/home/fit-pc/centaur/webcamPublisher/detectableHand.jpg", CV_LOAD_IMAGE_GRAYSCALE));
	
	//If something went wrong loading the images or keypoints, turn off detection	
	/*if(detectableObjects.size() <= 0 || detectableObjects.size() != detectableKeypoints.size()) {
		detect = false;
		std::cout << "detecting is off...\n";
	}*/

	int count = 0;
	while(!die)
	{
		capture >> frame;
		gray = frame.clone();
		cvtColor(frame, gray, CV_RGB2GRAY);

		if(detect) {
			//Only detect the keypoints for the scene once
			detector.detect(gray, sceneKeypoints);

			for(int i = 0; i < detectableObjects.size(); ++i) {
				//Detect keypoints, this is used as testing, we will make a document with all the keypoins
				//that corresponds with detectableObjects and load in start
				detector.detect(detectableObjects[i], detectableKeypoints);
				
				//Calculate descriptors
				extractor.compute(detectableObjects[i], detectableKeypoints, descriptors_object);
				extractor.compute(gray, sceneKeypoints, descriptors_scene);

				if(!descriptors_object.empty() && !descriptors_scene.empty()) {
					//Matching descriptor vectors using FLANN matcher
					matcher.match(descriptors_object, descriptors_scene, matches);
				
					for(int i = 0; i < descriptors_object.rows; i++) {
						double dist = matches[i].distance;
						if(dist < minDist) minDist = dist;
						if(dist > maxDist) maxDist = dist;
					}

					//Get only good matches 3*min
					for(int i = 0; i < descriptors_object.rows; i++) {
						if(matches[i].distance < 3*minDist){
							good_matches.push_back(matches[i]);
						}
					}

					if(good_matches.size() >= minGoodMatches) {
						std::cout << "match\n";
						drawMatches(detectableObjects[i], detectableKeypoints, gray, sceneKeypoints,
							       good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
							       vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

						//Homography
						/*Mat H = findHomography(obj, scene, RANSAC);

						//-- Get the corners from the image_1 ( the object to be "detected" )
						std::vector<Point2f> obj_corners(4);
						obj_corners[0] = Point(0,0); 
						obj_corners[1] = Point(detectableObjects[i].cols, 0);
						obj_corners[2] = Point(detectableObjects[i].cols, detectableObjects[i].rows); 
						obj_corners[3] = Point(0, detectableObjects[i].rows);
						std::vector<Point2f> scene_corners(4);

						perspectiveTransform(obj_corners, scene_corners, H);


						//-- Draw lines between the corners (the mapped object in the scene - image_2 )
						Point2f offset( (float)detectableObjects[i].cols, 0);
						line( img_matches, scene_corners[0] + offset, scene_corners[1] + offset, Scalar(0, 255, 0), 4 );
						line( img_matches, scene_corners[1] + offset, scene_corners[2] + offset, Scalar( 0, 255, 0), 4 );
						line( img_matches, scene_corners[2] + offset, scene_corners[3] + offset, Scalar( 0, 255, 0), 4 );
						line( img_matches, scene_corners[3] + offset, scene_corners[0] + offset, Scalar( 0, 255, 0), 4 );

						//-- Show detected matches*/
						imshow( "Good Matches & Object detection", img_matches );
						matches.clear();
						good_matches.clear();
					}
				}
			}
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
	descriptors_object.release();
	descriptors_scene.release();
	img_matches.release();
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
