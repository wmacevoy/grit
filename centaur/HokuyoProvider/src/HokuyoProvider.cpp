#include "../include/HokuyoProvider.h"
#include <chrono>
#include <iostream>
#include <string>
#include <CentaurTypes.h>
#include <bson.h>

volatile bool HokuyoProvider::s_shutdown = false;
volatile bool HokuyoProvider::s_isDone =false;

HokuyoProvider::HokuyoProvider()
:	m_replySocket()
,	m_workerThread(runFunction, this)
{}

HokuyoProvider::~HokuyoProvider()
{}

bool HokuyoProvider::start()
{
	std::vector<std::string> ports;
	qrk::findUrgPorts(ports);
	if(ports.size() <= 0)
		return false;

	if(!m_lidarDevice.connect(ports[0].c_str()))
		return false;

	std::string tmp("tcp://*:");
	tmp += HOKUYO_PORTNUMBER;
	if(!m_replySocket.open(tmp.c_str()))
		return false;
		
	m_workerThread.detach();
	return true;
}

void HokuyoProvider::runFunction(HokuyoProvider *pProvider)
{
	while(!HokuyoProvider::s_shutdown){
		CM_Array<char, 1> dataFromRequest;
		int recvCnt = pProvider->m_replySocket.recv(dataFromRequest, true);
		if(recvCnt > 0){
			std::vector<long> data;
			int n = pProvider->m_lidarDevice.capture(data);
			
			bson::BSONObjBuilder objBuilder;
			bson::BSONArrayBuilder arrayBuilder;
			if(n > 0){
				for(int i = 0; i < data.size(); i++){
					int val = data[i];
					arrayBuilder.append(val);
				}
				objBuilder.append("data", arrayBuilder.arr());
				objBuilder.append("error", "");
			} else {
				objBuilder.append("data", arrayBuilder.arr());
				objBuilder.append("error", "Unable to read data from the lidar device.");
			}
			
			std::string response = objBuilder.obj().jsonString();
			pProvider->m_replySocket.send(response.data(), (int)response.size());
		}
		std::chrono::milliseconds dura( 100 );
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
