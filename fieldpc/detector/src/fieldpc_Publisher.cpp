#include <signal.h>
#include "fieldpc_webProviderOBJ.h"
#include "Configure.h"
Configure cfg;

webcamProvider *ghost;

void quitproc(int sig) {
	std::cout << std::endl << "Quitting..." << std::endl;
	ghost->kill();
}

int main(int argc, char** argv) {
	cfg.path("../../setup");
	cfg.args("detector.webcam.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	bool verbose = cfg.flag("detector.webcam.provider.verbose", true);
	if (verbose) cfg.show();
		
	int indexR = (int)cfg.num("detector.webcam.provider.indexR");
	int indexL = (int)cfg.num("detector.webcam.provider.indexL");
	int sleep_time = (int)cfg.num("detector.webcam.provider.sleep_time");
	std::string addressf = cfg.str("detector.webcam.provider.f_ip").c_str();
	std::string addressc = cfg.str("detector.webcam.provider.c_ip").c_str();
	std::string port = cfg.str("detector.webcam.provider.port").c_str();

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	webcamProvider p(indexR, indexL, sleep_time, verbose, argv[0], addressf.c_str(), addressc.c_str(), port);
	ghost = &p;	
	if(p.init()) {
		//p.rotate();
		p.provide();
		
	}

	return 0;
}
