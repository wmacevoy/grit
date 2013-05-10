#pragma once

#include <CentaurThread.h>
#include <CentaurSockets.h>

class ThreadDriverRegistry : public CentaurThread
{

public:
	ThreadDriverRegistry();
	virtual ~ThreadDriverRegistry() {}

	CentaurSocketRep m_socket;

	virtual bool Start();

protected:

	virtual void DoWork();

};
