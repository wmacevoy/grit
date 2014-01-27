#include <signal.h>
#include "webcamProviderObj.h"
#include "Configure.h"
Configure cfg;

webcamProvider *ghost;

void quitproc(int sig) {
	std::cout << std::endl << "Quitting..." << std::endl;
	ghost->kill();
}

int main(int argc, char** argv) {
	/*int index = (int)cfg.num("webcam.provider.index");
	int sleep_time = (int)cfg.num("webcam.provider.sleep_time");
	bool detect = cfg.flag("webcam.provider.detect");
	bool verbose = cfg.flag("webcam.provider.verbose");
	std::string address = cfg.str("webcam.provider.c_ip").c_str();
	std::string port = cfg.str("webcam.provider.port").c_str();*/

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	webcamProvider p(0, 20, true, true, argv[0], "127.0.0.1", "9993");
	ghost = &p;	
	if(p.init()) {
		p.provide();
	}

	return 0;
}
