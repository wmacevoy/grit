#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <atomic>

#include "BodyMover.h"
#include "CSVRead.h"
#include "BodyGlobals.h"
#include "split.h"
#include "Sawtooth.h"
#include "now.h"
#include <cmath>
#include <sstream>

using namespace std;

WalkParameters::WalkParameters(double newRadius,double x,double y,double newZ,double newStep,double newDirection,double newZStep){
	repeat=1;
	rotation=0.0;
	zOffset=0.0;
    x1=-x; 
    y1=y;
    x2=x;
    y2=y;
    x3=x;
    y3=-y;
    x4=-x;
    y4=-y;
    z=newZ;
    radius=newRadius;
    step=newStep;
    direction=newDirection;
    zStep=newZStep;
}


BodyMover::BodyMover()
  : legs(this), left(this), right(this)
{
}

void BodyMover::move(Body &body)
{
  legs.move(body.legs);
  waist.move(*body.waist);
  neck.move(body.neck);
  left.move(body.left);
  right.move(body.right);
}

#if 0
class Sashay
{
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
  bool ready;
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
    dx=x-x0;
    dy=y-y0;
    d=sqrt(dx*dx+dy*dy);
    ready = (d < r);
  }
};

class Stepper
{
  int state;
  int last_state;

  bool ready;
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
  float home[3]; // home coordinates

  void update(double dt)
  {
    switch(state) {
    case STEP_HOME: home(); break;
    case STEP_FORWARD: forward(); break;
    case STEP_DOWN: down(); break;
    case STEP_BACK: back(); break;
    case STEP_UP: up(); break;
    }
    last_state = state;
  }

  void home()
  {
    p0=home;
    shift();
  }

  void up()
  {
    if (last_state != state) {
      p0=p;
      p0[2] += height;
    }
    shift();
    close();
  }

  void down()
  {
    if (last_state != state) {
      p0=p;
      p0[2] -= 2*height;
    }
    shift();
    ready = is_down;
  }

  void forward()
  {
    if (last_state != state) {
      p0=p;
      p0[0] += v[0];
      p0[1] += v[1];
      p0[2] += v[2];
    }
    shift();
    close();
  }
};

#endif 

void BodyMover::logPosition(vector<vector <double> > data) {
    cout << "Time (seconds),x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,waist,l1ift,l21ift,l31ift,l41ift" << endl;
    for (size_t r=0; r<data.size(); ++r) {
      vector<double> values;
      values=data[r];
      for (size_t d=0;d<values.size();++d) {
		 cout << values[d] << ",";
      }
      cout << endl;
    }
}

void BodyMover::fromTips(vector<vector <double> > data) {
    int nr=data.size();

    // assume regular spacing of samples
    double T = data[nr-1][0]-data[0][0] + (data[1][0]-data[0][0]);

    map < float , pair<Point,int> > t2tips[4];
    map < float , float > t2waist;

    for (size_t r=0; r<data.size(); ++r) {
      for (int el=0; el<4; ++el) {
	pair<Point,int> p;
		  
	float t=data[r][0];
	p.first = Point(data[r][1+3*el],
				     data[r][2+3*el],
				     data[r][3+3*el]);
	p.second = 0;
	if (data[r].size()>13) {
	  p.second=int(data[r][14+el]);			   
	} 
	t2tips[el][t]=p;
	t2waist[t]=data[r][13];
      }
    }
    
    legs.setup(body->legs,t2tips,simTime,simTime+T);
    waist.setup(t2waist,simTime,simTime+T);	
}


