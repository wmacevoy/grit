#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>
#include <mutex>
#include <zmq.h>
#include <math.h>
#include <assert.h>
#include "LeapMessage.h"
#include "Configure.h"
#include "Leap.h"

using namespace Leap;

Configure cfg;
bool verbose;
volatile int die = 0;
int sleep_time = 100;
std::mutex locker;
LeapMessage leapMessage;

void publish(LeapMessage * data, void* zmq_pub)
{
  if(verbose) std::cout << "Sending leap data..." << std::endl;
  locker.lock();
  int rc = zmq_send(zmq_pub, data, sizeof(LeapMessage), ZMQ_DONTWAIT);
  locker.unlock();
  if(verbose && rc > 0) std::cout << "Leap data sent!" << std::endl;
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

void update(LeapHandMessage &message, const Hand &hand)
{
  const Vector &at = hand.palmPosition();
  message.at[0]=at[0];
  message.at[1]=at[1];
  message.at[2]=at[2];
  
  const Vector &point = hand.direction();
  message.point[0]=point[0];
  message.point[1]=point[1];
  message.point[2]=point[2];

  const Vector &down = hand.palmNormal();
  message.down[0]=down[0];
  message.down[1]=down[1];
  message.down[2]=down[2];
}

void handListener::onFrame(const Controller& controller)
{
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  if (frame.hands().count() != 2) return;

  const Hand &hand0=frame.hands()[0];
  const Hand &hand1=frame.hands()[1];

  if (!hand0.isValid() || !hand1.isValid()) return;

  locker.lock();
  if (hand0.palmPosition()[0] < hand1.palmPosition()[0]) {
    update(leapMessage.left,hand0);
    update(leapMessage.right,hand1);
  } else {
    update(leapMessage.left,hand1);
    update(leapMessage.right,hand0);
  }
  locker.unlock();

  if (verbose) {
    { 
      LeapHandMessage &hand=leapMessage.left;
      std::cout << "left" << ": at=[" << hand.at[0] << "," << hand.at[1] << "," << hand.at[2] << "] point=[" << hand.point[0] << "," << hand.point[1] << "," << hand.point[2] << "] down=[" << hand.down[0] << "," << hand.down[1] << "," << hand.down[2] << "]" << std::endl;
    }
    {
      LeapHandMessage &hand=leapMessage.right;
      std::cout << "right" << ": at=[" << hand.at[0] << "," << hand.at[1] << "," << hand.at[2] << "] point=[" << hand.point[0] << "," << hand.point[1] << "," << hand.point[2] << "] down=[" << hand.down[0] << "," << hand.down[1] << "," << hand.down[2] << "]" << std::endl;
    }
  }

}

void quitproc(int sig) {
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
	
	rc = zmq_bind(pub, cfg.str("leap.provider.publish").c_str());
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
		publish(&leapMessage, pub);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	std::cout << std::endl << "Quitting..." << std::endl;
	std::cout << "removing listenter..." << std::endl;
	controller.removeListener(listener);
	std::cout << "--done!" << std::endl;
	std::cout << "closing and destroying zmq..." << std::endl;
	zmq_close(pub);
	zmq_ctx_destroy(context);
	std::cout << "--done!" << std::endl;

	return 0;
}
