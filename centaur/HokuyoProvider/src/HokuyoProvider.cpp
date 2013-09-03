#include "../include/HokuyoProvider.h"
#include <iostream>
#include <string>
#include <CentaurTypes.h>
#include <libjson.h>
#include <chrono>

bool HokuyoDataSource::getData(std::vector<long> &data, std::string &error)
{
	qrk::Urg_driver lidarDevice;
	if (connectToLidar(lidarDevice, error)) {
		lidarDevice.start_measurement(qrk::Urg_driver::Distance, 1, 0);
		long time_stamp = 0;
		if (!lidarDevice.get_distance(data, &time_stamp)) {
			error = "Failed to get data from Hokuyo LIDAR scan.";
			return false;
		} 
		return true;
	}
	error = "Failed to connect to Hokuyo LIDAR.";
	return false;
}

bool HokuyoDataSource::connectToLidar(qrk::Urg_driver &lidarDevice, std::string &error) {

	std::vector<std::string> ports = qrk::Urg_driver::find_ports();
	if (ports.size() <= 0) {
		error = "Unable to find any ports that the Hokuyo LIDAR is on";
		return false;			
	}

	if(!lidarDevice.open(ports[0].c_str())) {
		error = "Unable to connect to Hokuyo LIDAR on port ";
		error += ports[0];
		error += " : ";
		error += lidarDevice.what();
		return false;		
	}
	return true;
}


volatile bool HokuyoProvider::s_shutdown = false;

HokuyoProvider::HokuyoProvider(HokuyoProviderDataSource * pDataSource)
:	m_done(false)
,	m_scans(HOKUYOPROVIDER_MAX_SCANS)
,	m_scanCurrent(0)
,	m_scansValid(0)
,	m_pDataSource(pDataSource)
,	m_workerThread(runFunction, this) 
{
	m_workerThread.detach();
	getStartThreadCV().notify_all();
}

HokuyoProvider::~HokuyoProvider()
{}

bool HokuyoProvider::run()
{

	CentaurSocketRep replySocket;

	std::string tmp("tcp://*:");
	tmp += HOKUYO_PORTNUMBER;
	if (!replySocket.open(tmp.c_str()))
	{
		std::cout << "Unable to open response port: " << tmp << std::endl;
		return false;
	}

	while (!isDone()) {

		unsigned int nScans = 0;

		int bytesRecv = replySocket.recv(&nScans, sizeof(nScans), false);
		if (bytesRecv < 0)
		{
			if (replySocket.getError() == EAGAIN)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(500));
				continue;
			}
			else
			{
				std::cout << "Error calling recv on Hokuyo Provider: " << tmp << std::endl;
				break;
			}
		}
		else if(bytesRecv == sizeof(nScans) && nScans > 0)
		{
			HokuyoData replyData;
			getScans(replyData, nScans);

			std::string response = replyData.toJSONString();
			replySocket.send(response);
		}
	}

	return true;
}

bool HokuyoProvider::getScans(HokuyoData &data, unsigned int &nScans)
{
	std::lock_guard<std::mutex> lock(m_scansMutex);
	
	if (nScans > m_scansValid)
		nScans = m_scansValid;

	for (int i = 0, scanIndex = m_scanCurrent - 1; i < (int)nScans; i++, scanIndex--)
	{
		if (scanIndex < 0)
			scanIndex = m_scans.size() - 1;
		data.m_dataArrayArray.insert(data.m_dataArrayArray.begin(), m_scans[scanIndex]);
	}
	return true;
}

std::condition_variable & HokuyoProvider::getStartThreadCV()
{
	static std::condition_variable sCV;
	return sCV;
}

void HokuyoProvider::runFunction(HokuyoProvider *pProvider)
{
	{
		std::mutex mutex;
		std::unique_lock<std::mutex> lk(mutex);
		getStartThreadCV().wait(lk);
	}

	if (!pProvider) 
		return;

	std::vector<long> scan;

	while (!HokuyoProvider::s_shutdown) {
		
		std::string error;
		pProvider->m_pDataSource->getData(scan, error);

		{
			std::lock_guard<std::mutex> lock(pProvider->m_scansMutex);

			std::vector<long> &currentScan = pProvider->m_scans[pProvider->m_scanCurrent];
			currentScan.resize(scan.size());
			memcpy(&currentScan[0], &scan[0], currentScan.size() * sizeof(currentScan[0]));

			pProvider->m_scanCurrent++;
			if (pProvider->m_scanCurrent >= HOKUYOPROVIDER_MAX_SCANS)
				pProvider->m_scanCurrent = 0;
			pProvider->m_scansValid++;
			if (pProvider->m_scansValid > HOKUYOPROVIDER_MAX_SCANS)
				pProvider->m_scansValid = HOKUYOPROVIDER_MAX_SCANS;

		}
		std::this_thread::yield();
	}
	std::cout << "THREAD: Shutting Down" << std::endl;
	pProvider->m_done = true;
}

void HokuyoProvider::setShutdown(){
	s_shutdown = true;
}

bool HokuyoProvider::isDone(){
	return m_done;
}