vector<vector<double> > BodyMover::createMove(double radius,double x,double y,double z,double xstep,double ystep,double zAdder,double left,double right,bool narrow,int repeat) {
  vector<vector<double>> data;
//  x-=xstep/2.0;
//  y-=ystep/2.0;
  double T = 6.0; // 10 is good
  double timeDivider=10.0;
  double steps=T*timeDivider; 
  double fullCircle=2.0*M_PI;
  double da=fullCircle/steps;
  double waist=-4.0;
  double dt=0.1;
//  double xAdder=6.6-radius*0.707;
//  double yAdder=6.6-radius*0.707;
  double xAdder=0.0; // Leg up and down not out.
  double yAdder=0.0;
  double stepTime=2.0;  // 2 is good 
  double t=0.0;
  for (int q=0;q<repeat;q++) {
  bool l1=true;
  bool l2=true;
  bool l3=true;
  bool l4=true;
//  double l1x=-x; double l1y=y;  double l1z=z;  // default positions
//  double l2x=x;  double l2y=y;  double l2z=z;
//  double l3x=x;  double l3y=-y; double l3z=z;
//  double l4x=-x; double l4y=-y; double l4z=z;
  double l1x=-x; double l1y=y-ystep/2.0;  double l1z=z;  // default positions
  double l2x=x;  double l2y=y-ystep/2.0;  double l2z=z;
  double l3x=x;  double l3y=-(y-ystep/2.0); double l3z=z;
  double l4x=-x; double l4y=-(y-ystep/2.0); double l4z=z;
  if (narrow) {
    l3y-=ystep;	
    l4y-=ystep;	
  }  
  if (!narrow) {
    l1y-=ystep/2.0;	
    l2y-=ystep/2.0;	
  }  
  double dl1x=-xAdder; double dl1y=yAdder;  double dl1z=zAdder;  // default offset for up leg
  double dl2x=xAdder;  double dl2y=yAdder;  double dl2z=zAdder;
  double dl3x=xAdder;  double dl3y=-yAdder; double dl3z=zAdder;
  double dl4x=-xAdder; double dl4y=-yAdder; double dl4z=zAdder;
 // cout <<  "t,pl1,pl2,pl3,pl4" << endl;
  //  double r=radius/2.0;
  //  double R=radius;
  //  double d=-1.0;

	double pl2=1.0/8.0;  // Leg 2 first at 1/8 phase of circle
	//pl2 = pl2-floor(pl2);
	double pl1=1.0/8.0+1.0/4.0;  // Leg 1 1/4 phase later
	//pl1 = pl1-floor(pl1);
	double pl4=1.0/8.0+2.0/4.0;  // Leg 4 1/2 phase later
	//pl4 = pl4-floor(pl4);
	double pl3=1.0/8.0+3.0/4.0;  // Leg 3 3/4 phase later

	double percentDown = 0.25;

  for(double a=0;a<fullCircle;a+=da) {
    double dx=radius*cos(a); double dy=radius*sin(a); // offset of center of mass
//    double dx=(R-r)*cos(a)+d*cos(a*(R-r)/r);  // Ellispe to make hip swish more back and forth not side to side
//    double dy=(R-r)*sin(a)-d*sin(a*(R-r)/r);
//	double phase=(1.0-a/fullCircle);
	//pl3 = pl3-floor(pl3);
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
 //   p.push_back(pl1); p.push_back(pl2); p.push_back(pl3); p.push_back(pl4);
    p.push_back(0.0); p.push_back(0.0); p.push_back(0.0); p.push_back(0.0);
    data.push_back(p);
    t+=dt;
    if (pl2-da/fullCircle <= 0 && l2) {  // Leg 2 up
      l2=false;
	  pl2=1.0;      
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx1); pu.push_back(l1y+dy1); pu.push_back(l1z);
        pu.push_back(l2x+dx2+dl2x*f); pu.push_back(l2y+dl2y*f+dy2); pu.push_back(l2z+dl2z*f);
        pu.push_back(l3x+dx3); pu.push_back(l3y+dy3); pu.push_back(l3z);
        pu.push_back(l4x+dx4); pu.push_back(l4y+dy4); pu.push_back(l4z);
        pu.push_back(waist);
    pu.push_back(0.0); pu.push_back(1.0); pu.push_back(0.0); pu.push_back(0.0);
        data.push_back(pu);
        t+=dt;
      }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
	    double lp=-3.0;
	    if (f>percentDown) lp=-1.0;
	    else if (f>percentDown/2.0) lp=-2.0;
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1); pd.push_back(l1y+dy1); pd.push_back(l1z);
        pd.push_back(l2x+dx2+dl2x*f+right*xstep); pd.push_back(l2y+dy2+dl2y*f+right*ystep); pd.push_back(l2z+dl2z*f);
        pd.push_back(l3x+dx3); pd.push_back(l3y+dy3); pd.push_back(l3z);
        pd.push_back(l4x+dx4); pd.push_back(l4y+dy4); pd.push_back(l4z);
        pd.push_back(waist);
	pd.push_back(0.0); pd.push_back(lp); pd.push_back(0.0); pd.push_back(0.0);
        data.push_back(pd);
        t+=dt;
      }
	} else if (pl1-da/fullCircle <= 0 && l1) {  // Leg 1 up
      l1=false;
	  pl1=1.0;      
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx1+dl1x*f); pu.push_back(l1y+dy1+dl1y*f); pu.push_back(l1z+dl1z*f);
        pu.push_back(l2x+dx2); pu.push_back(l2y+dy2); pu.push_back(l2z);
        pu.push_back(l3x+dx3); pu.push_back(l3y+dy3); pu.push_back(l3z);
        pu.push_back(l4x+dx4); pu.push_back(l4y+dy4); pu.push_back(l4z);
        pu.push_back(waist);
    pu.push_back(1.0); pu.push_back(0.0); pu.push_back(0.0); pu.push_back(0.0);
        data.push_back(pu);
        t+=dt;
      }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
	    double lp=-3.0;
	    if (f>percentDown) lp=-1.0;
	    else if (f>percentDown/2.0) lp=-2.0;
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1+dl1x*f+left*xstep); pd.push_back(l1y+dy1+dl1y*f+left*ystep); pd.push_back(l1z+dl1z*f);
        pd.push_back(l2x+dx2); pd.push_back(l2y+dy2); pd.push_back(l2z);
        pd.push_back(l3x+dx3); pd.push_back(l3y+dy3); pd.push_back(l3z);
        pd.push_back(l4x+dx4); pd.push_back(l4y+dy4); pd.push_back(l4z);
        pd.push_back(waist);
    pd.push_back(lp); pd.push_back(0.0); pd.push_back(0.0); pd.push_back(0.0);
        data.push_back(pd);
        t+=dt;
	  }
	} else if (pl4-da/fullCircle <= 0 && l4) {  //  Leg 4 up
	  l4=false;
	  pl4=1.0;
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx1); pu.push_back(l1y+dy1); pu.push_back(l1z);
        pu.push_back(l2x+dx2); pu.push_back(l2y+dy2); pu.push_back(l2z);
        pu.push_back(l3x+dx3); pu.push_back(l3y+dy3); pu.push_back(l3z);
        pu.push_back(l4x+dx4+dl4x*f); pu.push_back(l4y+dy4+dl4y*f); pu.push_back(l4z+dl4z*f);
        pu.push_back(waist);
    pu.push_back(0.0); pu.push_back(0.0); pu.push_back(0.0); pu.push_back(1.0);
        data.push_back(pu);
        t+=dt;
	  }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
	    double lp=-3.0;
	    if (f>percentDown) lp=-1.0;
	    else if (f>percentDown/2.0) lp=-2.0;
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1); pd.push_back(l1y+dy1); pd.push_back(l1z);
        pd.push_back(l2x+dx2); pd.push_back(l2y+dy2); pd.push_back(l2z);
        pd.push_back(l3x+dx3); pd.push_back(l3y+dy3); pd.push_back(l3z);
        pd.push_back(l4x+dx4+dl4x*f+left*xstep); pd.push_back(l4y+dy4+dl4y*f+left*ystep); pd.push_back(l4z+dl4z*f);
        pd.push_back(waist);
    pd.push_back(0.0); pd.push_back(0.0); pd.push_back(0.0); pd.push_back(lp);
        data.push_back(pd);
        t+=dt;
      }
	} else if (pl3-da/fullCircle <= 0 && l3) {  // Leg3 up
	  l3=false;
	  pl3=1.0;
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx1); pu.push_back(l1y+dy1); pu.push_back(l1z);
        pu.push_back(l2x+dx2); pu.push_back(l2y+dy2); pu.push_back(l2z);
        pu.push_back(l3x+dx3+dl3x*f); pu.push_back(l3y+dy3+dl3y*f); pu.push_back(l3z+dl3z*f);
        pu.push_back(l4x+dx4); pu.push_back(l4y+dy4); pu.push_back(l4z);
        pu.push_back(waist);
    pu.push_back(0.0); pu.push_back(0.0); pu.push_back(1.0); pu.push_back(0.0);
        data.push_back(pu);
        t+=dt;
      }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
	    double lp=-3.0;
	    if (f>percentDown) lp=-1.0;
	    else if (f>percentDown/2.0) lp=-2.0;
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1); pd.push_back(l1y+dy1); pd.push_back(l1z);
        pd.push_back(l2x+dx2); pd.push_back(l2y+dy2); pd.push_back(l2z);
        pd.push_back(l3x+dx3+dl3x*f+right*xstep); pd.push_back(l3y+dy3+dl3y*f+right*ystep); pd.push_back(l3z+dl3z*f);
        pd.push_back(l4x+dx4); pd.push_back(l4y+dy4); pd.push_back(l4z);
        pd.push_back(waist);
    pd.push_back(0.0); pd.push_back(0.0); pd.push_back(lp); pd.push_back(0.0);
        data.push_back(pd);
        t+=dt;
	  }
	  
	}
	double dp=da/fullCircle;
	pl1 -= dp;
	pl2 -= dp;
	pl3 -= dp;
	pl4 -= dp;	
  }
  }
  return data;
}

