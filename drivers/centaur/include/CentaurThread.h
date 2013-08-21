#pragma once

#include <boost/thread.hpp>

class CentaurThread
{

public:

	CentaurThread();
	virtual ~CentaurThread();

	boost::shared_ptr<boost::thread> getBoostThread() { return m_pThread; }

	virtual bool start();
	virtual void stop();

protected:

	volatile bool m_stopThread;

	boost::shared_ptr<boost::thread> m_pThread;

	virtual void doWork() = 0;

	static void run(CentaurThread * pCentaurThread);

};
