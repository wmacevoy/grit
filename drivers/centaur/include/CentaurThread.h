#pragma once

#include <memory>
#include <thread>

class CentaurThread
{

public:

	CentaurThread();
	virtual ~CentaurThread();

	std::shared_ptr<std::thread> getBoostThread() { return m_pThread; }
	std::shared_ptr<std::thread> getThread() { return m_pThread; }

	virtual bool start();
	virtual void stop();

protected:

	volatile bool m_stopThread;

	std::shared_ptr<std::thread> m_pThread;

	virtual void doWork() = 0;

	static void run(CentaurThread * pCentaurThread);

};