/* The natural lift by shift body away from the leg being lifted */
double lift(double elevation,double height,double angle) {
  return elevation+height*sin(angle*2.0);
}

/* Extra movement to raise the leg higher than natural */
double raise(double elevation,double angle) {
  double h=0.0;
  double pi_8=M_PI_4/2.0;
  while (angle>2*M_PI) 
    angle-=2.0*M_PI; // wrap around
  while (angle<0) 
    angle+=2.0*M_PI;
  if (5.0*pi_8<angle && angle<7.0*pi_8)  // pi/4 range 
    h=sin(4.0*(angle-5.0*pi_8))*elevation; // map to 0 to pi
  return h;
}

/* Circulation of the center of gravity in the x direction */
double circulateX(double position,double radius,double angle) {
  return position+radius*cos(angle);
}
 
/* Circulation of the center of gravity in the y direction */
double circulateY(double position,double radius,double angle) {
  return position+radius*sin(angle);
}

/* The function that steps forward with the leg naturally comes up */
double stepX(double dist,double direction,double angle) {
  double xstep=cos(direction)*dist; // Effective step size in x
  while (angle>2.0*M_PI) 
    angle-=2.0*M_PI; // wrap around
  while (angle<0.0) 
    angle+=2.0*M_PI;
  angle-=3.0*M_PI_4; // Subtract off center of raise angle
  if (angle<0.0) angle+=2.0*M_PI;  // Add circle if negative
  // Angle should between 0 and 2Pi. 
  Sawtooth st(2.0*M_PI,M_PI_4/2.0);
  return xstep*st(angle);
//  return xstep*(1.0-angle/(2.0*M_PI));
}

