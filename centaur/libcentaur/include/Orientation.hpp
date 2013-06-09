#ifndef ORIENTATION_HPP
#define ORIENTATION_HPP

#include <cstring>
#include <iostream>
#include <string>
#include <zmq.h>
#include <assert.h>

using namespace std;

class ThreeTuple {
	public:
    int x,y,z;
	ThreeTuple(int newX=0,int newY=0,int newZ=0) {
		x=newX;
		y=newY;
		z=newZ;
	}
	void print(ostream &out) {
		out << " X:"<<x<<" Y:"<<y<<" Z:"<<z;
	}
};

class Orientation{
  public:
  static const char *LOCATION;
  ThreeTuple acceleration,direction,rotation;
  void subscribe(void *zmq_sub) {
	 zmq_msg_t msg;
	 zmq_msg_init(&msg);
	 zmq_msg_recv(&msg,zmq_sub,0);
	 memcpy(this,zmq_msg_data(&msg),sizeof(Orientation));
	 zmq_msg_close(&msg);
  }
  void print(ostream &out) {
	out <<"A:";
    acceleration.print(out);
    out <<" D:";
    direction.print(out);
    out <<" R:";
    rotation.print(out);
    out << endl;
  }
};


#endif
