#pragma once

#include "ThreadDriverRegistry.h"

class Brain {

protected:


	Brain();
	bool started;
	ThreadDriverRegistry m_threadDriverRegistry;

public:

	static Brain& get();
	bool start();
	bool loop();
	void stop();

};
