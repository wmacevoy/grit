#include <zmqWrapper.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <string.h>
#include <time.h>
#include <zmq.h>

zmqWrapper::zmqWrapper(const std::string _sockType, const std::string _ip) {
	zmqWrapper(_sockType, _ip, 1, 25, 2.0, false, 5);
}

zmqWrapper::zmqWrapper(const std::string _sockType, const std::string _ip, const int _hwm, const int _linger, 
				const float _timeOut, const bool _block, const int _retries) {
	connected = false;
	waitTime = 200;
	ip = _ip;	
	hwm = _hwm;
	linger = _linger;
	timeOut = _timeOut;
	block = _block ? 0:ZMQ_DONTWAIT;
	retries = _retries;	

	context = zmq_ctx_new ();

	//Determine callback type here based on sockType
	if( strcmp("publish", _sockType.c_str()) == 0 ) {
		sockType = publish;
		c = &zmqWrapper::sockPublish;
	}
	else if( strcmp("subscribe", _sockType.c_str()) == 0 ) {
		sockType = subscribe;
		c = &zmqWrapper::sockSubscribe;
	}
	else if( strcmp("request", _sockType.c_str()) == 0 ) {
		sockType = request;
		c = &zmqWrapper::sockRequest;
	}
	else if( strcmp("reply", _sockType.c_str()) == 0 ) {
		sockType = reply;
		c = &zmqWrapper::sockReply;
	}
}

bool zmqWrapper::connect() {
	while(!connected && retries--) {
		switch(sockType) {
		case publish:
			socket = zmq_socket(context, ZMQ_PUB);
			if( zmq_setsockopt(socket, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0 ) {
				if( zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
					if( zmq_bind(socket, ip.c_str()) == 0) {
						connected = true;
					} } }
			break;
		case subscribe:
			socket = zmq_socket(context, ZMQ_SUB);
			if( zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0) == 0 ) {
				if( zmq_setsockopt(socket, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0 ) {
					if( zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
						if( zmq_connect(socket, ip.c_str()) == 0) {
							connected = true;
					} } } }
			break;
		case request:
			socket = zmq_socket(context, ZMQ_REQ);
			if( zmq_setsockopt(socket, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0 ) {
				if( zmq_setsockopt(socket, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0 ) {
					if( zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
						if( zmq_bind(socket, ip.c_str()) == 0) {
							connected = true;
						} } } }			
			break;
		case reply:
			socket = zmq_socket(context, ZMQ_REP);
			if( zmq_setsockopt(socket, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0 ) {
				if( zmq_setsockopt(socket, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0 ) {
					if( zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
						if( zmq_bind(socket, ip.c_str()) == 0) {
							connected = true;
						} } } }
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
	}

	timeStart = time(0);
	return connected;
}

void zmqWrapper::reconnect() {
	zmq_close(socket);
	if(connect()) {
		std::cout << "reconnected" << std::endl;
	}
}

void zmqWrapper::disconnect() {
	std::cout << "disconnecting" << std::endl;
	connected = false;
	zmq_close(socket);
	zmq_ctx_destroy(context);
}

int zmqWrapper::tx(void* _data, const size_t _size) {
	int ret = 0;

	if(connected) {
		ret = (this->*c)(_data, _size);
		if(ret == _size) {
			timeStart = time(0);
		}
	}
	timeEnd = time(0);
	if(timeEnd - timeStart >= timeOut) {
		connected = false;
		reconnect();
		connect();
	}

	return ret;
}

int zmqWrapper::rx(void* _data, const size_t _size = 0) {
	int ret = 0;	
	
	if(connected) {
		ret = (this->*c)(_data, _size);
		if(ret == _size) {
			timeStart = time(0);		
		}
	}
	timeEnd = time(0);
	if(timeEnd - timeStart >= timeOut) {
		connected = false;
		reconnect();
		connect();
	}

	return ret;
}

int zmqWrapper::sockPublish(void* data, const size_t size) {
	int ret = 0;		
	zmq_msg_t msg;
	if(zmq_msg_init_size(&msg, size) == 0) {
		memcpy(zmq_msg_data(&msg), data, zmq_msg_size(&msg));
		ret = zmq_sendmsg(socket, &msg, block);
		zmq_msg_close(&msg);
	}
	return ret;
}

int zmqWrapper::sockSubscribe(void* data, const size_t size) {
	int ret = 0;	
	zmq_msg_t msg;	
	if(zmq_msg_init(&msg) == 0) {
		ret = zmq_recvmsg(socket, &msg, block);
		if(ret > 0) {
			memcpy(data, zmq_msg_data(&msg), zmq_msg_size(&msg));
		}
		zmq_msg_close(&msg);
	}

	return ret;
}

//Return 0+ on success (valid request type) or -1 on error
int zmqWrapper::getReq() {
	requestType = -1;
	requested = false;
	if(zmq_recv(socket, &requestType, sizeof(requestType), block) == sizeof(requestType)) {
		requested = true;
	}
	return requestType;
}

int zmqWrapper::sockReply(void* data, const size_t size) {
	int ret = 0;

	zmq_msg_t msg;
	if(requested) {
		if(zmq_msg_init_size(&msg, size) == 0) {
			memcpy(zmq_msg_data(&msg), data, zmq_msg_size(&msg));
			ret = zmq_sendmsg(socket, &msg, block);
			zmq_msg_close(&msg);
		}
	}
	
	return ret;
}

//Return the requestType on success or -1 on error
int zmqWrapper::sendReq(const int _requestType) {
	requestType = _requestType;
	if(zmq_send(socket, &requestType, sizeof(requestType), block) == sizeof(requestType)) {
		requested = true;
		return requestType;
	}
	return -1;
}

int zmqWrapper::sockRequest(void* data, const size_t size) {
	int ret = 0;
	zmq_msg_t msg;
	if(requested) {
		if(zmq_msg_init(&msg) == 0) {
			ret = zmq_recvmsg(socket, &msg, block);
			memcpy(data, zmq_msg_data(&msg), zmq_msg_size(&msg));
			zmq_msg_close(&msg);
		}
		requested = false;
	}

	return ret;
}

zmqWrapper::~zmqWrapper() {
	if(connected) {
		disconnect();
	}
}
