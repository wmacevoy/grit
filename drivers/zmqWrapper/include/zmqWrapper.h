/*
 * A wrapper for zerom that handles publish, subscribe, request, and reply sockets
 *  -A socket will reconnect after 'timeOut' seconds and retry 'retries' times
 *  -Use tx for publish
 *  -Use rx for subscribe
 *  -Use getReq and tx for reply
 *  -Use sendReq and rx for request
 *
 * Give rx a minimum msg size so it can test for a valid packet.  If recieve >= _size then a msg is valid
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
	bool requested;

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
	int sockSubscribe(void*, const size_t = 0);
	int sockReply(void*, const size_t);
	int sockRequest(void*, const size_t = 0);

public:
	zmqWrapper(const std::string, const std::string);
	zmqWrapper(const std::string, const std::string, const int, const int, const float, const bool, const int);
	bool connect();
	void disconnect();
	int tx(void*, const size_t);
	int rx(void*, const size_t);
	int sendReq(const int);
	int getReq();
	~zmqWrapper();
};

#endif
