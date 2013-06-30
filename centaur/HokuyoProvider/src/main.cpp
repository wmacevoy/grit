#include "../include/HokuyoProvider.h"
//#include <stdlib.h>     /* exit */
//#include <signal.h>     /* signal */

int main( int argc, const char* argv[] )
{
//	signal(SIGINT, sigproc);
//	signal(SIGQUIT, quitproc);
	HokuyoProvider provider;
	provider.start();
}
 /*
void sigproc(int param)
{ 		 
	signal(SIGINT, sigproc);
}
 
void quitproc(int param)
{ 		 
	HokuyoProvider::setShutdown();
}
*/
