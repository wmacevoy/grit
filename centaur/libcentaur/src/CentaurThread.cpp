#include "CentaurThread.h"

#include "CentaurMessage.h"

// *****************************************************************
// CentaurThread

CentaurThread::CentaurThread()
{}

CentaurThread::~CentaurThread()
{}

bool CentaurThread::start()
{
	m_pThread.reset(new boost::thread(run, this));
	return !!m_pThread;
}

void CentaurThread::stop()
{
	m_stopThread = true;

	if (!!m_pThread)
		m_pThread->join();
}

void CentaurThread::run(CentaurThread * pCentaurThread)
{
	pCentaurThread->doWork();
}
