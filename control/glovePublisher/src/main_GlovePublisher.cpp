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

#define BAUDRATE B9600

int die = 0;

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
	char buff[1024];
	int fd;
	int hwm = 1, rc;
	struct termios oldtio,newtio;
	Hands manos;
	
	if (argc!=2) {
	  std::cerr << "Run GlovePublisher <serial>" << std::endl;
	  std::cerr << "  <serial> Serial Port /dev/ttyACM0 ..." << std::endl;
	  return -1;
	}
	
	fd = open((char *)argv[1], O_RDWR | O_NOCTTY ); 
	if (fd <0) {
	  perror(argv[1]); 
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
	signal(SIGQUIT, quitproc);
	
	while(!die)
	{
		int n = read(fd, buff, sizeof(buff));
		buff[n - 1] = 0;
		
		std::istringstream ss(buff);
		ss >> manos.ltrigger >> manos.lmiddle >> manos.lring >> 
			manos.lthumb >> manos.rtrigger >> manos.rmiddle >> 
			manos.rring >> manos.rthumb; 
			
		publish(pub, &manos);
		
		/*std::cout << manos.lring << " " << manos.lmiddle << " " << manos.ltrigger << " " << 
			manos.lthumb << " " << manos.rring << " " << manos.rmiddle << " " << 
			manos.rtrigger << " " << manos.rthumb << " " << std::endl;*/
	}
	
	zmq_close(pub);
	zmq_ctx_destroy(context);
	
	std::cout << "Adios" << std::endl;
	return 0;
}
