#pragma once
#include <HokuyoProviderRequest.hpp>
#include <CentaurSockets.h>
#include <UrgDevice.h>
#include <thread>


class HokuyoProvider {

private:

	CentaurSocketRep		m_replySocket;
	std::thread				m_workerThread;  
	static volatile bool	s_shutdown;
	static volatile bool	s_isDone;
	

public:
	HokuyoProvider();
	virtual ~HokuyoProvider();
	bool start();
	static void setShutdown();
	static bool isDone();
	
protected:

	static bool connectToLidar(qrk::UrgDevice &lidarDevice, std::string &error);
	static void runFunction(HokuyoProvider *pProvider);
		
};