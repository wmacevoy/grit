#include "HokuyoProvider.h"
#include <iostream>
#include <string>
#include <CentaurTypes.h>
#include <bson.h>
#include <findUrgPorts.h>
#include <chrono>
#include <now.h>


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
	std::string tmp("tcp://*:");
	tmp += HOKUYO_PORTNUMBER;
	if(!m_replySocket.open(tmp.c_str()))
		return false;
		
	m_workerThread.detach();
	return true;
}

bool HokuyoProvider::connectToLidar(qrk::UrgDevice &lidarDevice, std::string &error) {
	std::vector<std::string> ports;
	qrk::findUrgPorts(ports);
	if(ports.size() <= 0){
		error = "Unable to find any ports that the lidar device is on";
		return false;			
	}

	if(!lidarDevice.connect(ports[0].c_str())) {
		error = "Unable to connect to lidar device on port ";
		error += ports[0];
		error += " : ";
		error += lidarDevice.what();
		return false;		
	}
	
	return true;
}


void HokuyoProvider::runFunction(HokuyoProvider *pProvider)
{
	while(!HokuyoProvider::s_shutdown){
		unsigned int nScans = 0;
		int recvCnt = pProvider->m_replySocket.recv(&nScans, 1);
		if(recvCnt > 0 && nScans > 0){
			std::cout << "Received a request for data\n";
			qrk::UrgDevice lidarDevice;
			HokuyoData replyData;
			
			if(connectToLidar(lidarDevice, replyData.m_error)) {
				for(unsigned int i = 0; i < nScans; i++){
					std::vector<long> data;
					int n = lidarDevice.capture(data);
					
					if(n <= 0) {
						char buffer[64];
						sprintf(buffer, "Failed to get data from scan %i\n", i);
						replyData.m_error = buffer;
						break;
					} else {
						replyData.m_dataArrayArray.push_back(data);
					}		
				}
			}
			replyData.m_timestamp = now();
			bson::bo response = replyData.toBSON();
			
			pProvider->m_replySocket.send(response.objdata(), response.objsize());
		}
		std::chrono::milliseconds dura( 250 );
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
