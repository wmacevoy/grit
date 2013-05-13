#pragma once

#include <zmq.h>

class CentaurSocket {

protected:

	void * m_ctx;
	void * m_socket;

	int m_type;

	const char * m_addr;

	int m_nIOThreads;

public:

	CentaurSocket(int socketType, const char * addr, int nIOThreads = 1);
	virtual ~CentaurSocket();

	bool Send(void * data);
	bool Open();
	bool Close();

};

class CentaurSocketPub : public CentaurSocket{

public:

	CentaurSocketPub(const char *addr, int nIOThreads);
	void publish(void *data);
	virtual ~CentaurSocketPub();
};

class CentaurSocketSub : public CentaurSocket{

public:

	CentaurSocketSub(const char *addr, int nIOThreads);
	void subscribe();
	virtual ~CentaurSocketSub();
};

class CentaurSocketRep : public CentaurSocket{

public:

	CentaurSocketRep(const char *addr, int nIOThreads);
	virtual ~CentaurSocketRep();
};
