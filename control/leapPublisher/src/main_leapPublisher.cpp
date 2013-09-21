#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>
#include <mutex>
#include <zmq.h>
#include "leapStruct.h"
#include "Configure.h"
#include "Leap.h"

using namespace Leap;

Configure cfg;
bool verbose;
volatile int die = 0;
int sleep_time = 100;
std::mutex locker;
leapData leapD;

void publish(leapData* data, void* zmq_pub)
{
	if(verbose) std::cout << "Sending hand data..." << std::endl;
	locker.lock();
	int rc = zmq_send(zmq_pub, data, sizeof(leapData), 0);
	locker.unlock();
	if(verbose && rc > 0) std::cout << "Hand data sent!" << std::endl;
}

class handListener : public Listener
{
public:
	virtual void onInit(const Controller&);
	virtual void onConnect(const Controller&);
	virtual void onDisconnect(const Controller&);
	virtual void onExit(const Controller&);
	virtual void onFrame(const Controller&);
};

void handListener::onInit(const Controller& controller)
{
	if(verbose) std::cout << "Initialized" << std::endl;
}

void handListener::onConnect(const Controller& controller)
{
	if(verbose)   std::cout << "Connected" << std::endl;
}

void handListener::onDisconnect(const Controller& controller)
{
	//Note: not dispatched when running in a debugger.
	if(verbose) std::cout << "Disconnected" << std::endl;
}

void handListener::onExit(const Controller& controller)
{
	if(verbose) std::cout << "Exited" << std::endl;
}

void handListener::onFrame(const Controller& controller)
{
	// Get the most recent frame and report some basic information
	const Frame frame = controller.frame();
	if (!frame.hands().empty())
	{
		// Get the first hand
		const Hand hand = frame.hands()[0];
		if(verbose) std::cout << "Palm position: " << hand.palmPosition() << "  Palm normal: " << hand.palmNormal() << std::endl;
		locker.lock();
		leapD.x = hand.palmPosition()[0]; leapD.y = hand.palmPosition()[1]; leapD.z = hand.palmPosition()[2];
		leapD.normala = hand.palmNormal()[0]; leapD.normalb = hand.palmNormal()[1]; leapD.normalc = hand.palmNormal()[2];
		locker.unlock();

		std::cout << leapD.x << " " << leapD.y << " " << leapD.z << " " << leapD.normala << " " << leapD.normalb << " " << leapD.normalc << std::endl;
	}	
}

void SignalHandler(int sig)
{
	if(verbose) printf("\nQuitting...\n");
	die = 1;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("leap.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("leap.provider.verbose", false);
	if (verbose) cfg.show();
	
	sleep_time = (int)cfg.num("leap.provider.sleep_time", 400);

	int hwm = 1;
	int rc = 0;

	handListener listener;
	Controller controller;

	controller.addListener(listener);

	void *context = zmq_ctx_new ();
	void *pub = zmq_socket(context,ZMQ_PUB);
	rc = zmq_setsockopt(pub, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	
	rc = zmq_bind(pub, "tcp://*:9990");
	if (rc!=0) {
		int en=zmq_errno();
		std::cout << "TCP Error Number " << en << " " << zmq_strerror(en) << std::endl;
		return 1;
	}

	struct sigaction new_action;
	new_action.sa_handler = SignalHandler;
	sigemptyset (&new_action.sa_mask);
	new_action.sa_flags = 0;

	sigaction (SIGTERM, &new_action, NULL);
	sigaction (SIGINT, &new_action, NULL);

	while(!die)
	{
		publish(&leapD, pub);
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
	}
	
	std::cout << "\nQuitting..." << std::endl;

	controller.removeListener(listener);

	return 0;
}
