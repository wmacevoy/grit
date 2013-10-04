#include <zmq.h>
#include <iostream>

#include "joystick.hpp"
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

 #define BAUDRATE B9600            
 /* change this definition for the correct port */
 #define DEVICE "/dev/ttyACM0"
 #define _POSIX_SOURCE 1

using namespace std;

int main (int argc,char **argv)
{
  joystick js;
  struct termios oldtio,newtio;
  int fd;
  
  void *context = zmq_ctx_new ();
  void *subscriber = zmq_socket (context, ZMQ_SUB);
  if (zmq_connect(subscriber,"tcp://localhost:5555")!=0){
	int en=zmq_errno();
	cout << "Error Number " << en << " " << zmq_strerror(en) <<endl;
  } else {
	int rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "", 0);
    assert (rc == 0);
    fd = open((char *)DEVICE, O_RDWR | O_NOCTTY ); 
    if (fd <0) {
	  perror(DEVICE); 
	  return -1; 
	}
        
    tcgetattr(fd,&oldtio); /* save current serial port settings */
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | ICRNL;
    newtio.c_oflag = 0;
    newtio.c_lflag = ICANON;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);  
    char command[5];        
    cout << "Start" << endl;
    while (true) {
      js.subscribe(subscriber);
      if (js.y>16000) {
		sprintf(command,"d10\n");
		cout << "-";
	    write(fd,command,sizeof(command));
	  } else if (js.y<-16000) {
		sprintf(command,"u10\n");
		cout << "+";
	    write(fd,command,sizeof(command));
	  }
	  if (js.x>16000) {
		sprintf(command,"r10\n");
		cout << "r";
	    write(fd,command,sizeof(command));
	  } else if (js.x<-16000) {
		sprintf(command,"l10\n");
		cout << "l";
	    write(fd,command,sizeof(command));
	  }
	  cout.flush();
    }
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    zmq_close (subscriber);
    zmq_ctx_destroy (context);
  }
  return 0;
}

