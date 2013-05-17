#include "ThreadDriverRegistry.h"

#include <stdio.h>

ThreadDriverRegistry::ThreadDriverRegistry()
:	m_socket("tcp://*:31337")
{}

bool ThreadDriverRegistry::start()
{
	if (!m_socket.open())
		return false;

	return CentaurThread::start();
}

void ThreadDriverRegistry::doWork()
{
	const char * threadName = "Thread Driver Registry";
	printf("Starting %s...\n", threadName);


/*
	//  Bounce the message back.
	    char buf1 [32];
	    rc = zmq_recv (socket, buf1, 32, 0);
	    assert (rc == 32);
	    int rcvmore;
	    size_t sz = sizeof (rcvmore);
	    rc = zmq_getsockopt (socket, ZMQ_RCVMORE, &rcvmore, &sz);
	    assert (rc == 0);
	    assert (rcvmore);
	    rc = zmq_recv (socket, buf1, 32, 0);
	    assert (rc == 32);
	    rc = zmq_getsockopt (socket, ZMQ_RCVMORE, &rcvmore, &sz);
	    assert (rc == 0);
	    assert (!rcvmore);
	    rc = zmq_send (socket, buf1, 32, ZMQ_SNDMORE);
	    assert (rc == 32);
	    rc = zmq_send (socket, buf1, 32, 0);
	    assert (rc == 32);




	
	    zmq_errno
	    */
	

}
