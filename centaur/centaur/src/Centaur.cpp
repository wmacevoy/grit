#include "Centaur.h"

Centaur::Centaur()
{}

Centaur& Centaur::Get()
{
	static Centaur centaur;
	return centaur;
}


bool Centaur::Start()
{
	if (!m_threadDriverRegistry.Start())
		return false;

	return true;
}

bool Centaur::Stop()
{
	m_threadDriverRegistry.Stop();

	return true;
}