/* The function that steps forward with the leg naturally comes up */
double stepY(double dist,double direction,double angle) {
  double ystep=sin(direction)*dist;  // Effective step size in y
  while (angle>2.0*M_PI) 
    angle-=2.0*M_PI; // wrap around
  while (angle<0.0) 
    angle+=2.0*M_PI;
  angle-=3.0*M_PI_4; // Subtract off center of raise angle
  if (angle<0.0) angle+=2.0*M_PI;  // Add circle if negative
  // Angle should between 0 and 2Pi. 
  Sawtooth st(2.0*M_PI,M_PI_4/2.0);
  return ystep*st(angle);
//  return ystep*(1.0-angle/(2.0*M_PI));
}

/* The function that steps forward with the leg naturally comes up */
double stepZ(double dist,double angle) {
  while (angle>2*M_PI) 
    angle-=2.0*M_PI; // wrap around
  while (angle<0) 
    angle+=2.0*M_PI;
  angle-=3.0*M_PI_4; // Subtract off center of raise angle
  if (angle<0.0) angle+=2.0*M_PI;  // Add circle if negative
  // Angle should between 0 and 2Pi. 
  Sawtooth st(2.0*M_PI,M_PI_4/2.0);
  return dist*st(angle);
//  return dist*(1.0-angle/(2.0*M_PI));
}

double legDirection(double angle) {
  double pi_8=M_PI_4/2.0;
  if (5.0*pi_8<angle && angle<3.0*M_PI_4) return 1.0;
  else if (3.0*M_PI_4<angle && angle<(7.0*pi_8-M_PI/16.0)) return -1.0;
  else if ((7.0*pi_8-M_PI/16.0)<angle && angle<7.0*pi_8) return -2.0;
  else return 0.0; 
}
 
