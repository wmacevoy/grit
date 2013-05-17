#pragma once

#include <vector>
#include <cstring>

#define ZEROMQ_CONTEXT_NUMBER_IO_THREADS		1
#define ZEROMQ_MSG_BLOCK_SIZE					32 // not sure what is optimal

#define CENTAURSOCKET_RECV_NOT_ENOUGH_SPACE		-2

class ZeroMQContext;

class ZeroMQContext {
	
protected:

	void * m_ctx;
	
	ZeroMQContext();
	
	static ZeroMQContext& GetInstance();

public:
	
	~ZeroMQContext();
	
	static void * getContext();
};

class CentaurSocket {

protected:

	const char * m_name;
	void * m_socket;
	int m_type;
	const char * m_addr;
	bool m_connect;

public:

	CentaurSocket(int socketType, const char * name, const char * addr, bool connect = true);
	virtual ~CentaurSocket();

	int getError(const char **error = 0);

	bool open();
	bool close();

	int send(const char * pData, int nData, bool block = true);
	int send(std::vector<char> &data, bool block = true);

	int recv(std::vector<char> &data, bool block = false);

};

class CentaurSocketPub : public CentaurSocket{

public:

	CentaurSocketPub(const char *addr, bool connect = false);
	virtual ~CentaurSocketPub();
};

class CentaurSocketSub : public CentaurSocket{

public:

	CentaurSocketSub(const char *addr, bool connect = true);
	virtual ~CentaurSocketSub();

	bool open(bool defaultSubscribe = true);
	int subscribe(const void * pFilter, size_t nFilter);
	int unsubscribe(const void * pFilter, size_t nFilter);
};

class CentaurSocketRep : public CentaurSocket{

public:

	CentaurSocketRep(const char *addr);
	virtual ~CentaurSocketRep();
};

class CentaurSocketReq : public CentaurSocket{

public:

	CentaurSocketReq(const char *addr);
	virtual ~CentaurSocketReq();
};
