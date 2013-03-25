#pragma once

#include <CentaurThread.h>
#include <CentaurSocket.h>

class ThreadDriverRegistry : public CentaurThread
{

public:
	ThreadDriverRegistry();
	virtual ~ThreadDriverRegistry() {}

	CentaurSocket m_socket;

	virtual bool Start();

protected:

	virtual void DoWork();

};
