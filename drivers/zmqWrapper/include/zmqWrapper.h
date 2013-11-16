/*
 * A wrapper for zerom that handles publish, subscribe, request, and reply sockets
 *  -A socket will reconnect after 'timeOut' seconds and retry 'retries' times
 *  -Use tx for publish and reply
 *  -Use rx for subscribe and request
 *
 * Give rx a minimum msg size so it can test for a valid packet.  If recieve >= _size then a msg is valid
 * For reply sockets, give an array of pointers as the data, the request type is the index to use
 *
*/


#ifndef __zmqWrapper_h
#define __zmqWrapper_h
#include <string>

enum {publish, subscribe, request, reply};

class zmqWrapper {
	void* context;
	void* socket;

	typedef int (zmqWrapper::*callback)(void*, const size_t);
	callback c;

	int sockType;
	bool connected;
	int requestType;
	int waitTime;

	int hwm;
	int linger;
	int block;
	int retries;

	uint32_t timeStart;
	uint32_t timeEnd;
	uint32_t timeOut;

	std::string ip;
	
	void reconnect();

	int sockPublish(void*, const size_t);
	int sockSubscribe(void*, const size_t);
	int sockReply(void*, const size_t);
	int sockRequest(void*, const size_t);

public:
	zmqWrapper(const std::string _sockType, const std::string _ip, const int _hwm = 1, const int _linger = 25, 
				const float _timeOut = 2.0, const bool _block = false, const int _retries = 5);
	bool connect();
	void disconnect();
	int tx(void*, const size_t);
	int rx(void*, const size_t);
	void setRequestType(const int);
	~zmqWrapper();
};

#endif
