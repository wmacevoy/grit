#include "CentaurSockets.h"

// ***************************************************************************
// ZeroMQContext

ZeroMQContext::ZeroMQContext()
:	m_ctx(0)
{
	m_ctx = zmq_init(ZEROMQ_CONTEXT_NUMBER_IO_THREADS);
	if (!m_ctx)
		fprintf(stderr, "zmq_init: %d '%s'", errno, zmq_strerror(errno));
}

ZeroMQContext& ZeroMQContext::GetInstance()
{
	static ZeroMQContext s_instance;
	return s_instance;
}

ZeroMQContext::~ZeroMQContext()
{
	if (m_ctx)
		zmq_term(m_ctx);
	m_ctx = 0;
}

void * ZeroMQContext::getContext()
{
	return GetInstance().m_ctx;
}

// ***************************************************************************
// CentaurSocket

CentaurSocket::CentaurSocket(int socketType, const char * name, const char * addr, bool connect)
:	m_name(name)
,	m_socket(0)
,	m_type(socketType)
,	m_addr(addr)
,	m_connect(connect)
{}

CentaurSocket::~CentaurSocket()
{
	close();
}

int CentaurSocket::getError(const char ** error)
{
	int errorNumber = errno;
	if (error)
		*error = zmq_strerror(errorNumber);
	return errorNumber;
}

bool CentaurSocket::open()
{

	fprintf(stderr, "%s %s to %s...\n", m_name, m_connect ? "connecting" : "binding", m_addr);

	void * ctx = ZeroMQContext::getContext();
	if (!ctx)
	{
		fprintf(stderr, "%s: no ctx available.", m_addr);
		return false;
	}

	m_socket = zmq_socket(ctx, m_type);
	if (!m_socket)
	{
		fprintf(stderr, "%s: zmq_socket: %d '%s'", m_addr, errno, zmq_strerror(errno));
		close();
		return false;
	}

	if (m_connect)
	{
		if (zmq_connect(m_socket, m_addr))
		{
			fprintf(stderr, "%s: zmq_connect: %d '%s'", m_addr, errno, zmq_strerror(errno));
			close();
			return false;
		}
	}
	else
	{
		if (zmq_bind(m_socket, m_addr))
		{
			fprintf(stderr, "%s: zmq_bind: %d '%s'", m_addr, errno, zmq_strerror(errno));
			close();
			return false;
		}
	}

	return true;
}

bool CentaurSocket::close()
{
	// TODO: Could possibly check for error conditions here, but I'm lazy

	if (m_socket)
		zmq_close(m_socket);
	m_socket = 0;

	return true;
}

int CentaurSocket::send(const char * pData, int nData, bool block)
{
	const char * pDataEnd = pData + nData;

	int flags = block ? 0 : ZMQ_NOBLOCK;

	nData = 0;

	while (pDataEnd - pData > ZEROMQ_MSG_BLOCK_SIZE)
	{
		int rc = zmq_send(m_socket, pData, ZEROMQ_MSG_BLOCK_SIZE, flags | ZMQ_SNDMORE);
		if (rc != ZEROMQ_MSG_BLOCK_SIZE)
			return rc;
		nData += rc;
		pData += ZEROMQ_MSG_BLOCK_SIZE;
	}

	int rc = zmq_send(m_socket, pData, pDataEnd - pData, flags);
	if (rc != pDataEnd - pData)
		return rc;
	nData += rc;
	return nData;
}


// ***************************************************************************
// CentaurSocketPub

CentaurSocketPub::CentaurSocketPub(const char *addr, bool connect)
:	CentaurSocket(ZMQ_PUB, "Publisher", addr, connect)
{}

CentaurSocketPub::~CentaurSocketPub()
{}

// ***************************************************************************
// CentaurSocketSub

CentaurSocketSub::CentaurSocketSub(const char *addr, bool connect)
:	CentaurSocket(ZMQ_SUB, "Subscriber", addr, connect)
{}

CentaurSocketSub::~CentaurSocketSub()
{}

bool CentaurSocketSub::open(bool defaultSubscribe)
{
	if (!CentaurSocket::open())
		return false;

	if (defaultSubscribe)
		return subscribe("", 0) == 0;

	return true;
}

int CentaurSocketSub::subscribe(const void * pFilter, size_t nFilter)
{
	return zmq_setsockopt(m_socket, ZMQ_SUBSCRIBE, pFilter, nFilter);
}

int CentaurSocketSub::unsubscribe(const void * pFilter, size_t nFilter)
{
	return zmq_setsockopt(m_socket, ZMQ_UNSUBSCRIBE, pFilter, nFilter);
}

// ***************************************************************************
// CentaurSocketRep

CentaurSocketRep::CentaurSocketRep(const char *addr)
:	CentaurSocket(ZMQ_REP, "Response", addr, false)
{}

CentaurSocketRep::~CentaurSocketRep()
{}

// ***************************************************************************
// CentaurSocketReq

CentaurSocketReq::CentaurSocketReq(const char *addr)
:	CentaurSocket(ZMQ_REQ, "Request", addr, true)
{}

CentaurSocketReq::~CentaurSocketReq()
{}
