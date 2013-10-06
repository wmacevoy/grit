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
  double da=2.0*M_PI/steps;
  double waist=0;
  double toffset=0;
  double lift=5.0;
  double stepTime=1.0;
  for(double a=0;a<2.0*M_PI;a+=da) {
	double t=T*a/(2.0*M_PI)+toffset;
    double l1x=-x; double l1y=y;  double l1z=z;  // default positions
    double l2x=x;  double l2y=y;  double l2z=z;
    double l3x=x;  double l3y=-y; double l3z=z;
    double l4x=-x; double l4y=-y; double l4z=z;
    double dx=r*cos(a); double dy=r*sin(a); // offset of center of mass
    vector<double> p;
    p.push_back(t+toffset);
    p.push_back(l1x+dx); p.push_back(l1y+dy); p.push_back(l1z);
    p.push_back(l2x+dx); p.push_back(l2y+dy); p.push_back(l2z);
    p.push_back(l3x+dx); p.push_back(l3y+dy); p.push_back(l3z);
    p.push_back(l4x+dx); p.push_back(l4y+dy); p.push_back(l4z);
    p.push_back(waist);
    data.push_back(p);
    if (fabs(a-M_PI/4.0)<0.1) {  // Leg 2 up
      for (int i=0;i<10;i++) {
        vector<double> pu;
        pu.push_back(t+((float)i)*stepTime/10.0+toffset);
        pu.push_back(l1x+dx); pu.push_back(l1y+dy); pu.push_back(l1z);
        pu.push_back(l2x+dx); pu.push_back(l2y+dy); pu.push_back(l2z+lift);
        pu.push_back(l3x+dx); pu.push_back(l3y+dy); pu.push_back(l3z);
        pu.push_back(l4x+dx); pu.push_back(l4y+dy); pu.push_back(l4z);
        pu.push_back(waist);
        data.push_back(pu);
      }
      for (int i=0;i<10;i++) {
        vector<double> pd;
        pd.push_back(t+1.0+((float)i)*stepTime/10.0+toffset);
        pd.push_back(l1x+dx); pd.push_back(l1y+dy); pd.push_back(l1z);
        pd.push_back(l2x+dx); pd.push_back(l2y+dy); pd.push_back(l2z);
        pd.push_back(l3x+dx); pd.push_back(l3y+dy); pd.push_back(l3z);
        pd.push_back(l4x+dx); pd.push_back(l4y+dy); pd.push_back(l4z);
        pd.push_back(waist);
        data.push_back(pd);
      }
	  toffset+=2.0*stepTime;
	} else if (fabs(a-3.0*M_PI/4.0)<0.1) {  // Leg 1 up
      for (int i=0;i<10;i++) {
        vector<double> pu;
        pu.push_back(t+((float)i)*stepTime/10.0+toffset);
        pu.push_back(l1x+dx); pu.push_back(l1y+dy); pu.push_back(l1z+lift);
        pu.push_back(l2x+dx); pu.push_back(l2y+dy); pu.push_back(l2z);
        pu.push_back(l3x+dx); pu.push_back(l3y+dy); pu.push_back(l3z);
        pu.push_back(l4x+dx); pu.push_back(l4y+dy); pu.push_back(l4z);
        pu.push_back(waist);
        data.push_back(pu);
      }
      for (int i=0;i<10;i++) {
        vector<double> pd;
        pd.push_back(t+1.0+((float)i)*stepTime/10.0+toffset);
        pd.push_back(l1x+dx); pd.push_back(l1y+dy); pd.push_back(l1z);
        pd.push_back(l2x+dx); pd.push_back(l2y+dy); pd.push_back(l2z);
        pd.push_back(l3x+dx); pd.push_back(l3y+dy); pd.push_back(l3z);
        pd.push_back(l4x+dx); pd.push_back(l4y+dy); pd.push_back(l4z);
        pd.push_back(waist);
        data.push_back(pd);
	  }
	  toffset+=2.0*stepTime;
	} else if (fabs(a-5.0*M_PI/4.0)<0.1) {  //  Leg 4 up
      for (int i=0;i<10;i++) {
        vector<double> pu;
        pu.push_back(t+((float)i)*stepTime/10.0+toffset);
        pu.push_back(l1x+dx); pu.push_back(l1y+dy); pu.push_back(l1z);
        pu.push_back(l2x+dx); pu.push_back(l2y+dy); pu.push_back(l2z);
        pu.push_back(l3x+dx); pu.push_back(l3y+dy); pu.push_back(l3z);
        pu.push_back(l4x+dx); pu.push_back(l4y+dy); pu.push_back(l4z+lift);
        pu.push_back(waist);
        data.push_back(pu);
	  }
      for (int i=0;i<10;i++) {
        vector<double> pd;
        pd.push_back(t+1.0+((float)i)*stepTime/10.0+toffset);
        pd.push_back(l1x+dx); pd.push_back(l1y+dy); pd.push_back(l1z);
        pd.push_back(l2x+dx); pd.push_back(l2y+dy); pd.push_back(l2z);
        pd.push_back(l3x+dx); pd.push_back(l3y+dy); pd.push_back(l3z);
        pd.push_back(l4x+dx); pd.push_back(l4y+dy); pd.push_back(l4z);
        pd.push_back(waist);
        data.push_back(pd);
      }
	  toffset+=2.0*stepTime;
	} else if (fabs(a-7.0*M_PI/4.0) <0.1) {  // Leg3 up
      for (int i=0;i<10;i++) {
        vector<double> pu;
        pu.push_back(t+((float)i)*stepTime/10.0+toffset);
        pu.push_back(l1x+dx); pu.push_back(l1y+dy); pu.push_back(l1z);
        pu.push_back(l2x+dx); pu.push_back(l2y+dy); pu.push_back(l2z);
        pu.push_back(l3x+dx); pu.push_back(l3y+dy); pu.push_back(l3z+lift);
        pu.push_back(l4x+dx); pu.push_back(l4y+dy); pu.push_back(l4z);
        pu.push_back(waist);
        data.push_back(pu);
      }  
      for (int i=0;i<10;i++) {
        vector<double> pd;
        pd.push_back(t+1.0+((float)i)*stepTime/10.0+toffset);
        pd.push_back(l1x+dx); pd.push_back(l1y+dy); pd.push_back(l1z);
        pd.push_back(l2x+dx); pd.push_back(l2y+dy); pd.push_back(l2z);
        pd.push_back(l3x+dx); pd.push_back(l3y+dy); pd.push_back(l3z);
        pd.push_back(l4x+dx); pd.push_back(l4y+dy); pd.push_back(l4z);
        pd.push_back(waist);
        data.push_back(pd);
	  }
	  toffset+=2.0*stepTime;
	}
  }
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
