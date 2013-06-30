#pragma once
#include <UrgDevice.h>
#include <findUrgPorts.h>
#include <CentaurSockets.h>
#include <thread>

#define HOKUYO_PORTNUMBER	"31777"

class HokuyoProvider {

private:

	CentaurSocketRep		m_replySocket;
	std::thread				m_workerThread;  
	static volatile bool	s_shutdown;

public:
	HokuyoProvider();
	virtual ~HokuyoProvider();
	void start();
	static void setShutdown();
	
protected:

	static void runFunction(HokuyoProvider *pProvider);
		
};
