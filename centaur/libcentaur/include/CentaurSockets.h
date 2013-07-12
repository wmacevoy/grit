#pragma once

#include <vector>
#include <cstring>

#include <zmq.h>
#include "CentaurTypes.h"

#define ZEROMQ_CONTEXT_NUMBER_IO_THREADS		1
#define ZEROMQ_MSG_BLOCK_SIZE					1024 // not sure what is optimal

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
	bool m_connect;

public:

	CentaurSocket(int socketType, const char * name, bool connect = true);
	virtual ~CentaurSocket();

	int getError(const char **error = 0);

	bool open(const char * addr);
	bool close();

	bool bind(const char * addr);

	template <class T>
	int send(T * pData, int nData, bool block = true);

	template <class T, unsigned int staticCount, class Alloc>
	int send(CM_Array<T, staticCount, Alloc> &data, bool block = true)
	{
		return send(data.getData(), data.getSize());
	}

	template <unsigned int staticCount, class Alloc>
	int recv(CM_Array<char, staticCount, Alloc> &data, bool block = false)
	{
		int flags = block ? 0 : ZMQ_NOBLOCK;

		int nData = 0;

		int rcvmore = 1;
		size_t sz = sizeof(rcvmore);

		while (rcvmore)
		{
			data.setCount(data.getCount() + ZEROMQ_MSG_BLOCK_SIZE);
			char * pData = data.getData() + (data.getCount() - ZEROMQ_MSG_BLOCK_SIZE);

		    int rc = zmq_recv(m_socket, pData, ZEROMQ_MSG_BLOCK_SIZE, flags);

		    if (rc < 0)
		    {
		    	data.setCount(data.getCount() - ZEROMQ_MSG_BLOCK_SIZE);
		    	return rc;
		    }

		    nData += rc;

		    if (rc < (int)ZEROMQ_MSG_BLOCK_SIZE)
		    	data.setCount(nData);

		    rc = zmq_getsockopt(m_socket, ZMQ_RCVMORE, &rcvmore, &sz);
		    if (rc)
		    {
		    	data.setCount(nData);
		    	return rc;
		    }
		}

		data.setCount(nData);
		return nData;
	}

};

class CentaurSocketPub : public CentaurSocket{

public:

	CentaurSocketPub(bool connect = false);
	virtual ~CentaurSocketPub();
};

class CentaurSocketSub : public CentaurSocket{

public:

	CentaurSocketSub(bool connect = true);
	virtual ~CentaurSocketSub();

	bool open(const char * addr, bool defaultSubscribe = true);
	int subscribe(const void * pFilter, size_t nFilter);
	int unsubscribe(const void * pFilter, size_t nFilter);
};

class CentaurSocketRep : public CentaurSocket{

public:

	CentaurSocketRep();
	
	virtual ~CentaurSocketRep();
	void WaitForData(const char * data);
};

class CentaurSocketReq : public CentaurSocket{

public:

	CentaurSocketReq();
	virtual ~CentaurSocketReq();
};
