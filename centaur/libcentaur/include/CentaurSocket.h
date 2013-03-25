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

	bool Open();
	bool Close();

};
