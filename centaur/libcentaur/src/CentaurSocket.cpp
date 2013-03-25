#include "CentaurSocket.h"

CentaurSocket::CentaurSocket(int socketType, const char * addr, int nIOThreads)
:	m_ctx(0)
,	m_socket(0)
,	m_type(socketType)
,	m_addr(addr)
,	m_nIOThreads(nIOThreads)
{}

bool CentaurSocket::Open()
{

	printf("Opening %s...\n", m_addr);

	m_ctx = zmq_init(m_nIOThreads);
	if (!m_ctx)
	{
		printf("%s: zmq_init: %d '%s'", m_addr, errno, zmq_strerror(errno));
		return false;
	}

	m_socket = zmq_socket(m_ctx, m_type);
	if (!m_socket)
	{
		printf("%s: zmq_socket: %d '%s'", m_addr, errno, zmq_strerror(errno));
		Close();
		return false;
	}

	if (zmq_bind(m_socket, m_addr))
	{
		printf("%s: zmq_bind: %d '%s'", m_addr, errno, zmq_strerror(errno));
		Close();
		return false;
	}

	return true;
}

bool CentaurSocket::Close()
{
	// TODO: Could possibly check for error conditions here, but I'm lazy

	if (m_socket)
		zmq_close(m_socket);
	m_socket = 0;

	if (m_ctx)
		zmq_term(m_ctx);
	m_ctx = 0;

	return true;
}
