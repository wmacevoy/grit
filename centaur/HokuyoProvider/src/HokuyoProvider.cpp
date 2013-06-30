#include "../include/HokuyoProvider.h"
#include <chrono>
#include <string>


void sigproc(int);
void quitproc(int);

volatile bool HokuyoProvider::s_shutdown = false;

HokuyoProvider::HokuyoProvider()
:	m_replySocket()
,	m_workerThread(runFunction, this)
{}

HokuyoProvider::~HokuyoProvider()
{}

void HokuyoProvider::start()
{
	std::string tmp("tcp://*:");
	tmp += HOKUYO_PORTNUMBER;
	if(m_replySocket.open(tmp.c_str()))
		m_workerThread.join();
}

void HokuyoProvider::runFunction(HokuyoProvider *pProvider)
{
	while(!HokuyoProvider::s_shutdown){
		// Do stuff with the socket.
		std::chrono::milliseconds dura( 2000 );
		std::this_thread::sleep_for( dura );
	}
}

void HokuyoProvider::setShutdown(){
	s_shutdown = true;
}
