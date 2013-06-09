#ifndef LegPressure_HPP
#define LegPressure

#include <cstring>
#include <iostream>
#include <string>
#include <zmq.h>
#include <assert.h>

using namespace std;

class LegPressure{
  public:
  static const char *LOCATION;
  int l1,l2,l3,l4;
  void set(int newl1,int newl2,int newl3,int newl4) {
	  l1=newl1;
	  l2=newl2;
	  l3=newl3;
	  l4=newl4;
  }
  void subscribe(void *zmq_sub) {
	 zmq_msg_t msg;
	 zmq_msg_init(&msg);
	 zmq_msg_recv(&msg,zmq_sub,0);
	 memcpy(this,zmq_msg_data(&msg),sizeof(LegPressure));
	 zmq_msg_close(&msg);
  }
  void print(ostream &out) {
	 out << "L1=" << l1 << " L2="<< l2 <<" L3="<<l3<<" L4="<<l4<<endl;
	 out << endl;
  }
};

#endif
