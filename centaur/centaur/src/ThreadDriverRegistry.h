#pragma once

#include <CentaurThread.h>

class ThreadDriverRegistry : public CentaurThread
{

public:
	ThreadDriverRegistry();
	virtual ~ThreadDriverRegistry() {}

protected:
	virtual void DoWork();

};
