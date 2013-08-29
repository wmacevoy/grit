#define CATCH_CONFIG_MAIN  // This tell CATCH to provide a main() - only do this in one cpp file
#include "../../../libs/catch/catch.hpp"
#include "../../../libs/hokuyo/include/UrgDevice.h"
#include "../../../libs/hokuyo/include/findUrgPorts.h"
#include <string.h>

TEST_CASE( "Hokuyo/ReadDataTest", "Testing reading data" ) {
	int scan_times = 100;

	// Search URG port
	std::vector<std::string> ports;
	qrk::findUrgPorts(ports);

	REQUIRE(ports.size() > 0);

	qrk::UrgDevice urg;
	REQUIRE(urg.connect(ports[0].c_str()));

	// Get data by GD command
	std::vector<long> data;
	for (int i = 0; i < scan_times; ++i) {
		int n = urg.capture(data);
		REQUIRE(n >= 0);
	}
}
