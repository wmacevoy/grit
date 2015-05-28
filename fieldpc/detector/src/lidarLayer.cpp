#include "lidarLayer.h"

using namespace boost::asio::ip;

//Helper functions
std::string convstr(const float t) {
	std::stringstream ftoa;
	ftoa << std::setprecision(3) << std::setw(4) << t << " CM";
	return ftoa.str();
}
//End helper functions

LidarLayer::LidarLayer() {
	index = 0;	
	timeOut = 3.0;
	MAX_SIZE = 5;

	t1 = 0;
	t2 = 0;
}

bool LidarLayer::setup(int _port, bool _verbose = false) {
	port = _port;
	verbose = _verbose;
	
	my_socket = new udp::socket(my_io_service, udp::endpoint(udp::v4(), port));
    my_socket->non_blocking(true);
	return true;
}

int LidarLayer::recvData() {
	int ret = 0;
	boost::system::error_code ec;

  	buff.resize(MAX_SIZE);
	size_t length = my_socket->receive_from(boost::asio::buffer(buff, MAX_SIZE), sender_endpoint, 0, ec);
    // if(verbose) std::cout << "Lidar recv length = " << length << std::endl;
	if(length > 0)
		{
		for(int i=0; i<buff.size(); ++i)
			{
			if(isdigit(buff[i]))
				{
				ret*=10;
				ret+= (buff[i]-'0');
				}
			}
		if(verbose) std::cout << "ret: " << ret << std::endl;
		}
	
	return ret;
}

LidarLayer::~LidarLayer() {
	std::cout << "Quitting Lidar..." << std::endl;
	std::cout << "closing and destroying zmqz..." << std::endl;
	my_socket->close();
	free(my_socket);
	std::cout << "--done!" << std::endl;
}
