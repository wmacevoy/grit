#include "../include/CentaurSockets.h"

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

CentaurSocket::CentaurSocket(int socketType, const char * name, bool connect)
:	m_name(name)
,	m_socket(0)
,	m_type(socketType)
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

bool CentaurSocket::open(const char * addr)
{

	void * ctx = ZeroMQContext::getContext();
	if (!ctx)
	{
		fprintf(stderr, "%s: no ctx available.\n", addr);
		return false;
	}

	m_socket = zmq_socket(ctx, m_type);
	if (!m_socket)
	{
		fprintf(stderr, "%s: zmq_socket: %d '%s'", addr, errno, zmq_strerror(errno));
		close();
		return false;
	}

	if (m_connect)
	{
		fprintf(stderr, "%s connecting to %s...\n", m_name, addr);
		if (zmq_connect(m_socket, addr))
		{
			fprintf(stderr, "%s: zmq_connect: %d '%s'\n", addr, errno, zmq_strerror(errno));
			close();
			return false;
		}
	}
	else
	{
		if (!bind(addr))
			return false;
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

bool CentaurSocket::bind(const char * addr)
{
	if (m_connect)
		return false;

	fprintf(stderr, "%s binding to %s...\n", m_name, addr);
	if (zmq_bind(m_socket, addr))
	{
		fprintf(stderr, "%s: zmq_bind: %d '%s'\n", addr, errno, zmq_strerror(errno));
		close();
		return false;
	}
	return true;
}

// ***************************************************************************
// CentaurSocketPub

CentaurSocketPub::CentaurSocketPub(bool connect)
:	CentaurSocket(ZMQ_PUB, "Publisher", connect)
{}

CentaurSocketPub::~CentaurSocketPub()
{}

// ***************************************************************************
// CentaurSocketSub

CentaurSocketSub::CentaurSocketSub(bool connect)
:	CentaurSocket(ZMQ_SUB, "Subscriber", connect)
{}

CentaurSocketSub::~CentaurSocketSub()
{}

bool CentaurSocketSub::open(const char * addr, bool defaultSubscribe)
{
	if (!CentaurSocket::open(addr))
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

CentaurSocketRep::CentaurSocketRep()
:	CentaurSocket(ZMQ_REP, "Response", false)
{}

CentaurSocketRep::~CentaurSocketRep()
{}

// ***************************************************************************
// CentaurSocketReq

CentaurSocketReq::CentaurSocketReq()
:	CentaurSocket(ZMQ_REQ, "Request", true)
{}

CentaurSocketReq::~CentaurSocketReq()
{}
