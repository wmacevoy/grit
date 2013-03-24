#include "CentaurThread.h"

// *****************************************************************
// CentaurThread

CentaurThread::CentaurThread()
{}

CentaurThread::~CentaurThread()
{
	if (!!m_pThread)
		m_pThread->join();
}

boost::shared_ptr<boost::thread> CentaurThread::Start()
{
	m_pThread.reset(new boost::thread(Run, this));
	return m_pThread;
}

void CentaurThread::Stop()
{
	if (!!m_pThread)
		m_pThread->interrupt();
}

void CentaurThread::Run(CentaurThread * thread)
{
	thread->DoWork();
}
