#pragma once
#include <UrgDevice.h>
#include <findUrgPorts.h>
#include <CentaurSockets.h>
#include <thread>

#define HOKUYO_PORTNUMBER	"31777"

class HokuyoProvider {

private:

	CentaurSocketRep		m_replySocket;
	qrk::UrgDevice			m_lidarDevice;
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

	static void runFunction(HokuyoProvider *pProvider);
		
};
