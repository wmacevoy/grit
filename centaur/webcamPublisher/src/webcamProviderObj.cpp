#include "webcamProviderObj.h"

webcamProvider::webcamProvider(int _index, int _sleep_time, bool _verbose, const char* _argv0, std::string _address, std::string _port) : resolver(this->io_service) {
	die.store(false);
	verbose = _verbose;

	index = _index;
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
}

bool webcamProvider::init() {
	socket->open(boost::asio::ip::udp::v4());

	capture.open(index);
	if(!capture.isOpened())
	{
		std::cout << "ERROR: capture is NULL on device " << index << "\n";
		capture.release();
		capture.open(++index);
		if(!capture.isOpened())
		{
			std::cout << "ERROR: capture is NULL on device " << index << "\n";
			capture.release();
		}
		return false;
	}

	capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);

	return true;
}

void webcamProvider::provide() {
	while(!die.load())
	{
		capture >> frame;
		//gray = frame.clone();
		//cvtColor(frame, gray, CV_RGB2GRAY);

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
	std::cout << "--done!" << std::endl;
	std::cout << "closing boost socket..." << std::endl;
	socket->close();
	std::cout << "--done!" << std::endl;	
}
