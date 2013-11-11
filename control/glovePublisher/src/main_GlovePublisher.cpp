#include <fcntl.h>
#include <termios.h>
#include <zmq.h>
#include <iostream>
#include <string>
#include <sstream>
#include <signal.h>
#include <strings.h>
#include <stdint.h>
#include "glovestruct.h"

#include "Configure.h"

#define BAUDRATE B9600

Configure cfg;

volatile int die = 0;

void quitproc(int param)
{
	die = 1;
}

void publish(void* zmq_pub, Hands* manos)
{
	int rc = zmq_send(zmq_pub, manos, sizeof(Hands),0);
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("gloves.publisher.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("gloves.publisher.verbose", false);
	if (verbose) cfg.show();

	std::string gloves_path = cfg.str("gloves.publisher.dev_path");
	
	char buff[1024];
	int fd;
	int hwm = 1, rc;
	struct termios oldtio,newtio;
	Hands manos;
	
	
	fd = open(gloves_path.c_str(), O_RDWR | O_NOCTTY ); 
	if (fd <0) {
	  perror(gloves_path.c_str()); 
	  return -1; 
	}
	
	void *context = zmq_ctx_new ();
	void *pub = zmq_socket(context,ZMQ_PUB);
	rc = zmq_setsockopt(pub, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	
	rc = zmq_bind(pub, "tcp://*:6689");
	if (rc!=0) {
		int en=zmq_errno();
		std::cout << "TCP Error Number " << en << " " << zmq_strerror(en) << std::endl;
		return -2;
	}
		
	tcgetattr(fd,&oldtio); /* save current serial port settings */
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);  
	
	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);
	
	while(!die)
	{
		int n = read(fd, buff, sizeof(buff));
		buff[n - 1] = 0;
		
		std::istringstream ss(buff);
		ss >> manos.lring >> manos.lmiddle >> manos.ltrigger >> 
			manos.lthumb >> manos.rring >> manos.rmiddle >> 
			manos.rtrigger >> manos.rthumb; 
		
		publish(pub, &manos);
		
		if(verbose) std::cout << manos.lring << " " << manos.lmiddle << " " << manos.ltrigger << " " << 
			manos.lthumb << " " << manos.rring << " " << manos.rmiddle << " " << 
			manos.rtrigger << " " << manos.rthumb << " " << std::endl;
	}
	
	std::cout << "Quitting..." << std::endl;
	std::cout << "destroying and closing zmq..." << std::endl;
	zmq_close(pub);
	zmq_ctx_destroy(context);
	std::cout << "--done!" << std::endl;
	return 0;
}
