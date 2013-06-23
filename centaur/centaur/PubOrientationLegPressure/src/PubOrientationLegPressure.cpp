#include <zmq.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

using namespace std;

#include "LegPressure.hpp"
#include "Orientation.hpp"

#define BAUDRATE B9600            

void publish(Orientation *o,LegPressure *lp,void *zmq_pub) {
      o->print(cout);
      lp->print(cout);
      zmq_msg_t msg;
      int rc=zmq_msg_init_size(&msg,sizeof(Orientation));
      assert(rc==0);
      memcpy(zmq_msg_data(&msg),o,sizeof(Orientation));
      rc=zmq_msg_send(&msg,zmq_pub,0);
      zmq_msg_close(&msg);
      rc=zmq_msg_init_size(&msg,sizeof(LegPressure));
      assert(rc==0);
      memcpy(zmq_msg_data(&msg),lp,sizeof(LegPressure));
      rc=zmq_msg_send(&msg,zmq_pub,0);
      zmq_msg_close(&msg);
}

int main(int argc,char **argv) {
  Orientation o;
  LegPressure lp;
  string line;
  void *context = zmq_ctx_new ();
  void *pub=zmq_socket(context,ZMQ_PUB);
  struct termios oldtio,newtio;
  int fd;
  ifstream in;
  if (argc!=2) {
	  cerr << "Run PubOrientationLegPressure <serial> <ZMQ Com>"<<endl;
	  cerr << "  <serial> Serial Port /dev/ttyUSB0 ..."<<endl;
	  return -1;
  }
  int rc = zmq_bind(pub, "tcp://*:5555");
  if (rc!=0) {
    int en=zmq_errno();
	cout << "TCP Error Number " << en << " " << zmq_strerror(en) <<endl;
	return -2;
  }
    fd = open((char *)argv[1], O_RDWR | O_NOCTTY ); 
    if (fd <0) {
	  perror(argv[1]); 
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
  
//  in.open(argv[1]);
//  if (!in.is_open()) {
//	 cout << "Port "<< argv[1] << " is not available for open"<<endl;
//	 return -3;
//  }
  while(true)
  {  
	 char buff[1024];
	 read(fd,buff, sizeof(buff));
	 istringstream ss(buff);
	 cout <<"Line:{"<<buff<<"}"<<endl;
	 string A,C,G,L;
	 char c;
	 int ax,ay,az,cx,cy,cz,gx,gy,gz,l1,l2,l3,l4;
	 ss>> 
	   A >> c >> ax >> c >> ay >> c >> az >> c >> // A,ax,ay,az,
	   C >> c >> cx >> c >> cy >> c >> cz >> c >> // C,cx,cy,cz,
	   G >> c >> gx >> c >> gy >> c >> gz >>  // G,gx,gy,gz,
	   L >> c >> l1 >> c >> l2 >> c>> l3 >> c >> l4;  // The L,l1,l2,l3,l4
	 // Mapping to different types or to 
	 // different values should occur here 
     o.set(ax,ay,az,cx,cy,cz,gx,gy,gz);
     lp.set(l1,l2,l3,l4);
     publish(&o,&lp,pub);
  }
  zmq_ctx_destroy(context);
  return 0;
}
