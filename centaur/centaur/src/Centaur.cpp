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
	if (!m_threadDriverRegistry.m_socket.Open())
		return false;

	m_threadDriverRegistry.Start();

	return true;
}

bool Centaur::Stop()
{
	m_threadDriverRegistry.Stop();

	return true;
}