vector<vector<double> > BodyMover::bMove(WalkParameters wp) {
  vector<vector<double>> data;
  double direction=(wp.direction*M_PI)/180.0;
  double T =20.0; 
  double timeDivider=10.0;
  double steps=T*timeDivider; 
  double fullCircle=2.0*M_PI;
  double da=fullCircle/steps;
  double waist=0.0;
  double dt=0.1;
  double t=0.0;
  double l1d=direction; // All same direction is a translation
  double l2d=direction;
  double l3d=direction;
  double l4d=direction;
  double l1z=wp.z/*+zoffset/2.0*/;
  double l2z=wp.z/*+zoffset/2.0*/;
  double l3z=wp.z/*-zoffset/2.0*/;
  double l4z=wp.z/*-zoffset/2.0*/;
  l2d+=wp.rotation*3.0*M_PI_2;
  l3d+=wp.rotation*M_PI;
  l4d+=wp.rotation*M_PI_2;

  double zRise = wp.zOffset * wp.step / (wp.y1 - wp.y4);

  for (int q=0;q<wp.repeat;q++) {
    for(double a=0;a<fullCircle;a+=da) {
	  float l1a=a;
	  float l2a=a+M_PI_2;
	  float l3a=a+M_PI;
	  float l4a=a+3*M_PI_2;
      vector<double> p;
      p.push_back(t);
      { // leg 1
        p.push_back(circulateX(wp.x1,wp.radius,a)+stepX(wp.step,l1d,l1a)); 
        p.push_back(circulateY(wp.y1,wp.radius,a) +stepY(wp.step,l1d,l1a)); 
        p.push_back(lift(l1z,1.0,a+M_PI_2)+raise(wp.zStep,l1a)+stepZ(zRise,l1a)-wp.zOffset/2.0);
      } { // leg 2
        p.push_back(circulateX(wp.x2,wp.radius,a) +stepX(wp.step,l2d,l2a)); 
        p.push_back(circulateY(wp.y2,wp.radius,a) +stepY(wp.step,l2d,l2a)); 
        p.push_back(lift(l2z,1.0,a)       +raise(wp.zStep,l2a)+stepZ(zRise,l2a)-wp.zOffset/2.0);
      } { // leg 3 
        p.push_back(circulateX(wp.x3,wp.radius,a) +stepX(wp.step,l3d,l3a)); 
        p.push_back(circulateY(wp.y3,wp.radius,a)+stepY(wp.step,l3d,l3a)); 
        p.push_back(lift(l3z,1.0,a+M_PI_2)+raise(wp.zStep,l3a)-stepZ(zRise,l3a)+wp.zOffset/2.0);
      } { // leg 4
        p.push_back(circulateX(wp.x4,wp.radius,a)+stepX(wp.step,l4d,l4a)); 
        p.push_back(circulateY(wp.y4,wp.radius,a)+stepY(wp.step,l4d,l4a)); 
        p.push_back(lift(l4z,1.0,a)       +raise(wp.zStep,l4a)-stepZ(zRise,l4a)+wp.zOffset/2.0);
      }         
      p.push_back(waist); 
      p.push_back(legDirection(l1a)); 
      p.push_back(legDirection(l2a)); 
      p.push_back(legDirection(l3a)); 
      p.push_back(legDirection(l4a));
      data.push_back(p);
      t+=dt;
	}
  }
  return data;
}

bool BodyMover::bStep(WalkParameters wp) {
  vector<vector<double > > f;
  f=bMove(wp);
  logPosition(f);
  fromTips(f);
  return true;  
}

bool BodyMover::blended(double radius,double x,double y,double z,double xstep,double ystep,double zAdder,double left,double right,bool narrow,int repeat) {
  vector<vector<double > > f;
  f=createMove(radius,x,y,z,xstep,ystep,zAdder,left,right,narrow,repeat);
  vector<vector<double > > lf;
  lf=createMove(radius,x,y,z,xstep,ystep,zAdder-5.75,left,right,narrow,repeat);
  for (int i=0;i<4;i++) {
	for (size_t t=0;t<lf.size();t++) {
	  size_t t2=t;
	  while (t2<lf.size() && lf[t2][14+i]<0) {
		  t2++; // find the time for the end of the step down
	  }
      if (lf[t][14+i]<0) {
        if (f[t][3+3*i]-5.75>z) {  // coming down and higher than z
          lf[t][1+3*i]=lf[t2][1+3*i]; // x and y are end x and y
          lf[t][2+3*i]=lf[t2][2+3*i];
          lf[t][3+3*i]=f[t][3+3*i]-5.75;
	    } else {
          lf[t][1+3*i]=lf[t2][1+3*i];
          lf[t][2+3*i]=lf[t2][2+3*i];
          lf[t][3+3*i]=z;
	    }
      }
    }
  }
  logPosition(lf);
  fromTips(lf);
  return true;
}


