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
#include <chrono>

using namespace std;
using namespace std::chrono;

#include "LegPressure.hpp"
#include "Orientation.hpp"

#define BAUDRATE B9600

            
steady_clock::time_point last=steady_clock::now();

void publish(Orientation *o,LegPressure *lp,void *zmq_pub) {
      steady_clock::time_point next=steady_clock::now();
      int diff=duration_cast<milliseconds>(next-last).count();
//      cout << diff << endl;
      if (diff>100) { 
//        o->print(cout);
//        lp->print(cout);
        cout << '.';
        cout.flush();
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
        last=next;
      }
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
	  cerr << "Run PubOrientationLegPressure <serial>"<<endl;
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
	 int n=read(fd,buff, sizeof(buff));
	 buff[n-1]=0;
	 istringstream ss(buff);
//	 cout <<"Line:{"<<buff<<"}"<<endl;
	 char A,C,G,L,c;
	 int ax,ay,az,cx,cy,cz,gx,gy,gz,l1,l2,l3,l4;
	 ss>> 
	   A >> c >> ax >> c >> ay >> c >> az >> c >> // A,ax,ay,az,
	   C >> c >> cx >> c >> cy >> c >> cz >> c >> // C,cx,cy,cz,
	   G >> c >> gx >> c >> gy >> c >> gz >> c >> // G,gx,gy,gz,
	   L >> c >> l1 >> c >> l2 >> c>> l3 >> c >> l4;  // The L,l1,l2,l3,l4
//	 cout << '.' <<endl;
	 // Mapping to different types or to 
	 // different values should occur here
	 if (A=='A' && C=='C' && G=='G' && L=='L') { 
       o.set(ax,ay,az,cx,cy,cz,gx,gy,gz);
       lp.set(l1,l2,l3,l4);
       publish(&o,&lp,pub);
     }
  }
  zmq_ctx_destroy(context);
  return 0;
}
