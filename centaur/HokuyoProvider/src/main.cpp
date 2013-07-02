#include "../include/HokuyoProvider.h"
#include <stdlib.h>     /* exit */
#include <signal.h>     /* signal */
#include <chrono>
#include <iostream>

void quitproc(int);

int main( int argc, const char* argv[] )
{
	signal(SIGINT, quitproc);
	signal(SIGQUIT, quitproc);
	HokuyoProvider provider;
	provider.start();
	while(!HokuyoProvider::isDone()){
		std::cout << "MAIN: Nothing to do, sleeping for 5 seconds\n";
		std::chrono::milliseconds dura( 5000 );
		std::this_thread::sleep_for( dura );
	}
}
 
void quitproc(int param)
{ 		 
	std::cout << "MAIN: Recieved quit signal, shutting everything down...\n";
	HokuyoProvider::setShutdown();
}

