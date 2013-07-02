#include "../include/HokuyoProvider.h"
#include <chrono>
#include <iostream>
#include <string>

volatile bool HokuyoProvider::s_shutdown = false;
volatile bool HokuyoProvider::s_isDone =false;

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
		m_workerThread.detach();
}

void HokuyoProvider::runFunction(HokuyoProvider *pProvider)
{
	while(!HokuyoProvider::s_shutdown){
		// Do stuff with the socket.
		std::cout << "THREAD : Nothing to do, sleeping for 2 seconds\n";
		std::chrono::milliseconds dura( 2000 );
		std::this_thread::sleep_for( dura );
	}
	std::cout << "THREAD: Shutting Down\n";
	s_isDone = true;
}

void HokuyoProvider::setShutdown(){
	s_shutdown = true;
}

bool HokuyoProvider::isDone(){
	return s_isDone;
}