std::vector<std::vector<double> > BodyMover::stepMerge(std::vector<std::vector <double> > start,std::vector<std::vector <double> > end) {
  std::vector<std::vector<double> > data;
  bool ldown[]={false,false,false,false};
  for (size_t i=0;i<start.size();i++) {
	vector<double> temp;
	temp.push_back(start[i][0]);
	for (int j=0;j<4;j++) {
	  if (!ldown[j]) {
	    temp.push_back(start[i][1+j*3]);
	    temp.push_back(start[i][2+j*3]);
	    temp.push_back(start[i][3+j*3]);
	  } else {
	    temp.push_back(end[i][1+j*3]);
	    temp.push_back(end[i][2+j*3]);
	    temp.push_back(end[i][3+j*3]);
	  }
    }
	temp.push_back(start[i][13]); // Waist
	temp.push_back(start[i][14]);
	temp.push_back(start[i][15]);
	temp.push_back(start[i][16]);
	temp.push_back(start[i][17]);
	if (i>0 && start[i-1][14]==1 && start[i][14]<0) ldown[0]=true;
	if (i>0 && start[i-1][15]==1 && start[i][15]<0) ldown[1]=true;
	if (i>0 && start[i-1][16]==1 && start[i][16]<0) ldown[2]=true;
	if (i>0 && start[i-1][17]==1 && start[i][17]<0) ldown[3]=true;
    data.push_back(temp);
  }
  return data;
}

void BodyMover::changeZ(WalkParameters start,double newZ){
  std::vector<std::vector<double> > data;
  double T =20.0; 
  double timeDivider=10.0;
  double steps=T*timeDivider; 
  double dt=0.1;
  double t=0.0;
  double startZ=start.z;
  double endZ=newZ;
  double dz=(endZ-startZ)/steps;
  double z=startZ;
  while (t<T) {
    vector<double> p;
    p.push_back(t);
    p.push_back(start.x1); p.push_back(start.y1); p.push_back(z);
    p.push_back(start.x2); p.push_back(start.y2); p.push_back(z);
    p.push_back(start.x3); p.push_back(start.y3); p.push_back(z);
    p.push_back(start.x4); p.push_back(start.y4); p.push_back(z);
    p.push_back(0);// waist
    p.push_back(0);// Leg is not going up or down
    p.push_back(0);
    p.push_back(0);
    p.push_back(0);
    data.push_back(p);
    z+=dz;
	t+=dt;
  }
  logPosition(data);
  fromTips(data);
}

void BodyMover::stepMerge(WalkParameters start,WalkParameters end) {
  vector<vector<double>> moveStart;
  vector<vector<double>> moveEnd;
  vector<vector<double>> move;
  start.repeat=1;
  end.repeat=1;
  moveStart=bMove(start);
  moveEnd=bMove(end);
  move=stepMerge(moveStart,moveEnd);
  logPosition(move);
  fromTips(move);
}

bool BodyMover::stepMove(double radius,double x,double y,double z,double xstep,double ystep,double zAdder,double left,double right,bool narrow,int repeat) {
  vector<vector<double>> data;
  data=createMove(radius,x,y,z,xstep,ystep,zAdder,left,right,narrow,repeat);
  logPosition(data);
  fromTips(data);
  return true;
}

//EXPERIMENT - TIME TO RUN!
std::thread *walkThread;
std::atomic<bool> walking;

