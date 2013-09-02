#include "../include/HokuyoProvider.h"

#include <stdlib.h>     /* exit */
#include <signal.h>     /* signal */
#include <chrono>
#include <iostream>
#include <thread>

void quitproc(int);

int main( int argc, const char* argv[] )
{
	signal(SIGINT, quitproc);

	HokuyoProvider provider(new HokuyoDataSource());

	if (!provider.run())
		return 1;
	return 0;
}

void quitproc(int param)
{ 		 
	std::cout << "MAIN: Recieved quit signal, shutting everything down, please wait\n";
	HokuyoProvider::setShutdown();
}
