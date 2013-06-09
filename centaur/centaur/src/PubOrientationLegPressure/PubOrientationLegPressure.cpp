#include <zmq.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>
#include <sstream>

using namespace std;

#include "LegPressure.hpp"
#include "Orientation.hpp"

void publish(Orientation *o,LegPressure *lp,void *zmq_pub) {
      o->print(cout);
      lp->print(cout);
      zmq_msg_t msg;
      int rc=zmq_msg_init_size(&msg,sizeof(Orientation));
      assert(rc==0);
      memcpy(zmq_msg_data(&msg),j,sizeof(Orientation));
      rc=zmq_msg_send(&msg,zmq_pub,0);
      zmq_msg_close(&msg);
      int rc=zmq_msg_init_size(&msg,sizeof(LegPressure));
      assert(rc==0);
      memcpy(zmq_msg_data(&msg),j,sizeof(LegPressure));
      rc=zmq_msg_send(&msg,zmq_pub,0);
      zmq_msg_close(&msg);
}

int main(int argc,char **argv) {
  Orientation o;
  LegPressure lp;
  string line;
  void *context = zmq_ctx_new ();
  void *pub=zmq_socket(context,ZMQ_PUB);
  ifstream in;
  if (argc<>3) {
	  cerr << "Run PubOrientationLegPressure <serial> <ZMQ Com>"<<endl;
	  cerr << "  <serial> Serial Port /dev/ttyUSB0 ..."<<endl;
	  cerr << "  <ZMQ Com> 0MQ publishing style tcp://*:5555"<<endl;
	  return -1;
  }
  int rc = zmq_bind(pub, argv[2]);
  if (rc!=0) {
    int en=zmq_errno();
	cout << "TCP Error Number " << en << " " << zmq_strerror(en) <<endl;
	return -2;
  }
  in.open(argv[1]);
  if (!in.is_open()) {
	 cout << "Port "<< argv[1] << " is not available for open"<<endl;
	 return -3;
  }
  while(true)
  {  
	 in.getline(in,line);
	 istringstream ss(line);
	 string A,C,G,L;
	 char c;
	 int ax,ay,az,cx,cy,cz,gx,gy,gz,l1,l2,l3,l4;
	 ss>> 
	   A >> c >> ax >> c >> ay >> c >> az >> c >> // A,ax,ay,az,
	   C >> c >> cx >> c >> cy >> c >> cz >> c >> // C,cx,cy,cz,
	   G >> c >> gx >> c >> gy >> c >> gz >>  // G,gx,gy,gz,
	   L >> c >> l1 >> c >> l2 >> c>> l3 >> c >> l4  // The L,l1,l2,l3,l4
	 // Mapping to different types or to 
	 // different values should occur here 
     o.set(ax,ay,az,cx,cy,cz,gx,gy,gz);
     lp.set(l1,l2,l3,l4);
     publish(&o,&lp,pub);
  }
  zmq_ctx_destroy(context);
  return 0;
}