void BodyMover::dynamicWalk(void* _wp) {
  WalkParameters wp = *(WalkParameters*)_wp;

  vector<vector<double>> data;
  double direction=(wp.direction*M_PI)/180.0;
  double T =20.0; 
  double timeDivider=10.0;
  double steps=T*timeDivider; 
  double fullCircle=2.0*M_PI;
  double da=fullCircle/steps;
  double waist=0.0;
  double dt=0.1;
  double t=0.0;
  double a = 0.0; //Current index of a regular walk, same as in the original -- for(double a=0;a<fullCircle;a+=da)
  double l1d=direction; // All same direction is a translation
  double l2d=direction;
  double l3d=direction;
  double l4d=direction;
  double l1z=wp.z/*+zoffset/2.0*/;
  double l2z=wp.z/*+zoffset/2.0*/;
  double l3z=wp.z/*-zoffset/2.0*/;
  double l4z=wp.z/*-zoffset/2.0*/;
  l2d+=wp.rotation*3.0*M_PI_2;
  l3d+=wp.rotation*M_PI;
  l4d+=wp.rotation*M_PI_2;

  double zRise = wp.zOffset * wp.step / (wp.y1 - wp.y4);

  //Keep track of the wall clock time passing
  double t1 = 0.0, t2 = 0.0, framerate = 0.1;

  float l1a;
  float l2a;
  float l3a;
  float l4a;

  while(walking) {
      vector<double> p;
      /*Check bots sensors to determine next move
            Function to check leg pressures, determine which legs should have pressure and adjust if needed
            Function to check accelerometers and level chassis accordingly using zOffset
      */

      l1a=a;
      l2a=a+M_PI_2;
      l3a=a+M_PI;
      l4a=a+3*M_PI_2;
 
			//Regulate how fast moves are generated. This may need to be placed elsewhere if at all needed?
      t1 = now();
      if( (data.size() > 3) || (t1 - t2 >= framerate)) {

		    p.push_back(t);
		    { // leg 1
		      p.push_back(circulateX(wp.x1,wp.radius,a)+stepX(wp.step,l1d,l1a)); 
		      p.push_back(circulateY(wp.y1,wp.radius,a) +stepY(wp.step,l1d,l1a)); 
		      p.push_back(lift(l1z,1.0,a+M_PI_2)+raise(wp.zStep,l1a)+stepZ(zRise,l1a)-wp.zOffset/2.0);
		    } { // leg 2
		      p.push_back(circulateX(wp.x2,wp.radius,a) +stepX(wp.step,l2d,l2a)); 
		      p.push_back(circulateY(wp.y2,wp.radius,a) +stepY(wp.step,l2d,l2a)); 
		      p.push_back(lift(l2z,1.0,a)       +raise(wp.zStep,l2a)+stepZ(zRise,l2a)-wp.zOffset/2.0);
		    } { // leg 3 
		      p.push_back(circulateX(wp.x3,wp.radius,a) +stepX(wp.step,l3d,l3a)); 
		      p.push_back(circulateY(wp.y3,wp.radius,a)+stepY(wp.step,l3d,l3a)); 
		      p.push_back(lift(l3z,1.0,a+M_PI_2)+raise(wp.zStep,l3a)-stepZ(zRise,l3a)+wp.zOffset/2.0);
		    } { // leg 4
		      p.push_back(circulateX(wp.x4,wp.radius,a)+stepX(wp.step,l4d,l4a)); 
		      p.push_back(circulateY(wp.y4,wp.radius,a)+stepY(wp.step,l4d,l4a)); 
		      p.push_back(lift(l4z,1.0,a)       +raise(wp.zStep,l4a)-stepZ(zRise,l4a)+wp.zOffset/2.0);
		    }         

		    p.push_back(waist); 
		    p.push_back(legDirection(l1a)); 
		    p.push_back(legDirection(l2a)); 
		    p.push_back(legDirection(l3a)); 
		    p.push_back(legDirection(l4a));

		    data.push_back(p);
				
				t2 = now();
      }

      //Check if enough data is in the vector to create a curve
      //If so, push it to from tips
      if(data.size() >= 3) {
				if(data.size() > 3)
					{
					data.resize(3);
					}
        logPosition(data);
        fromTips(data);

        //Erase first move to allow for next move to be pushed
        data.erase(data.begin());
      }
      
      t+=dt;

      a+=da;
      if(a>=fullCircle) { a-=2.0 * M_PI; }
  }
}

//END EXPERIMENT

