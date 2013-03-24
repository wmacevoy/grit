#pragma once

#include <boost/thread.hpp>

class CentaurThread
{

public:

	CentaurThread();
	virtual ~CentaurThread();

	boost::shared_ptr<boost::thread> GetBoostThread() { return m_pThread; }

	boost::shared_ptr<boost::thread> Start();
	void Stop();

protected:

	volatile bool m_stopThread;

	boost::shared_ptr<boost::thread> m_pThread;

	virtual void DoWork() = 0;

	static void Run(CentaurThread * pCentaurThread);

};
