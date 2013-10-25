// !!!
// !!! DO NOT USE -- THIS IS VERY INCOMPLETE CODE
// !!!

#if 0

#include "CreateWalkMover.h"
#include "BodyGlobals.h"

#include <thread>
#include <math.h>

using namespace std;

class Sashay
{
public:
  enum { 
    SASHAY_CENTER,
    SASHAY_LIFT_LEG1,
    SASHAY_LIFT_LEG2,
    SASHAY_LIFT_LEG3,
    SASHAY_LIFT_LEG4,
    SASHAY_STATES
  };

  double p0[SASHAY_STATES][2];
  double p[2]; // current offset (in)
  double v; // velocity of movement (in/sec)
  double epsilon; // radius close enough (in)
  int state;

  Sashay()
  {
    init(4,4);
  }

  void init(double wx, double wy)
  {
    p[0]=0;
    p[1]=0;
    v=1.0;
    epsilon=0.5;
    state=0;
    p0[SASHAY_CENTER][0]=0;
    p0[SASHAY_CENTER][1]=0;

    p0[SASHAY_LIFT_LEG1][0]=cx+wx/2;
    p0[SASHAY_LIFT_LEG1][1]=cx-wy/2;

    p0[SASHAY_LIFT_LEG2][0]=cx-wx/2;
    p0[SASHAY_LIFT_LEG2][1]=cx-wy/2;

    p0[SASHAY_LIFT_LEG3][0]=cx-wx/2;
    p0[SASHAY_LIFT_LEG3][1]=cx+wy/2;

    p0[SASHAY_LIFT_LEG4][0]=cx+wx/2;
    p0[SASHAY_LIFT_LEG4][1]=cx+wy/2;
  }

  void update(double dt)
  {
    double dx=p[0]-x0[state][0];
    double dy=p[1]-y0[state][1];
    double d=sqrt(dx*dx+dy*dy);
    if (d > v*dt) {
      double scale=v*dt/d;
      dx=scale*dx;
      dy=scale*dy;
    };
    p[0] += dx;
    p[1] += dy;
  }

  bool ready()
  {
    return pow(p[0]-p0[state][0],2)+pow(p[1]-p0[state][1],2) < pow(epsilon,2);
  }
};

class Stepper
{
public:
  int state;
  double v;
  double epsilon;

  enum { 
    STEP_HOME,
    STEP_FORWARD,
    STEP_DOWN,
    STEP_BACK,
    STEP_UP,
    STEP_STATES
  };

  double p[3];
  int sx,sy;
  double curvature;
  double length;

  void init(int leg, double cx, double cy, double omega, double wx, double wy, double heightAboveGround, double stepAboveGround)
  {
    double theta = 3.0*M_PI/4.0-M_PI/2.0*leg;
    sx = copysign(1.0,cos(theta));
    sy = copysign(1.0,sin(theta));

    p0[STEP_HOME][0]=sx*wx/2;
    p0[STEP_HOME][1]=sx*wy/2;
    p0[STEP_HOME][2]=-heightAboveGround;
    
    p0[STEP_FORWARD][0]=sx*wx/2+((sx > 0) ? right : left)/2;
    p0[STEP_FORWARD][1]=sy*wy/2+((sx > 0) ? right : left)/2;
    p0[STEP_FORWARD][2]=-heightAboveGround;
    
    
  }
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

  ServoControllerSP servos;
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

