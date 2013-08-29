#include "../include/HokuyoProvider.h"
#include <iostream>
#include <string>
#include <CentaurTypes.h>
#include <bson.h>
#include <chrono>


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

bool HokuyoProvider::connectToLidar(qrk::Urg_driver &lidarDevice, std::string &error) {
	std::vector<std::string> ports = qrk::Urg_driver::find_ports();
	if(ports.size() <= 0){
		error = "Unable to find any ports that the lidar device is on";
		return false;			
	}

	if(!lidarDevice.open(ports[0].c_str())) {
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
			//std::cout << "Received a request for data\n";
			qrk::Urg_driver lidarDevice;
			HokuyoData replyData;
			
			if(connectToLidar(lidarDevice, replyData.m_error)) {
				lidarDevice.start_measurement(qrk::Urg_driver::Distance, nScans, 0);
				for (int i = 0; i < nScans; i++) {
					vector<long> data;
					long time_stamp = 0;

					if (!lidarDevice.get_distance(data, &time_stamp)) {
						char buffer[64];
						sprintf(buffer, "Failed to get data from scan %i\n", i);
						replyData.m_error = buffer;
					} else {
						replyData.m_dataArrayArray.push_back(data);
					}
				}
				
			}
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
