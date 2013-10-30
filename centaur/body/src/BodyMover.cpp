#include <vector>
#include <string>
#include <fstream>

#include "BodyMover.h"
#include "CSVRead.h"
#include "BodyGlobals.h"
#include "split.h"
#include <cmath>
#include <sstream>

using namespace std;

BodyMover::BodyMover()
  : legs(this)
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

bool BodyMover::stepMove(double radius,double x,double y,double z,double xstep,double ystep,double zAdder,double left,double right,bool narrow) {
  vector<vector<double>> data;
//  x-=xstep/2.0;
//  y-=ystep/2.0;
  double T = 6.0; // 10 is good
  double timeDivider=10.0;
  double steps=T*timeDivider; 
  double fullCircle=2.0*M_PI;
  double da=fullCircle/steps;
  double waist=-2.0;
  double dt=0.1;
//  double xAdder=6.6-radius*0.707;
//  double yAdder=6.6-radius*0.707;
  double xAdder=0.0; // Leg up and down not out.
  double yAdder=0.0;
  double stepTime=2.0;  // 2 is good 
  bool l1=true;
  bool l2=true;
  bool l3=true;
  bool l4=true;
  double t=0.0;
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
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1); pd.push_back(l1y+dy1); pd.push_back(l1z);
        pd.push_back(l2x+dx2+dl2x*f+right*xstep); pd.push_back(l2y+dy2+dl2y*f+right*ystep); pd.push_back(l2z+dl2z*f);
        pd.push_back(l3x+dx3); pd.push_back(l3y+dy3); pd.push_back(l3z);
        pd.push_back(l4x+dx4); pd.push_back(l4y+dy4); pd.push_back(l4z);
        pd.push_back(waist);
	pd.push_back(0.0); pd.push_back((f>percentDown) ? -1:0); pd.push_back(0.0); pd.push_back(0.0);
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
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1+dl1x*f+left*xstep); pd.push_back(l1y+dy1+dl1y*f+left*ystep); pd.push_back(l1z+dl1z*f);
        pd.push_back(l2x+dx2); pd.push_back(l2y+dy2); pd.push_back(l2z);
        pd.push_back(l3x+dx3); pd.push_back(l3y+dy3); pd.push_back(l3z);
        pd.push_back(l4x+dx4); pd.push_back(l4y+dy4); pd.push_back(l4z);
        pd.push_back(waist);
    pd.push_back((f>percentDown) ? -1:0); pd.push_back(0.0); pd.push_back(0.0); pd.push_back(0.0);
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
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1); pd.push_back(l1y+dy1); pd.push_back(l1z);
        pd.push_back(l2x+dx2); pd.push_back(l2y+dy2); pd.push_back(l2z);
        pd.push_back(l3x+dx3); pd.push_back(l3y+dy3); pd.push_back(l3z);
        pd.push_back(l4x+dx4+dl4x*f+left*xstep); pd.push_back(l4y+dy4+dl4y*f+left*ystep); pd.push_back(l4z+dl4z*f);
        pd.push_back(waist);
    pd.push_back(0.0); pd.push_back(0.0); pd.push_back(0.0); pd.push_back((f>percentDown) ? -1:0);
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
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx1); pd.push_back(l1y+dy1); pd.push_back(l1z);
        pd.push_back(l2x+dx2); pd.push_back(l2y+dy2); pd.push_back(l2z);
        pd.push_back(l3x+dx3+dl3x*f+right*xstep); pd.push_back(l3y+dy3+dl3y*f+right*ystep); pd.push_back(l3z+dl3z*f);
        pd.push_back(l4x+dx4); pd.push_back(l4y+dy4); pd.push_back(l4z);
        pd.push_back(waist);
    pd.push_back(0.0); pd.push_back(0.0); pd.push_back((f>percentDown) ? -1:0); pd.push_back(0.0);
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
  logPosition(data);
  fromTips(data);
  return true;
}

ServoMover* BodyMover::getMover(const std::string &name)
{
  if (name == "LEFTARM_SHOULDER_IO") return &left.leftRight;
  if (name == "LEFTARM_SHOULDER_UD") return &left.upDown;
  if (name == "LEFTARM_BICEP_ROTATE") return &left.bicep;
  if (name == "LEFTARM_ELBOW") return &left.elbow;
  if (name == "LEFTARM_FOREARM_ROTATE") return &left.forearm;
  if (name == "LEFTARM_TRIGGER") return &left.trigger;  
  if (name == "LEFTARM_MIDDLE") return &left.middle;  
  if (name == "LEFTARM_RING") return &left.ring;
  if (name == "LEFTARM_THUMB") return &left.thumb;

  if (name == "RIGHTARM_SHOULDER_IO") return &right.leftRight;
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
