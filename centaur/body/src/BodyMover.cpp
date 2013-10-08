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

void BodyMover::move(Body &body)
{
  legs.move(body.legs);
  waist.move(*body.waist);
  neck.move(body.neck);
  left.move(body.left);
  right.move(body.right);
}

void BodyMover::logPosition(vector<vector <double> > data) {
    cout << "t,x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,w" << endl;
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

    map < float , Point > t2tips[4];
    map < float , float > t2waist;

    for (size_t r=0; r<data.size(); ++r) {
      for (int el=0; el<4; ++el) {
	float t=data[r][0];
	t2tips[el][t]=Point(data[r][1+3*el],
				     data[r][2+3*el],
				     data[r][3+3*el]);
	t2waist[t]=data[r][13];
      }
    }

    legs.setup(body->legs,t2tips,simTime,simTime+T);
    waist.setup(t2waist,simTime,simTime+T);	
}

bool BodyMover::circle(double r,double x,double y,double z) {
  vector<vector<double>> data;
  double T = 10.0;
  double steps=T*10.0; // 4s in ten of a second steps;
  double fullCircle=2.0*M_PI;
  double da=fullCircle/steps;
  double waist=0.0;
  double dt=0.1;
  double zAdder=10.0; // or zAdder
  double xAdder=6.6-r*0.707;
  double yAdder=6.6-r*0.707;
  double stepTime=5.0;
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
  double dl1x=-xAdder; double dl1y=yAdder;  double dl1z=zAdder;  // default positions
  double dl2x=xAdder;  double dl2y=yAdder;  double dl2z=zAdder;
  double dl3x=xAdder;  double dl3y=-yAdder; double dl3z=zAdder;
  double dl4x=-xAdder; double dl4y=-yAdder; double dl4z=zAdder;
  for(double a=0;a<fullCircle;a+=da) {
    double dx=r*cos(a); double dy=r*sin(a); // offset of center of mass
    vector<double> p;
    p.push_back(t);
    p.push_back(l1x+dx); p.push_back(l1y+dy); p.push_back(l1z);
    p.push_back(l2x+dx); p.push_back(l2y+dy); p.push_back(l2z);
    p.push_back(l3x+dx); p.push_back(l3y+dy); p.push_back(l3z);
    p.push_back(l4x+dx); p.push_back(l4y+dy); p.push_back(l4z);
    p.push_back(waist);
    data.push_back(p);
    t+=dt;
    if (fabs(a-M_PI/4.0)<da && l2) {  // Leg 2 up
      l2=false;
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx); pu.push_back(l1y+dy); pu.push_back(l1z);
        pu.push_back(l2x+dx+dl2x*f); pu.push_back(l2y+dy+dl2y*f); pu.push_back(l2z+dl2z*f);
        pu.push_back(l3x+dx); pu.push_back(l3y+dy); pu.push_back(l3z);
        pu.push_back(l4x+dx); pu.push_back(l4y+dy); pu.push_back(l4z);
        pu.push_back(waist);
        data.push_back(pu);
        t+=dt;
      }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx); pd.push_back(l1y+dy); pd.push_back(l1z);
        pd.push_back(l2x+dx+dl2x*f); pd.push_back(l2y+dy+dl2y*f); pd.push_back(l2z+dl2z*f);
        pd.push_back(l3x+dx); pd.push_back(l3y+dy); pd.push_back(l3z);
        pd.push_back(l4x+dx); pd.push_back(l4y+dy); pd.push_back(l4z);
        pd.push_back(waist);
        data.push_back(pd);
        t+=dt;
      }
	} else if (fabs(a-3.0*M_PI/4.0)<da && l1) {  // Leg 1 up
      l1=false;
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx+dl1x*f); pu.push_back(l1y+dy+dl1y*f); pu.push_back(l1z+dl1z*f);
        pu.push_back(l2x+dx); pu.push_back(l2y+dy); pu.push_back(l2z);
        pu.push_back(l3x+dx); pu.push_back(l3y+dy); pu.push_back(l3z);
        pu.push_back(l4x+dx); pu.push_back(l4y+dy); pu.push_back(l4z);
        pu.push_back(waist);
        data.push_back(pu);
        t+=dt;
      }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx+dl1x*f); pd.push_back(l1y+dy+dl1y*f); pd.push_back(l1z+dl1z*f);
        pd.push_back(l2x+dx); pd.push_back(l2y+dy); pd.push_back(l2z);
        pd.push_back(l3x+dx); pd.push_back(l3y+dy); pd.push_back(l3z);
        pd.push_back(l4x+dx); pd.push_back(l4y+dy); pd.push_back(l4z);
        pd.push_back(waist);
        data.push_back(pd);
        t+=dt;
	  }
	} else if (fabs(a-5.0*M_PI/4.0)<da && l4) {  //  Leg 4 up
	  l4=false;
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx); pu.push_back(l1y+dy); pu.push_back(l1z);
        pu.push_back(l2x+dx); pu.push_back(l2y+dy); pu.push_back(l2z);
        pu.push_back(l3x+dx); pu.push_back(l3y+dy); pu.push_back(l3z);
        pu.push_back(l4x+dx+dl4x*f); pu.push_back(l4y+dy+dl4y*f); pu.push_back(l4z+dl4z*f);
        pu.push_back(waist);
        data.push_back(pu);
        t+=dt;
	  }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx); pd.push_back(l1y+dy); pd.push_back(l1z);
        pd.push_back(l2x+dx); pd.push_back(l2y+dy); pd.push_back(l2z);
        pd.push_back(l3x+dx); pd.push_back(l3y+dy); pd.push_back(l3z);
        pd.push_back(l4x+dx+dl4x*f); pd.push_back(l4y+dy+dl4y*f); pd.push_back(l4z+dl4z*f);
        pd.push_back(waist);
        data.push_back(pd);
        t+=dt;
      }
	} else if (fabs(a-7.0*M_PI/4.0)<da && l3) {  // Leg3 up
	  l3=false;
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=(float)i/(stepTime*timeDivider);
        vector<double> pu;
        pu.push_back(t);
        pu.push_back(l1x+dx); pu.push_back(l1y+dy); pu.push_back(l1z);
        pu.push_back(l2x+dx); pu.push_back(l2y+dy); pu.push_back(l2z);
        pu.push_back(l3x+dx+dl3x*f); pu.push_back(l3y+dy+dl3y*f); pu.push_back(l3z+dl3z*f);
        pu.push_back(l4x+dx); pu.push_back(l4y+dy); pu.push_back(l4z);
        pu.push_back(waist);
        data.push_back(pu);
        t+=dt;
      }
      for (int i=0;i<stepTime*timeDivider;i++) {
	    double f=1.0-(float)i/(stepTime*timeDivider);
        vector<double> pd;
        pd.push_back(t);
        pd.push_back(l1x+dx); pd.push_back(l1y+dy); pd.push_back(l1z);
        pd.push_back(l2x+dx); pd.push_back(l2y+dy); pd.push_back(l2z);
        pd.push_back(l3x+dx+dl3x*f); pd.push_back(l3y+dy+dl3y*f); pd.push_back(l3z+dl3z*f);
        pd.push_back(l4x+dx); pd.push_back(l4y+dy); pd.push_back(l4z);
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

  if (name == "LEG1_KNEE") return &legs.legMovers[LEG1].kneeMover;
  if (name == "LEG1_FEMUR") return &legs.legMovers[LEG1].femurMover;
  if (name == "LEG1_HIP") return &legs.legMovers[LEG1].hipMover;

  if (name == "LEG2_KNEE") return &legs.legMovers[LEG2].kneeMover;
  if (name == "LEG2_FEMUR") return &legs.legMovers[LEG2].femurMover;
  if (name == "LEG2_HIP") return &legs.legMovers[LEG2].hipMover;

  if (name == "LEG3_KNEE") return &legs.legMovers[LEG3].kneeMover;
  if (name == "LEG3_FEMUR") return &legs.legMovers[LEG3].femurMover;
  if (name == "LEG3_HIP") return &legs.legMovers[LEG3].hipMover;

  if (name == "LEG4_KNEE") return &legs.legMovers[LEG4].kneeMover;
  if (name == "LEG4_FEMUR") return &legs.legMovers[LEG4].femurMover;
  if (name == "LEG4_HIP") return &legs.legMovers[LEG4].hipMover;

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
    getline(ifs,header);
    split(header,names);
  }
    
  if (!CSVRead(file,header,data)) {
    cout << "CSVRead failed " << endl;
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
/*  All below copied into fromTips
    //    // add last row to finish cycle
    //data.push_back(data[0]);
    //int nr=data.size();
    //data[nr-1][0]=data[nr-2][0]+(data[nr-2][0]-data[nr-3][0]);

    int nr=data.size();

    // assume regular spacing of samples
    double T = data[nr-1][0]-data[0][0] + (data[1][0]-data[0][0]);

    map < float , Point > t2tips[4];
    map < float , float > t2waist;

    for (size_t r=0; r<data.size(); ++r) {
      for (int el=0; el<4; ++el) {
	float t=data[r][0];
	t2tips[el][t]=Point(data[r][1+3*el],
				     data[r][2+3*el],
				     data[r][3+3*el]);
	t2waist[t]=data[r][13];
      }
    }

    legs.setup(T,body->legs,t2tips,simTime,simTime+T);
    waist.setup(T,t2waist,simTime,simTime+T);
    */ 
    return true;
}