ServoMover* BodyMover::getMover(const std::string &name)
{
  if (name == "LEFTARM_SHOULDER_IO") return &left.inOut;
  if (name == "LEFTARM_SHOULDER_UD") return &left.upDown;
  if (name == "LEFTARM_BICEP_ROTATE") return &left.bicep;
  if (name == "LEFTARM_ELBOW") return &left.elbow;
  if (name == "LEFTARM_FOREARM_ROTATE") return &left.forearm;
  if (name == "LEFTARM_TRIGGER") return &left.trigger;  
  if (name == "LEFTARM_MIDDLE") return &left.middle;  
  if (name == "LEFTARM_RING") return &left.ring;
  if (name == "LEFTARM_THUMB") return &left.thumb;

  if (name == "RIGHTARM_SHOULDER_IO") return &right.inOut;
  if (name == "RIGHTARM_SHOULDER_UD") return &right.upDown;
  if (name == "RIGHTARM_BICEP_ROTATE") return &right.bicep;
  if (name == "RIGHTARM_ELBOW") return &right.elbow;
  if (name == "RIGHTARM_FOREARM_ROTATE") return &right.forearm;
  if (name == "RIGHTARM_TRIGGER") return &right.trigger;  
  if (name == "RIGHTARM_MIDDLE") return &right.middle;  
  if (name == "RIGHTARM_RING") return &right.ring;
  if (name == "RIGHTARM_THUMB") return &right.thumb;

  if (name == "LEG1_KNEE") return &*legs.legMovers[LEG1]->kneeMover;
  if (name == "LEG1_FEMUR") return &*legs.legMovers[LEG1]->femurMover;
  if (name == "LEG1_HIP") return &*legs.legMovers[LEG1]->hipMover;

  if (name == "LEG2_KNEE") return &*legs.legMovers[LEG2]->kneeMover;
  if (name == "LEG2_FEMUR") return &*legs.legMovers[LEG2]->femurMover;
  if (name == "LEG2_HIP") return &*legs.legMovers[LEG2]->hipMover;

  if (name == "LEG3_KNEE") return &*legs.legMovers[LEG3]->kneeMover;
  if (name == "LEG3_FEMUR") return &*legs.legMovers[LEG3]->femurMover;
  if (name == "LEG3_HIP") return &*legs.legMovers[LEG3]->hipMover;

  if (name == "LEG4_KNEE") return &*legs.legMovers[LEG4]->kneeMover;
  if (name == "LEG4_FEMUR") return &*legs.legMovers[LEG4]->femurMover;
  if (name == "LEG4_HIP") return &*legs.legMovers[LEG4]->hipMover;

  if (name == "WAIST") return &waist;

  if (name == "NECKUD") return &neck.upDown;
  if (name == "NECKLR") return &neck.leftRight;

  return 0;
}

bool BodyMover::play(const std::string &file)
{
  vector<string> names;
  vector<vector<double>> data;
  string header;

  { 
    ifstream ifs(file.c_str());
    if (!ifs) { 
      cout << "file '" << file << "' not found." << endl; 
      return false; 
    }
    getline(ifs,header);
    split(header,names);
  }
    
  if (!CSVRead(file,header,data)) {
    cout << "CSVRead of file '" << file << "' failed " << endl;
    return false;
  }
  
  int nr=data.size();
  int nc=data[0].size();
  float T=data[nr-1][0]-data[0][0];

  for (int c=1; c<nc; ++c) {
    string name=names[c];
    if (atoi(name.c_str()) != 0) {
      name = cfg->servo(atoi(name.c_str()),"name");
    }
    ServoMover *mover = getMover(name);
    if (mover == 0) {
      cout << "skipped unknown servo " << name << endl;
    } else {
      std::map < float , float > angles;
      for (int r=0; r<nr; ++r) {
	angles[data[r][0]]=data[r][c];
      }
      mover->setup(angles,simTime,simTime+T);
    }
  }
  return true;
}


bool BodyMover::load(const std::string &file)
{
    vector<vector<double>> data;
    //                             0  1  2  3  4  5  6  7  8  9 10 11 12 13
    string headers=    "Time (seconds),x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,waist";
    if (!CSVRead(file,headers,data)) {
	  cout << "CSVRead failed " << endl;
      return false;
    }

    cout << "read '" << file << "' ok." << endl;
    fromTips(data);
    return true;
}

bool BodyMover::done() const
{
  if (!legs.done()) return false;
  if (!left.done()) return false;
  if (!right.done()) return false;
  if (!waist.done()) return false;
  if (!neck.done()) return false;
  return true;
}
