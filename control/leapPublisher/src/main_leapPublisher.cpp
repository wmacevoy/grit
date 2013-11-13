#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
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
using namespace std;

void *zmq_context = 0;
void *zmq_pub = 0;
Configure cfg;
bool verbose;
volatile int die = 0;
int sleep_time = 100;
std::mutex locker;
LeapMessage leapMessage;

class MyListener : public Listener
{
public:
  void onInit(const Controller &controller)
  {
    if (verbose) cout << "init" << endl;
  }
  void onConnect(const Controller &controller) 
  {
    if (verbose) cout << "connect" << endl;
  }
  void onDisconnect(const Controller &controller)
  {
    if (verbose) cout << "disconnect" << endl;
  }
  void onExit(const Controller &controller)
  {
    if (verbose) cout << "exit" << endl;
  }

  void update(LeapHandMessage &message, const Hand &hand)
  {
    const Vector &at = hand.palmPosition();
    message.at[0]=at[0]/25.4;
    message.at[1]=at[1]/25.4;
    message.at[2]=at[2]/25.4;
  
    const Vector &point = hand.direction();
    message.point[0]=point[0];
    message.point[1]=point[1];
    message.point[2]=point[2];
    
    const Vector &down = hand.palmNormal();
    message.down[0]=down[0];
    message.down[1]=down[1];
    message.down[2]=down[2];
  }

  void onFrame(const Controller &controller)
  {
    const Frame frame = controller.frame();
    if (frame.hands().count() != 2) return;
    
    const Hand &hand0=frame.hands()[0];
    const Hand &hand1=frame.hands()[1];
    
    if (!hand0.isValid() || !hand1.isValid()) return;
    
    if (hand0.palmPosition()[0] < hand1.palmPosition()[0]) {
      update(leapMessage.left,hand0);
      update(leapMessage.right,hand1);
    } else {
      update(leapMessage.left,hand1);
      update(leapMessage.right,hand0);
    }
    
    int rc = zmq_send(zmq_pub, &leapMessage, sizeof(LeapMessage), ZMQ_DONTWAIT);
    if (verbose) {
      { std::cout << " status=" << rc; }
      { 
	LeapHandMessage &hand=leapMessage.left;
	std::cout << std::setprecision(2) << std::fixed;
	std::cout << " left" << ": at=[" << hand.at[0] << "," << hand.at[1] << "," << hand.at[2] << "] point=[" << hand.point[0] << "," << hand.point[1] << "," << hand.point[2] << "] down=[" << hand.down[0] << "," << hand.down[1] << "," << hand.down[2] << "]";
      }
      {
	LeapHandMessage &hand=leapMessage.right;
	std::cout << std::setprecision(2);
	std::cout << " right" << ": at=[" << hand.at[0] << "," << hand.at[1] << "," << hand.at[2] << "] point=[" << hand.point[0] << "," << hand.point[1] << "," << hand.point[2] << "] down=[" << hand.down[0] << "," << hand.down[1] << "," << hand.down[2] << "]";
      }
      { std::cout << endl; }
    }
  }
};

void quitproc(int sig) {
  die = 1;
}
  
int main(int argc, char** argv)
{
  pid_t leapd_pid;

  if ((leapd_pid=fork()) == 0) {
    execl("/usr/bin/leapd","leapd",0);
  }

  cfg.path("../../setup");
  cfg.args("leap.provider.", argv);
  if (argc == 1) cfg.load("config.csv");
  verbose = cfg.flag("leap.provider.verbose", false);
  if (verbose) cfg.show();
  
  sleep_time = (int)cfg.num("leap.provider.sleep_time");
  
  int hwm = 1;
  int linger = 25;
  int rc = 0;
  
  MyListener listener;
  Controller controller(listener);	
  
  zmq_context = zmq_ctx_new ();
  zmq_pub = zmq_socket(zmq_context,ZMQ_PUB);
  rc = zmq_setsockopt(zmq_pub, ZMQ_SNDHWM, &hwm, sizeof(hwm));
  assert(rc == 0);
	
  rc = zmq_setsockopt(zmq_pub, ZMQ_LINGER, &linger, sizeof(linger));
  assert(rc == 0);
	
  rc = zmq_bind(zmq_pub, cfg.str("leap.provider.publish").c_str());
  if (rc!=0) {
    int en=zmq_errno();
    std::cout << "TCP Error Number " << en << " " << zmq_strerror(en) << std::endl;
    return 1;
  }

  signal(SIGINT, quitproc);
  signal(SIGTERM, quitproc);
  signal(SIGQUIT, quitproc);

  while(!die) {
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
  }

  controller.removeListener(listener);
  zmq_close(zmq_pub);
  zmq_ctx_destroy(zmq_context);

  kill(leapd_pid,SIGQUIT);
  waitpid(leapd_pid,0,0);

  std::cout << "done." << std::endl;

  return 0;
}
