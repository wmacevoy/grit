#include "webcamProviderObj.h"

webcamProvider::webcamProvider(int _indexR, int _indexL, int _sleep_time, bool _verbose, const char* _argv0, std::string _address, std::string _port) : resolver(this->io_service) {
	die.store(false);
	verbose = _verbose;
	indexR = _indexR;
	indexL = _indexL;
	sleep_time = _sleep_time;
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

	
	path = fs::system_complete(_argv0).string();

}

bool webcamProvider::init() {
	socket->open(boost::asio::ip::udp::v4());

	captureR.open(indexR);
	if(!captureR.isOpened())
	{
		std::cout << "ERROR: capture is NULL on device " << indexR << "\n";
		captureR.release();
		return false;
	}
	
	captureL.open(indexL);
	if(!captureL.isOpened())
	{
		std::cout << "ERROR: capture is NULL on device " << indexL << "\n";
		captureL.release();
		return false;
	}


	captureR.set(CV_CAP_PROP_FRAME_WIDTH, width);//width
	captureR.set(CV_CAP_PROP_FRAME_HEIGHT,height);//height
	
	captureL.set(CV_CAP_PROP_FRAME_WIDTH, width);//width
	captureL.set(CV_CAP_PROP_FRAME_HEIGHT,height);//height
	//std::cout<<width<<" "<<height<<std::endl;


	return true;
}

void webcamProvider::rotate(cv::Mat& src, double angle, cv::Mat& dst){
	cv::Point2f src_center(src.cols/2.0f,src.rows/2.0f);
	cv::Mat rot_mat = cv::getRotationMatrix2D(src_center,angle,1.0);
	cv::warpAffine(src, dst, rot_mat, src.size());
}

void webcamProvider::provide() {
	while(!die.load())
	{
		captureR >> frameR;
		captureL >> frameL;

		imencode(output_type.c_str(), frameR, buff, param);
		buff.insert(buff.begin(), 'R');
		if(verbose) std::cout<<"coded file size(jpg) R"<<buff.size()<< ", width: " << width << ", height: " << height << std::endl;
		socket->send_to(boost::asio::buffer(buff, buff.size()), receiver_endpoint);
		
		buff.resize(0);
		
		imencode(output_type.c_str(), frameL, buff, param);
		buff.insert(buff.begin(), 'L');
		if(verbose) std::cout<<"coded file size(jpg) L"<<buff.size()<< ", width: " << width << ", height: " << height << std::endl;
		socket->send_to(boost::asio::buffer(buff, buff.size()), receiver_endpoint);

		waitKey(sleep_time);
	}
}

int webcamProvider::getwidth(){
	return width;
}
int webcamProvider::getheight(){
	return height;
}
/*
bool webcamProvider::setResolution(int _width, int _height) {
	if(_width >= 160 && _width <= 1280 && _height >= 120 && _height <= 720) {
		width = _width;
		height = _height;
		capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
		return true;
	}
	return false;
}*/

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
	captureR.release();
	captureL.release();
	frameR.release();
	frameL.release();
	//gray.release();
	std::cout << "--done!" << std::endl;
	std::cout << "closing boost socket..." << std::endl;
	socket->close();
	std::cout << "--done!" << std::endl;	
}
