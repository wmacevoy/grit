#include <vector>
#include <string>

#include "BodyMover.h"
#include "CSVRead.h"
#include "BodyGlobals.h"
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
  double steps=4.0/10.0; // 4s in ten of a second steps;
  double da=2.0*M_PI/steps;
  double waist=0;
  for(double a=0;a<2.0*M_PI;a+=da) {
	double t=4.0*a/(2.0*M_PI);
    double l1x=-x; double l1y=y;  double l1z=z;  // default positions
    double l2x=x;  double l2y=y;  double l2z=z;
    double l3x=x;  double l3y=-y; double l3z=z;
    double l4x=-x; double l4y=-y; double l4z=z;
    double dx=r*cos(a); double dy=r*sin(a); // offset of center of mass
    vector<double> p;
    p.push_back(t);
    p.push_back(l1x+dx); p.push_back(l1y+dy); p.push_back(l1z);
    p.push_back(l2x+dx); p.push_back(l2y+dy); p.push_back(l2z);
    p.push_back(l3x+dx); p.push_back(l3y+dy); p.push_back(l3z);
    p.push_back(l4x+dx); p.push_back(l4y+dy); p.push_back(l4z);
    p.push_back(waist);
    data.push_back(p);
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

  return 0;
}

bool BodyMover::play(const std::string &file)
{
  vector<vector<double>> data;

  ostringstream oss;
  oss << "time";
  for (std::map < int , SPServo > :: iterator i = servos.begin();  i!=servos.end(); ++i) {
    oss << "," << i->first;
  }
  
  string headers = oss.str();
  
  if (!CSVRead(file,headers,data)) {
    cout << "CSVRead failed " << endl;
    return false;
  }
  
  int nr=data.size();
  int c=0;

  double T = data[nr-1][0]-data[0][0]+(data[nr-1][0]-data[nr-2][0]);
  
  for (std::map < int , SPServo > :: iterator i = servos.begin();  i!=servos.end(); ++i) {
    ++c;
    string name = cfg->servo(i->first,"name");
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
  return false;
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
