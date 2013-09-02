#include <zmq.h>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>

int die = 0;

struct Hands{
	int64_t lthumb, ltrigger, lmiddle, lring;
	int64_t rthumb, rtrigger, rmiddle, rring;
	void clear() {
	  lthumb=0;
	  ltrigger=0;
	  lmiddle=0; 
	  lring=0;
	  rthumb=0;
	  rtrigger=0; 
	  rmiddle=0; 
	  rring=0;
	}
};

void quitproc(int param)
{
	die = 1;
}

void subscribe(void *zmq_sub, Hands* manos) 
{
	manos->clear();
	int rc = zmq_recv(zmq_sub, manos, sizeof(Hands), 0);
}

int main(int argc, char** argv)
{
	int hwm = 1, rc;
	Hands manos;
	
	void *context = zmq_ctx_new ();
	void *sub = zmq_socket(context, ZMQ_SUB);
	//rc = zmq_setsockopt(sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
	
	if (zmq_connect(sub, "tcp://192.168.2.115:6689") != 0)
	{
		printf("Error initializing 0mq...\n");
		return 1;
	}
	
	while(!die)
	{
			subscribe(sub, &manos);
			
			std::cout << manos.lring << " " << manos.lmiddle << " " << manos.ltrigger << " " << 
			manos.lthumb << " " << manos.rring << " " << manos.rmiddle << " " << 
			manos.rtrigger << " " << manos.rthumb << " " << std::endl;
	}	
	
	zmq_close(sub);
	zmq_ctx_destroy(context);
	std::cout << "Adios" << std::endl;
	return 0;
}
