#pragma once

#include "ThreadDriverRegistry.h"

class Centaur {

protected:

	Centaur();

	ThreadDriverRegistry m_threadDriverRegistry;

public:

	static Centaur& Get();

	bool Start();
	bool Stop();

};
