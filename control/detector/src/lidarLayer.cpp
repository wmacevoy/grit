#include "lidarLayer.h"

//Helper functions
std::string convstr(const float t) {
	std::stringstream ftoa;
	ftoa << std::setprecision(3) << std::setw(4) << t;
	return ftoa.str();
}
//End helper functions

LidarLayer::LidarLayer() {
	context_lidar = zmq_ctx_new ();
	sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);

	hwm = 1;
	linger = 25;
	index = 0;	
	timeOut = 3.0;

	fontFace = FONT_HERSHEY_SIMPLEX;
	fontScale = 0.35;
	thickness = 1;

	t1 = 0;
	t2 = 0;
}

bool LidarLayer::setup(std::string _address, bool _verbose = false) {
	address = _address;
	verbose = _verbose;
	return true;
}

int LidarLayer::recvData() {
	int ret = 0;
	zmq_msg_t msg;
	int rc = zmq_msg_init (&msg);
	if(rc == 0)
		{
		rc = zmq_recvmsg(sub_lidar, &msg, ZMQ_DONTWAIT);		
		if(rc > 0) {
			ret = *((int*)zmq_msg_data(&msg));
			if(verbose) std::cout << "Lidar data received " << rc << " bytes, ret: " << ret << std::endl;
			t1 = time(0);
			}
		}

	t2 = time(0);
	if(t2 - t1 > timeOut) {
		zmq_close(sub_lidar);
		sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);
	
		if(zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0) == 0) {
			if(zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
				if(zmq_setsockopt(sub_lidar, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
					rc = zmq_connect(sub_lidar, address.c_str());
				}
			}
		}
	}	
	zmq_msg_close (&msg);
	return ret;
}

LidarLayer::~LidarLayer() {
	std::cout << "Quitting Lidar..." << std::endl;
	std::cout << "closing and destroying zmq..." << std::endl;
	zmq_close(sub_lidar);
	zmq_ctx_destroy(context_lidar);
	std::cout << "--done!" << std::endl;
}
