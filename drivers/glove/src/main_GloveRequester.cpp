#include <zmq.h>
#include <iostream>
#include <signal.h>
#include <stdio.h>

int die = 0;

struct hands{
	long lthumb, ltrigger, lmiddle, lring;
	long rthumb, rtrigger, rmiddle, rring;
};

void quitproc(int param)
{
	die = 1;
}

void subscribe(void *zmq_sub, hands* manos) 
{
	int rc = zmq_recv(zmq_sub, manos, sizeof(hands), ZMQ_DONTWAIT);
}

int main(int argc, char** argv)
{
	int hwm = 1, rc;
	hands manos;
	
	void *context = zmq_ctx_new ();
	void *sub = zmq_socket(context, ZMQ_SUB);
	//rc = zmq_setsockopt(sub, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
	
	if (zmq_connect(sub, "tcp://127.0.0.1:6689") != 0)
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
