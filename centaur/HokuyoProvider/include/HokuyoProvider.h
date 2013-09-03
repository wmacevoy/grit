#pragma once

#include <HokuyoProviderRequest.hpp>
#include <CentaurSockets.h>
#include <Urg_driver.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#define HOKUYOPROVIDER_MAX_SCANS		1024

struct HokuyoProviderDataSource
{
	virtual bool getData(std::vector<long> &data, std::string &error) = 0;
};

typedef std::shared_ptr<HokuyoProviderDataSource> HokuyoProviderDataSourcePtr;

struct HokuyoDataSource
:	public HokuyoProviderDataSource
{

protected:
	static bool connectToLidar(qrk::Urg_driver &lidarDevice, std::string &error);

public:
	virtual bool getData(vector<long> &data, std::string &error);
};

class HokuyoProvider {

private:

	std::thread				m_workerThread;

	static volatile bool	s_shutdown;

	volatile bool	m_done;

	HokuyoProviderDataSourcePtr m_pDataSource;

	std::mutex m_scansMutex;
	
	unsigned int m_scanCurrent;
	unsigned int m_scansValid;
	std::vector<std::vector<long>> m_scans;

public:
	HokuyoProvider(HokuyoProviderDataSource * pDataSource);
	virtual ~HokuyoProvider();
	
	bool run();
	bool getScans(HokuyoData &data, unsigned int &nScans);

	static void runFunction(HokuyoProvider *pProvider);

	static std::condition_variable & getStartThreadCV();

	static void setShutdown();
	bool isDone();	
};
