#include <stdio.h>
#include <signal.h>

#include "Centaur.h"

static volatile bool g_stopCentaur = false;

void intHandler(int dummy = 0) {
	g_stopCentaur = true;
	printf("Starting shutdown sequence.\n");
}

int main() {

	signal(SIGINT, intHandler);

	Centaur& centaur = Centaur::Get();

	centaur.Start();

	while(!g_stopCentaur) {
		sleep(10);
	}

	centaur.Stop();

	return 0;
}
