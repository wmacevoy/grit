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
#include <atomic>
#include "Configure.h"
#include <boost/asio.hpp>

Configure cfg;

using namespace cv;
using namespace std;
using namespace std::chrono;
namespace fs = boost::filesystem;

typedef std::chrono::duration<int,std::milli> millisecs_t;

std::atomic<bool> die(false);
bool verbose = false;
std::atomic<int> detectionTime;

const int IMAGE_QUALITY=70;

void quitproc(int param)
{
	std::cout << "\nQuitting..." << std::endl;
	die.store(true);
}

/*The following is sample code to write and read keypoint data
 *We can use it when ready

//TO WRITE
vector<Keypoint> myKpVec;
FileStorage fs(filename,FileStorage::WRITE);

ostringstream oss;
for(size_t i;i<myKpVec.size();++i) {
    oss << i;
    fs << oss.str() << myKpVec[i];
}
  fs.release();

//TO READ
vector<Keypoint> myKpVec;
FileStorage fs(filename,FileStorage::READ);
ostringstream oss;
Keypoint aKeypoint;
for(size_t i;i<myKpVec.size();<++i) {
    oss << i;
    fs[oss.str()] >> aKeypoint;
    myKpVec.push_back(aKeypoint);
}
fs.release();

*/

//img_1 are detectable objects, img_2 is the scene/camera image
//TODO: when ready, remove _ from _keypoints_2 and update code to keypoints_2[i]
//      then remove keypoint_2 detection from below
bool findObjectSURF( std::vector<Mat>& img_1, Mat img_2, std::vector<KeyPoint>& _keypoints_2 )
{
  	//-- Step 1: Detect the keypoints using SURF Detector
 	static int minHessian = 600;
	static int minGoodMatches = 7;
	static high_resolution_clock timer;

	high_resolution_clock::time_point start = timer.now();

 	SurfFeatureDetector detector( minHessian );
	SurfDescriptorExtractor extractor;

 	std::vector<KeyPoint> keypoints_1, keypoints_2;
	
	//Detect the keypoints in the scene only once
	Mat descriptors_2;
	detector.detect( img_2, keypoints_2 );

	//Extract the descriptors from the scene only once
	extractor.compute( img_2, keypoints_2, descriptors_2 );

	//Loop through the detectable objects and check for matches
	for(int i = 0; i < img_1.size(); ++i) {
		detector.detect( img_1[0], keypoints_1 );

		//-- Step 2: Calculate descriptors (feature vectors)
		Mat descriptors_1;

		extractor.compute( img_1[0], keypoints_1, descriptors_1 );

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
				drawMatches( img_1[0], keypoints_1, img_2, keypoints_2,
						         good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
						         vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

				//-- Show detected matches
				imshow( "Good Matches", img_matches );
			}
		}
	}
	high_resolution_clock::time_point end = timer.now();
	millisecs_t elapsed(std::chrono::duration_cast<millisecs_t>(end-start));
	detectionTime.store(elapsed.count());
	//std::cout << detectionTime << std::endl;
	
	
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
	std::string port = cfg.str("webcam.provider.port").c_str();

	//Boost items
	boost::asio::io_service io_service;
	boost::asio::ip::udp::resolver resolver(io_service);
	boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), address, port);
	boost::asio::ip::udp::endpoint receiver_endpoint = *resolver.resolve(query);
	boost::asio::ip::udp::socket socket(io_service);
	

	//SURF items
	std::vector<Mat> detectableObjects;
	std::vector<KeyPoint> detectableKeypoints;

	//Image items
	bool connected = false;
	std::string cascadeName;
	Mat frame;
	Mat gray;	

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	socket.open(boost::asio::ip::udp::v4());
	
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

	std::vector<uchar> buff;
	std::vector<int> param = std::vector<int>(2);
	param[0]=CV_IMWRITE_JPEG_QUALITY;
	param[1]=IMAGE_QUALITY;
	std::string output_type = ".jpg";

	int count = 0;
	while(!die.load())
	{
		capture >> frame;
		//gray = frame.clone();
		//cvtColor(frame, gray, CV_RGB2GRAY);

		if(detect) {
			findObjectSURF(detectableObjects, frame, detectableKeypoints);
		}

		imencode(output_type.c_str(), frame, buff, param);
		std::cout<<"coded file size(jpg)"<<buff.size()<<std::endl;
		socket.send_to(boost::asio::buffer(buff,buff.size()), receiver_endpoint);

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
	std::cout << "closing boost socket and freeing packet..." << std::endl;
	socket.close();
	std::cout << "--done!" << std::endl;
	return 0;
}
