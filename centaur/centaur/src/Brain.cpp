#include "Brain.h"

Brain::Brain()
:	started(false)
{}

Brain& Brain::get()
{
	static Brain brain;
	return brain;
}


bool Brain::start()
{
	if(started)
		return true;
	if (!m_threadDriverRegistry.Start())
		return false;
	started = true;


	return true;
}

void Brain::stop()
{
	if(!started)
		return;
	m_threadDriverRegistry.Stop();
	started = false;
}

bool Brain::loop()
{
	if(!started && !start())
		return false;
	//TODO:put the brains in here!
	return true;
}
