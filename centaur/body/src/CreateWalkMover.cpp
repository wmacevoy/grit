// !!!
// !!! DO NOT USE -- THIS IS VERY INCOMPLETE CODE
// !!!

#include "CreateWalkMover.h"
#include "BodyGlobals.h"

#include <thread>
#include <math.h>

using namespace std;

class Sashay
{
public:
  enum { SASHAY_CENTER,
	 SASHAY_LIFT_LEG1,
	 SASHAY_LIFT_LEG2,
	 SASHAY_LIFT_LEG3,
	 SASHAY_LIFT_LEG4 };
	 
  double x,y; // current offset
  double x0,y0; // goal offset
  double d0; // goal distance
  double v; // velocity of movement
  double epsilon; // radius close enough
  int state;

  Sashay()
  {
    x=0;
    y=0;
    x0=0;
    y0=0;
    v=0;
    epsilon=0.5;
    state=0;
  }

  void update(double dt)
  {
    switch(state) {
    case SASHAY_CENTER: { x0=0; y0=0; break; }
    case SASHAY_LIFT_LEG1: { x0=d0, y0=-d0; break; }
    case SASHAY_LIFT_LEG2: { x0=-d0; y0=-d0; break; }
    case SASHAY_LIFT_LEG3: { x0=-d0; y0=d0; break; }
    case SASHAY_LIFT_LEG4: { x0=d0; y0=-d0; break; }
    }
    double dx=x-x0;
    double dy=y-y0;
    double d=sqrt(dx*dx+dy*dy);
    if (d > v*dt) {
      d=v*dt/d;
      dx=d*dx;
      dy=d*dy;
    };
    x += dx;
    y += dy;
  }

  bool ready()
  {
    return pow(x-x0,2)+pow(y-y0,2) < pow(epsilon,2);
  }

  void next(int macroState)
  {
    state = macroState;
  }
};

class Stepper
{
public:
  int state;
  int last_state;

  enum { 
    STEP_HOME,
    STEP_FORWARD,
    STEP_DOWN,
    STEP_BACK,
    STEP_UP
  };
  
  float omega; // 1/turn radius 
  float v[3]; // velocity

  float p[3]; // tip coordinates
  float p0[3]; // goal coordinates
  float phome[3]; // home coordinates
  bool is_ready;

  void update(double dt)
  {
    switch(state) {
    case STEP_HOME: home(); break;
    case STEP_FORWARD: forward(); break;
    case STEP_DOWN: down(); break;
      //    case STEP_BACK: back(); break;
      //    case STEP_UP: up(); break;
    }
    last_state = state;
  }

  void home()
  {
    p0[0]=phome[0];
    p0[1]=phome[1];
    p0[2]=phome[2];
    shift();
  }
  
  void shift()
  {
    
  }

  void up()
  {
    if (last_state != state) {
      p0[0]=p[0];
      p0[1]=p[1];
      p0[2] += 6.0;
    }
    shift();
    //    close();
  }

  void down()
  {
    if (last_state != state) {
      p0[0]=p[0];
      p0[1]=p[1];
      p0[2] -= 2*6.0;
    }
    shift();
    // fixme
  }

  void forward()
  {
    if (last_state != state) {
      p0[0] = p[0]+v[0];
      p0[1] = p[1]+v[1];
      p0[2] = p[2]+v[2];
    }
    shift();
  }

  bool ready() { return false; }
};


class WalkMover : public Mover
{
public:
  std::thread *moving;
  int lastState;
  int state;

  double lastSimTime;
  double simDelta;

  double lastRealTime;
  double realDelta;

  Sashay sashay;
  Stepper steppers[4];
  int sequence[4];
  bool running;

  WalkMover()
  {
    moving = 0;
    running = false;

    lastState = -1;
    state = 0;

    lastSimTime = simTime;
    simDelta =0;

    lastRealTime = realTime;
    realDelta=0;

    sequence[0]=0;
    sequence[1]=1;
    sequence[2]=2;
    sequence[3]=3;

    simDelta=0;
    realDelta=0;
  }

  void start()
  {
    if (moving == 0) {
      running = true;
      moving = new std::thread(&WalkMover::run,this);
    }
  }

  void update(double dt)
  {
    bool microReady = true;
    sashay.update(dt);
    microReady = microReady && sashay.ready();

    for (int i=0; i<4; ++i) {
      steppers[i].update(dt);
      microReady = microReady && steppers[i].ready();
    }

    if (microReady) {
      state = (state + 1);
      if (state == 5) {
	sashay.next(0);
      }
      sashay.next(state);
      for (int i=0; i<4; ++i) {
	//	legs[i].next();
      }
    }
  }

  void run()
  {
    while (running) {
      simDelta=simTime-lastSimTime;
      lastSimTime=simTime;
      realDelta=realTime-lastRealTime;
      lastRealTime=realTime;
      
      update(simDelta);
    }
  }

  void stop() {
    running = false;
    moving->join();
    delete moving;
    moving = 0;
    state = -1;
  }

  ~WalkMover()
  {
    stop();
  }

};

MoverSP CreateWalkMover()
{
  return MoverSP(new WalkMover());
}
