#define CATCH_CONFIG_RUNNER

#include "../include/HokuyoProvider.h"

#include <thread>

#include "../../../libs/catch/include/catch.hpp" // This needs to be last!!

int main(int argc, char * const argv[]) {

#if !_UNICODE
	int retVal = Catch::Session().run(argc, argv);
#else
	std::vector<std::string> strings(argc);

	for (int i = 0; i < argc; i++)
		std::string &string = strings[i];

	std::vector<const char *> stringPtrArray(argc);

	for (int i = 0; i < argc; i++)
	{
		const char * & stringPtr = stringPtrArray[i];
		stringPtr = &strings[i];
	}

	int retVal = Catch::Session().run(argc, (char * const *) stringPtrArray.GetData());
#endif

#if _DEBUG && _WIN32
	system("pause");
#endif
	return retVal;
}

struct DummyDataSource
:	public HokuyoProviderDataSource
{

public:
	virtual bool getData(vector<long> &data, std::string &error)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); // pretend it takes 100 ms for a scan
	
		data.resize(1024);
		for (unsigned int i = 0; i < data.size(); i++)
			data[i] = i;
		return true;
	}
};

static void runProvider()
{
	HokuyoProvider provider(new DummyDataSource());
	provider.run();
}

// ****************************************************************************
// HokuyoProvider Test Cases

TEST_CASE("HokuyoProvider/Dummy", "Recieve data from a dummy Hokuyo Provider") {

	{
		HokuyoData data = HokuyoProviderRequest::GetData(1);
	}

	std::thread providerThread(runProvider);
	providerThread.detach();

	std::this_thread::sleep_for(std::chrono::milliseconds(500)); // sleep so some scans get in

	{
		HokuyoData data;
		
		while (data.m_dataArrayArray.size() < 1024)
		{
			
			data = HokuyoProviderRequest::GetData("tcp://localhost", 1024);
			
			if (!data.m_error.empty())
			{
				HokuyoProvider::setShutdown();
				providerThread.join();
				REQUIRE(data.m_error.empty());
			}

			for (unsigned int i = 0; i < data.m_dataArrayArray.size(); i++)
			{
				std::vector<long> &dataArray = data.m_dataArrayArray[i];
				REQUIRE(dataArray.size() != 1024);
			}
		}
	}

	HokuyoProvider::setShutdown();
	providerThread.join();
}

