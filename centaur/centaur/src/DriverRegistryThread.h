#pragma once

#include <CentaurThread.h>

class DriverRegistryThread : public CentaurThread
{

public:
	DriverRegistryThread() {}
	virtual ~DriverRegistryThread() {}

protected:
	virtual void * DoWork();
};
