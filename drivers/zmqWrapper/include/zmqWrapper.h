/*
 * A wrapper for zerom that handles publish, subscribe, request, and reply sockets
 *  -A socket will reconnect after 'timeOut' seconds and retry 'retries' times
 * Use tx for publish and reply
 * Use rx for subscribe and request
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
	int waitTime;

	int hwm;
	int linger;
	int block;
	int retries;

	float timeStart;
	float timeEnd;
	float timeOut;

	std::string ip;
	
	void reconnect();

	int sockPublish(void*, const size_t);
	int sockSubscribe(void*, const size_t);
	int sockReply(void*, const size_t);
	int sockRequest(void*, const size_t);

public:
	zmqWrapper(const std::string, const std::string, const int, const int, const float, const bool, const int);
	bool connect();
	void disconnect();
	int tx(void*, const size_t);
	int rx(void*, const size_t);
	~zmqWrapper();
};

#endif
