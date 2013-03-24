#include "ThreadDriverRegistry.h"

#include <stdio.h>

ThreadDriverRegistry::ThreadDriverRegistry()
{}

void * ThreadDriverRegistry::DoWork()
{
	printf("hello from the thread.\n");
	return NULL;
}
