#pragma once

#include <CentaurThread.h>
#include <CentaurSocket.h>

class ThreadDriverRegistry : public CentaurThread
{

public:
	ThreadDriverRegistry();
	virtual ~ThreadDriverRegistry() {}

	CentaurSocket m_socket;

protected:

	virtual void DoWork();

};
