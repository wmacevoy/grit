#include <zmqWrapper.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <string.h>
#include <time.h>
#include <zmq.h>

zmqWrapper::zmqWrapper(const std::string _sockType, const std::string _ip, const int _hwm = 1, const int _linger = 25, 
				const float _timeOut = 2.0, const bool _block = false, const int _retries = 5) {
	connected = false;
	waitTime = 200;
	ip = _ip;	
	hwm = _hwm;
	linger = _linger;
	timeOut = _timeOut;
	retries = _retries;

	if(_block) {
		block = 0;
	} else {
		block = ZMQ_DONTWAIT;
	}
	

	context = zmq_ctx_new ();

	//Determine callback type here based on sockType
	if( strcmp("publish", _sockType.c_str()) == 0 ) {
		sockType = publish;
		c = &zmqWrapper::sockPublish;
		std::cout << "publish" << std::endl;
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
	//Use callback here
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

int zmqWrapper::rx(void* _data, const size_t _size) {
	//Use callback here
	int ret = 0;	
	
	if(connected) {
		ret = (this->*c)(_data, _size);
		if(ret > 0) {
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

int zmqWrapper::sockSubscribe(void* data, const size_t size = 0) {
	int ret = 0;	
	zmq_msg_t msg;	
	if(zmq_msg_init (&msg) == 0) {
		ret = zmq_recvmsg(socket, &msg, block);
		if(ret > 0) {
			memcpy(data, zmq_msg_data(&msg), zmq_msg_size(&msg));
		}
		zmq_msg_close(&msg);
	}

	return ret;
}

int zmqWrapper::sockReply(void* data, const size_t size) {
	int ret = 0;
	int8_t requestReceived = 0;
	zmq_msg_t msg;
	if(zmq_msg_init_size(&msg, size) == 0) {
		if(zmq_recv(socket, &requestReceived, sizeof(int8_t), block) == sizeof(int8_t)) {
			if(requestReceived == 1) {
				memcpy(zmq_msg_data(&msg), data, zmq_msg_size(&msg));
				ret = zmq_sendmsg(socket, &msg, block);
			}
		}
		zmq_msg_close(&msg);
	}
	
	return ret;
}

int zmqWrapper::sockRequest(void* data, const size_t size = 0) {
	int ret = 0;
	int8_t requestSent = 1;
	zmq_msg_t msg;
	if(zmq_msg_init(&msg) == 0) {
		if(zmq_send(socket, &requestSent, sizeof(int8_t), block) == sizeof(int8_t)) {
			ret = zmq_recvmsg(socket, &msg, block);
			memcpy(data, zmq_msg_data(&msg), zmq_msg_size(&msg));
		}
		zmq_msg_close(&msg);
	}

	return ret;
}

zmqWrapper::~zmqWrapper() {
	if(connected) {
		disconnect();
	}
}
