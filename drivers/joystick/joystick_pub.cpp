#include <zmq.h>
#include <iostream>
#include <SDL/SDL.h>
#include <assert.h>

using namespace std;

#include "joystick.hpp"

int last;
void publish(joystick *j,void *zmq_pub) {
	if (last==0) last=SDL_GetTicks();
    if (SDL_GetTicks()-last>100) { // Don't send messages faster than 10 a second
      j->print(cout);
      zmq_msg_t msg;
      int rc=zmq_msg_init_size(&msg,sizeof(joystick));
      assert(rc==0);
      memcpy(zmq_msg_data(&msg),j,sizeof(joystick));
      rc=zmq_msg_send(&msg,zmq_pub,0);
	/*  if (rc!=11) {
		  int en=zmq_errno();
	      cout << "Error Number " << en << " " << zmq_strerror(en) <<endl;
	  } */
      zmq_msg_close(&msg);
      last=SDL_GetTicks();  
    }
}

int main(int argc,char **argv) {
  joystick jm;
  SDL_Joystick *joystick;
  void *context = zmq_ctx_new ();
 
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0) {
        cerr<< "Couldn't initialize SDL:"<< SDL_GetError()<<endl;
        return 1;
  } else {
    cout << "Press Square to terminate"<<endl;
    SDL_JoystickEventState(SDL_ENABLE);
    void *pub=zmq_socket(context,ZMQ_PUB);
    int rc = zmq_bind(pub, "tcp://*:5555");
    if (rc!=0) {
      int en=zmq_errno();
	  cout << "TCP Error Number " << en << " " << zmq_strerror(en) <<endl;
    }
 /*  rc = zmq_bind (pub, "ipc://joystick.ipc");
    if (rc!=0) {
      int en=zmq_errno();
	  cout << "ipc Error Number " << en << " " << zmq_strerror(en) <<endl;
    } */
    joystick = SDL_JoystickOpen(0);
    SDL_Event event;
    last=SDL_GetTicks();
    while(true)
    {  
        SDL_Delay(10); // Don't check faster than 10 times a second
        while (SDL_PollEvent(&event)) {
          switch(event.type)
          {  
            case SDL_JOYAXISMOTION:
              if (event.jaxis.axis==0 && event.jaxis.which==0) { 
                jm.setX(event.jaxis.value);
                publish(&jm,pub);
              }
              else if (event.jaxis.axis==1 && event.jaxis.which==0)  {
                jm.setY(event.jaxis.value); 
                publish(&jm,pub);
              } 
            break;
            case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
              if ( event.jbutton.button==0 && event.jbutton.which==0) 
                return 0;
              if ( event.jbutton.button == 1 && event.jbutton.which==0 ) {
                jm.setButtonDown();
                publish(&jm,pub);
              }
            break;
            case SDL_JOYBUTTONUP:  /* Handle Joystick Button Presses */
              if ( event.jbutton.button == 1 && event.jbutton.which==0 ) {
                jm.setButtonUp();
                publish(&jm,pub);
              }
            break;
            case SDL_QUIT:
		SDL_Quit();
              return 0;
            break;
          }
        }
     }
    zmq_ctx_destroy(context);
    return 0;
  }
}
