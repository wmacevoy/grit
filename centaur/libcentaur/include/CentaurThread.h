#pragma once

#include <boost/thread.hpp>

class CentaurThread
{

public:

	CentaurThread();
	virtual ~CentaurThread();

	boost::shared_ptr<boost::thread> GetThread() { return m_pThread; }

	virtual boost::shared_ptr<boost::thread> Start();
	virtual void Stop();

protected:

	boost::shared_ptr<boost::thread> m_pThread;

	virtual void * DoWork() = 0;

	static void Run(CentaurThread * thread);

};