  bool stepMove(double radius,double x,double y,double z,double xstep,double ystep,double zAdder,double left,double right) {
  vector<vector<double>> data;
  double T = 10.0; // 10 is good
  double steps=T*4.0; // 8s in ten of a second steps;
  double fullCircle=2.0*M_PI;
  double da=fullCircle/steps;
  double waist=2.0;
  double dt=0.1;
//  double xAdder=6.6-radius*0.707;
//  double yAdder=6.6-radius*0.707;
  double xAdder=0.0; // Leg up and down not out.
  double yAdder=0.0;
  double stepTime=1.0;  // 2 is good 
  double timeDivider=10.0;
  bool l1=true;
  bool l2=true;
  bool l3=true;
  bool l4=true;
  double t=0.0;
  double l1x=-x; double l1y=y;  double l1z=z;  // default positions
  double l2x=x;  double l2y=y;  double l2z=z;
  double l3x=x;  double l3y=-y; double l3z=z;
  double l4x=-x; double l4y=-y; double l4z=z;
  double dl1x=-xAdder; double dl1y=yAdder;  double dl1z=zAdder;  // default offset for up leg
  double dl2x=xAdder;  double dl2y=yAdder;  double dl2z=zAdder;
  double dl3x=xAdder;  double dl3y=-yAdder; double dl3z=zAdder;
  double dl4x=-xAdder; double dl4y=-yAdder; double dl4z=zAdder;
 // cout <<  "t,pl1,pl2,pl3,pl4" << endl;
  double r=radius/2.0;
  double R=radius;
  double d=-1.0;
  for(double a=0;a<fullCircle;a+=da) {
    double dx=radius*cos(a); double dy=radius*sin(a); // offset of center of mass
    //    double dx=(R-r)*cos(a)+d*cos(a*(R-r)/r);  // Ellispe to make hip swish more back and forth not side to side
    //    double dy=(R-r)*sin(a)-d*sin(a*(R-r)/r);
	double phase=(1.0-a/fullCircle);
	double pl2=phase+1.0/8.0;  // Leg 2 first at 1/8 phase of circle
	pl2 = pl2-floor(pl2);
	double pl1=phase+1.0/8.0+1.0/4.0;  // Leg 1 1/4 phase later
	pl1 = pl1-floor(pl1);
	double pl4=phase+1.0/8.0+2.0/4.0;  // Leg 4 1/2 phase later
	pl4 = pl4-floor(pl4);
	double pl3=phase+1.0/8.0+3.0/4.0;  // Leg 3 3/4 phase later
	pl3 = pl3-floor(pl3);
	if (a<fullCircle/2) waist=-0.0;
	else waist=0.0;
//	cout << t << "," << pl1 << "," << pl2 << "," << pl3 << "," << pl4 << endl;
    double dx1=dx+pl1*xstep;  // x offset for leg moving sideway in walk
    double dx2=dx+pl2*xstep;
    double dx3=dx+pl3*xstep;
    double dx4=dx+pl4*xstep;
    double dy1=dy+pl1*ystep*left;  // y offset for leg moving forward in walk
    double dy2=dy+pl2*ystep*right;
    double dy3=dy+pl3*ystep*right;
    double dy4=dy+pl4*ystep*left;
    vector<double> p;
    p.push_back(t);
    p.push_back(l1x+dx1); p.push_back(l1y+dy1); p.push_back(l1z);
    p.push_back(l2x+dx2); p.push_back(l2y+dy2); p.push_back(l2z);
    p.push_back(l3x+dx3); p.push_back(l3y+dy3); p.push_back(l3z);
    p.push_back(l4x+dx4); p.push_back(l4y+dy4); p.push_back(l4z);
    p.push_back(waist);
    data.push_back(p);
    t+=dt;
    if (fabs(a-M_PI/4.0)<da/2.0 && l2) {  // Leg 2 up
      l2=false;
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx1); pu.push_back(l1y+dy1); pu.push_back(l1z);
        pu.push_back(l2x+dx2+dl2x*f); pu.push_back(l2y+dl2y*f+dy2); pu.push_back(l2z+dl2z*f);
        pu.push_back(l3x+dx3); pu.push_back(l3y+dy3); pu.push_back(l3z);
        pu.push_back(l4x+dx4); pu.push_back(l4y+dy4); pu.push_back(l4z);
        pu.push_back(waist);
        data.push_back(pu);
        t+=dt;
      }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1); pd.push_back(l1y+dy1); pd.push_back(l1z);
        pd.push_back(l2x+dx2+dl2x*f+xstep); pd.push_back(l2y+dy2+dl2y*f+ystep); pd.push_back(l2z+dl2z*f);
        pd.push_back(l3x+dx3); pd.push_back(l3y+dy3); pd.push_back(l3z);
        pd.push_back(l4x+dx4); pd.push_back(l4y+dy4); pd.push_back(l4z);
        pd.push_back(waist);
        data.push_back(pd);
        t+=dt;
      }
	} else if (fabs(a-3.0*M_PI/4.0)<da/2.0 && l1) {  // Leg 1 up
      l1=false;
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx1+dl1x*f); pu.push_back(l1y+dy1+dl1y*f); pu.push_back(l1z+dl1z*f);
        pu.push_back(l2x+dx2); pu.push_back(l2y+dy2); pu.push_back(l2z);
        pu.push_back(l3x+dx3); pu.push_back(l3y+dy3); pu.push_back(l3z);
        pu.push_back(l4x+dx4); pu.push_back(l4y+dy4); pu.push_back(l4z);
        pu.push_back(waist);
        data.push_back(pu);
        t+=dt;
      }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1+dl1x*f+xstep); pd.push_back(l1y+dy1+dl1y*f+ystep); pd.push_back(l1z+dl1z*f);
        pd.push_back(l2x+dx2); pd.push_back(l2y+dy2); pd.push_back(l2z);
        pd.push_back(l3x+dx3); pd.push_back(l3y+dy3); pd.push_back(l3z);
        pd.push_back(l4x+dx4); pd.push_back(l4y+dy4); pd.push_back(l4z);
        pd.push_back(waist);
        data.push_back(pd);
        t+=dt;
	  }
	} else if (fabs(a-5.0*M_PI/4.0)<da/2.0 && l4) {  //  Leg 4 up
	  l4=false;
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx1); pu.push_back(l1y+dy1); pu.push_back(l1z);
        pu.push_back(l2x+dx2); pu.push_back(l2y+dy2); pu.push_back(l2z);
        pu.push_back(l3x+dx3); pu.push_back(l3y+dy3); pu.push_back(l3z);
        pu.push_back(l4x+dx4+dl4x*f); pu.push_back(l4y+dy4+dl4y*f); pu.push_back(l4z+dl4z*f);
        pu.push_back(waist);
        data.push_back(pu);
        t+=dt;
	  }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1); pd.push_back(l1y+dy1); pd.push_back(l1z);
        pd.push_back(l2x+dx2); pd.push_back(l2y+dy2); pd.push_back(l2z);
        pd.push_back(l3x+dx3); pd.push_back(l3y+dy3); pd.push_back(l3z);
        pd.push_back(l4x+dx4+dl4x*f+xstep); pd.push_back(l4y+dy4+dl4y*f+ystep); pd.push_back(l4z+dl4z*f);
        pd.push_back(waist);
        data.push_back(pd);
        t+=dt;
      }
	} else if (fabs(a-7.0*M_PI/4.0)<da/2.0 && l3) {  // Leg3 up
	  l3=false;
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx1); pu.push_back(l1y+dy1); pu.push_back(l1z);
        pu.push_back(l2x+dx2); pu.push_back(l2y+dy2); pu.push_back(l2z);
        pu.push_back(l3x+dx3+dl3x*f); pu.push_back(l3y+dy3+dl3y*f); pu.push_back(l3z+dl3z*f);
        pu.push_back(l4x+dx4); pu.push_back(l4y+dy4); pu.push_back(l4z);
        pu.push_back(waist);
        data.push_back(pu);
        t+=dt;
      }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1); pd.push_back(l1y+dy1); pd.push_back(l1z);
        pd.push_back(l2x+dx2); pd.push_back(l2y+dy2); pd.push_back(l2z);
        pd.push_back(l3x+dx3+dl3x*f+xstep); pd.push_back(l3y+dy3+dl3y*f+ystep); pd.push_back(l3z+dl3z*f);
        pd.push_back(l4x+dx4); pd.push_back(l4y+dy4); pd.push_back(l4z);
        pd.push_back(waist);
        data.push_back(pd);
        t+=dt;
	  }
	}
  }
  logPosition(data);
  fromTips(data);
  return true;
}


MoverSP CreateWalkMover()
{
  return MoverSP(new WalkMover());
}

#endif
