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
#include "LeapMessage.h"
#include "Configure.h"
#include "Leap.h"
#include "now.h"

using namespace Leap;
using namespace std;

void *zmq_context = 0;
void *zmq_pub = 0;
Configure cfg;
bool verbose;
volatile int die = 0;
double decay_time;
double t;
double dt;

LeapMessage leapMessage;

class MyListener : public Listener
{
public:
  void onInit(const Controller &controller)
  {
    if (verbose) cout << "init" << endl;
    t=now();
    dt=0;
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

  void average(double p, double q, float uxyzbar[3], float w,const Vector &uxzy)
  {
    if (p > 0.0) {
      uxyzbar[0]=p*uxyzbar[0]+q*( w*uxzy[0]);
      uxyzbar[1]=p*uxyzbar[1]+q*(-w*uxzy[2]);
      uxyzbar[2]=p*uxyzbar[2]+q*( w*uxzy[1]);
    } else {
      uxyzbar[0]=( w*uxzy[0]);
      uxyzbar[1]=(-w*uxzy[2]);
      uxyzbar[2]=( w*uxzy[1]);
    }

  }

  void update(double p, double q, LeapHandMessage &message, const Hand &hand)
  {
    average(p,q,message.at,(1/25.4),hand.palmPosition());
    average(p,q,message.point,1,hand.direction());
    average(p,q,message.down,1,hand.palmNormal());
  }

  void onFrame(const Controller &controller)
  {
    double t1=now();
    dt=t1-t;
    t=t1;
    double p=(dt < decay_time) ? exp(-dt/decay_time) : 0.0;
    double q=1-p;
    

    const Frame frame = controller.frame();
    if (frame.hands().count() != 2) return;
    
    const Hand &hand0=frame.hands()[0];
    const Hand &hand1=frame.hands()[1];
    
    if (!hand0.isValid() || !hand1.isValid()) return;
    
    if (hand0.palmPosition()[0] < hand1.palmPosition()[0]) {
      update(p,q,leapMessage.left,hand0);
      update(p,q,leapMessage.right,hand1);
    } else {
      update(p,q,leapMessage.left,hand1);
      update(p,q,leapMessage.right,hand0);
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

class LeapPublisher : public ZMQHub
{
  bool rx(ZMQSubscribeSocket &socket) { return true; }

  bool tx(ZMQPublishSocket &socket)
  {
    ZMQMessage message(sizeof(LeapMessage));
    memcpy(message.data(),&leapMessage,sizeof(LeapMessage));
    return message.send(socket) != 0;
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
  
  decay_time = cfg.num("leap.provider.decay_time");
  
  int hwm = 1;
  int retries = 5;
  int linger = 25;
  int rc = 0;
  bool connected = false;
  
  MyListener listener;
  Controller controller(listener);	
  
  zmq_context = zmq_ctx_new ();
  zmq_pub = zmq_socket(zmq_context,ZMQ_PUB);

  while(!connected && retries--) {
	  if(zmq_setsockopt(zmq_pub, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0) {
	  	if(zmq_setsockopt(zmq_pub, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
	  		if(zmq_bind(zmq_pub, cfg.str("leap.provider.publish").c_str()) == 0) {
				connected = true;
			}

		}		
	  }
	  if (retries <= 0) {
	    int en=zmq_errno();
	    std::cout << "TCP Error Number " << en << " " << zmq_strerror(en) << std::endl;
	    die = true;
	  }
	  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
  signal(SIGINT, quitproc);
  signal(SIGTERM, quitproc);
  signal(SIGQUIT, quitproc);

  while(!die) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  controller.removeListener(listener);
  zmq_close(zmq_pub);
  zmq_ctx_destroy(zmq_context);

  kill(leapd_pid,SIGINT);
  waitpid(leapd_pid,0,0);

  std::cout << "--done!" << std::endl;

  return 0;
}
