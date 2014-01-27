#include "webcamProvider.h"
#include "Configure.h"
Configure cfg;

int main(int argc, char** argv) {
	/*int index = (int)cfg.num("webcam.provider.index");
	int sleep_time = (int)cfg.num("webcam.provider.sleep_time");
	bool detect = cfg.flag("webcam.provider.detect");
	std::string address = cfg.str("webcam.provider.c_ip").c_str();
	std::string port = cfg.str("webcam.provider.port").c_str();*/

	webcamProvider p(0, 20, false, "127.0.0.1", "9993");
	p.init();
	p.provide();

	return 0;
}
