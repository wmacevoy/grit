#include <stdio.h>
#include <signal.h>

#include "Brain.h"
#include "Body.h"
#include "Defines.h"
#include <CentaurSockets.h>

static volatile bool g_stop = false;

void intHandler(int dummy = 0) {
	g_stop = true;
	printf("Starting shutdown sequence.\n");
}

int main() {

	signal(SIGINT, intHandler);
	CentaurSocketSub subSocket(MAIN_STOPADDRESS, 1);
	if(!subSocket.open()){
		printf("Unable to open subscriber socket\n");
		return 1;
	}

	Brain& brain = Brain::get();
	Body& body = Body::get();

	if (!brain.start() || !body.start())
		return 1;

	while(!g_stop) {
		if(!brain.loop() || !body.loop())
			g_stop = true;
	}

	brain.stop();
	body.stop();

	return 0;
}
