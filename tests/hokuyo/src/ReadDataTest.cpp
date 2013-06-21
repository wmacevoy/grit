#include "ReadDataTest.h"
#include <findUrgPorts.h>
#include <UrgDevice.h>
#include <string.h>

CPPUNIT_TEST_SUITE_REGISTRATION(ReadDataTestCase);

void ReadDataTestCase::testReadingData()
{
	int scan_times = 10;
	std::max(scan_times, 0);

	// Search URG port
	std::vector<std::string> ports;
	qrk::findUrgPorts(ports);

	CPPUNIT_ASSERT(ports.size() > 0);

	qrk::UrgDevice urg;
	CPPUNIT_ASSERT(urg.connect(ports[0].c_str()));

	// Get data by GD command
	std::vector<long> data;
	for (int i = 0; i < scan_times; ++i) {
		int n = urg.capture(data);
		if (n < 0) {
			continue;
		}
	}

}
