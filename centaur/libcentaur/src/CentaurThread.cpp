#include "CentaurThread.h"

#include "CentaurMessage.h"

// *****************************************************************
// CentaurThread

CentaurThread::CentaurThread()
{}

CentaurThread::~CentaurThread()
{}

bool CentaurThread::Start()
{
	m_pThread.reset(new boost::thread(Run, this));
	return !!m_pThread;
}

void CentaurThread::Stop()
{
	m_stopThread = true;

	if (!!m_pThread)
		m_pThread->join();
}

void CentaurThread::Run(CentaurThread * pCentaurThread)
{
	pCentaurThread->DoWork();
}
