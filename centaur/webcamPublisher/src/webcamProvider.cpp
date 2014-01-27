#include "webcamProvider.h"

webcamProvider::webcamProvider(int _index, int _sleep_time, bool _detect, std::string _address, std::string _port) : resolver(this->io_service) {
	die.store(false);
	verbose = false;
	detectionTime.store(0);

	index = _index;
	sleep_time = _sleep_time;
	detect = _detect;
	address = _address;
	port = _port;

	//Boost items
	query = new boost::asio::ip::udp::resolver::query(boost::asio::ip::udp::v4(), address, port);
	receiver_endpoint = *resolver.resolve(*query);
	socket = new boost::asio::ip::udp::socket(io_service);
	connected = false;	

	//Image items
	image_quality = IMAGE_QUALITY;
	param = std::vector<int>(2);
	param[0]=CV_IMWRITE_JPEG_QUALITY;
	param[1]=image_quality;
	output_type = ".jpg";

	width = DEFAULT_WIDTH;
	height = DEFAULT_HEIGHT;
}

bool webcamProvider::init() {
	socket->open(boost::asio::ip::udp::v4());

	capture.open(0);
	if(!capture.isOpened())
	{
		std::cout << "ERROR: capture is NULL \n";
		capture.release();
		return false;
	}

	capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);

	if(detect) {	
		//Load detectableObjects and detectableKeypoints here
		//loadImagesAndKeypoints(fs::system_complete(argv[0]).string(), detectableObjects, detectableKeypoints);
	
		//If something went wrong loading the images or keypoints, turn off detection	
		if(detectableObjects.empty()) { // || detectableObjects.size() != detectableKeypoints.size()) {
			detect = false;
			std::cout << "detecting is off...\n";
		}
	}
}

void webcamProvider::provide() {
	while(!die.load())
	{
		capture >> frame;
		//gray = frame.clone();
		//cvtColor(frame, gray, CV_RGB2GRAY);

		if(detect) {
			//findObjectSURF(detectableObjects, frame, detectableKeypoints);
		}

		imencode(output_type.c_str(), frame, buff, param);
		if(verbose) std::cout<<"coded file size(jpg)"<<buff.size()<<std::endl;
		socket->send_to(boost::asio::buffer(buff,buff.size()), receiver_endpoint);

		waitKey(sleep_time);
	}
}

bool webcamProvider::setResolution(int _width, int _height) {
	if(_width >= 160 && _width <= 320 && _height >= 120 && _height <= 240) {
		width = _width;
		height = _height;
		capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
		return true;
	}
	return false;
}
bool webcamProvider::setQuality(int _quality) {
	if(_quality > 0 && _quality <= 100) {
		param[1] = _quality;
		return true;
	}
	return false;
}

bool webcamProvider::setFramerate(int _framerate) {
	if(_framerate > 0 && _framerate < 100000) {
		sleep_time = _framerate;
		return true;
	}
	return false;
}

bool webcamProvider::kill() {
	if(!die.load()) {
		die.store(true);
		return die.load();
	}
	return die.load();
}

webcamProvider::~webcamProvider() {
	//Cleanup
	std::cout << std::endl << "releasing capture and freeing mat memory..." << std::endl;
	capture.release();
	frame.release();
	gray.release();
	for(int i = 0; i < detectableObjects.size(); ++i) {
		detectableObjects[i].release();	
	}
	std::cout << "--done!" << std::endl;
	std::cout << "closing boost socket and freeing packet..." << std::endl;
	socket->close();
	std::cout << "--done!" << std::endl;	
}


//////////////////////////////////////DETECTABLE STUFF//////////////////////////////////////////////////////////////////

/*
The following is sample code to write and read keypoint data
 *We can use it when ready

//TO WRITE
vector<KeyPoint> myKpVec;
FileStorage fs(filename,FileStorage::WRITE);

ostringstream oss;
for(size_t i;i<myKpVec.size();++i) {
    oss << i;
    fs << oss.str() << myKpVec[i];
}
  fs.release();

//TO READ
vector<KeyPoint> myKpVec;
FileStorage fs(filename,FileStorage::READ);
ostringstream oss;
KeyPoint aKeypoint;
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
{return false;}
  /*	//-- Step 1: Detect the keypoints using SURF Detector
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

			//if(verbose) printf("-- Max dist : %f \n", max_dist );
			//if(verbose) printf("-- Min dist : %f \n", min_dist );

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
}*/

void loadImagesAndKeypoints(std::string _path, std::vector<Mat>& detectableObjects, std::vector<KeyPoint>& detectableKeypoints) 
{}
/*
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
}*/
