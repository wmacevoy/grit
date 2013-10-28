#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>
#include <mutex>
#include <zmq.h>
#include <math.h>
#include <assert.h>
#include "leapStruct.h"
#include "Configure.h"
#include "Leap.h"

using namespace Leap;

Configure cfg;
bool verbose;
volatile int die = 0;
int sleep_time = 100000;
std::mutex locker;
leapData leapD;

void publish(leapData* data, void* zmq_pub)
{
  //if(verbose) std::cout << "Sending leap data..." << std::endl;
	locker.lock();
	int rc = zmq_send(zmq_pub, data, sizeof(leapData), ZMQ_DONTWAIT);
	locker.unlock();
	//if(verbose && rc > 0) std::cout << "Leap data sent!" << std::endl;
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
		const Hand hand0 = frame.hands()[0];
		const Hand hand1 = frame.hands()[1];
		if(verbose) std::cout << "hand0 Palm position: " << hand0.palmPosition() << std::endl;
		if(verbose) std::cout << "hand1 Palm position: " << hand1.palmPosition() << std::endl;

		locker.lock();
		
		//This is a possible solution to keep the hands appropriately distinguished.  
			leapD.lx = hand0.palmPosition()[0]; leapD.ly = hand0.palmPosition()[1]; leapD.lz = hand0.palmPosition()[2];
			leapD.lroll = hand0.palmNormal().roll() * 180.0 / M_PI;
			leapD.lpitch = hand0.palmNormal().pitch()  * 180.0 / M_PI;
			leapD.lyaw = hand0.palmNormal().yaw() * 180.0 / M_PI;
			
		if(hand0.palmPosition()[0] < 0)
		{
			leapD.lx = hand0.palmPosition()[0]; leapD.ly = hand0.palmPosition()[1]; leapD.lz = hand0.palmPosition()[2];
			leapD.lroll = hand0.palmNormal().roll() * 180.0 / M_PI;
			leapD.lpitch = hand0.palmNormal().pitch()  * 180.0 / M_PI;
			
			if(hand1.isValid())
			{
					leapD.rx = hand1.palmPosition()[0]; leapD.ry = hand1.palmPosition()[1]; leapD.rz = hand1.palmPosition()[2];
					leapD.rroll = hand1.palmNormal().roll() * 180.0 / M_PI;
					leapD.rpitch = hand1.palmNormal().pitch()  * 180.0 / M_PI;
			}
		}
		else if(hand0.palmPosition()[0] > 0)
		{
			leapD.rx = hand0.palmPosition()[0]; leapD.ry = hand0.palmPosition()[1]; leapD.rz = hand0.palmPosition()[2];
			leapD.rroll = hand0.palmNormal().roll() * 180.0 / M_PI;
			leapD.rpitch = hand0.palmNormal().pitch()  * 180.0 / M_PI;

			if(hand1.isValid())
			{
				leapD.lx = hand1.palmPosition()[0]; leapD.ly = hand1.palmPosition()[1]; leapD.lz = hand1.palmPosition()[2];
				leapD.lroll = hand1.palmNormal().roll() * 180.0 / M_PI;
				leapD.lpitch = hand1.palmNormal().pitch()  * 180.0 / M_PI;
			}
			}
	  
		locker.unlock();

		if(verbose) std::cout << "LEFT: " << leapD.lx << " " << leapD.ly << " " << leapD.lz << " roll:" << leapD.lroll << " pitch:" << leapD.lpitch << " yaw:" << leapD.lyaw << " hnorm:" << hand0.palmNormal() << std::endl;
		if(verbose) std::cout << "RIGHT: " << leapD.rx << " " << leapD.ry << " " << leapD.rz << " " << leapD.rroll << std::endl;
	}	
}

void quitproc(int sig)
{
	printf("\nQuitting...\n");
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
	int linger = 25;
	int rc = 0;

	handListener listener;
	Controller controller(listener);	

	void *context = zmq_ctx_new ();
	void *pub = zmq_socket(context,ZMQ_PUB);
	rc = zmq_setsockopt(pub, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	assert(rc == 0);
	
	rc = zmq_setsockopt(pub, ZMQ_LINGER, &linger, sizeof(linger));
	assert(rc == 0);
	
	rc = zmq_bind(pub, "tcp://*:9990");
	if (rc!=0) {
		int en=zmq_errno();
		std::cout << "TCP Error Number " << en << " " << zmq_strerror(en) << std::endl;
		return 1;
	}

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	while(!die)
	{
		publish(&leapD, pub);
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
	}

	controller.removeListener(listener);
	zmq_close(pub);
	zmq_ctx_destroy(context);

	return 0;
}
