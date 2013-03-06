#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include <cstring>
#include <iostream>
#include <string>
#include <zmq.h>
#include <assert.h>

using namespace std;

class joystick{
  public:
  static const char *LOCATION;
  int x,y;
  bool button;
  void setX(int newX) { x=newX; }
  void setY(int newY) { y=newY; }
  void setButtonDown() { button=true; }
  void setButtonUp() { button=false; }
  void subscribe(void *zmq_sub) {
	 zmq_msg_t msg;
	 zmq_msg_init(&msg);
//	 cout <<"."<<endl;
	 zmq_msg_recv(&msg,zmq_sub,0);
	 memcpy(this,zmq_msg_data(&msg),sizeof(joystick));
	 zmq_msg_close(&msg);
  }
  void print(ostream &out) {
	 out << "X=" << x << " Y="<< y <<" Button=";
	 if (button) out << "Yes";
	 else out << "No";
	 out << " size="<< sizeof(joystick);
	 out << endl;
  }
};


#endif
