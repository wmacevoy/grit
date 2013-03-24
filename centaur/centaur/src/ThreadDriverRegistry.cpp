#include "ThreadDriverRegistry.h"

#include <stdio.h>
#include <zmq.h>

ThreadDriverRegistry::ThreadDriverRegistry()
{}

void ThreadDriverRegistry::DoWork()
{
	void *ctx = zmq_init (1);
	assert (ctx);

	void *sb = zmq_socket(ctx, ZMQ_REP);
	assert (sb);

	int rc = zmq_bind(sb, "tcp://0.0.0.0:31337");
	assert(rc == 0);

}
