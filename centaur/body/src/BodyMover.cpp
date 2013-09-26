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

bool BodyMover::circle(float r,float x,float y,float z) {
  vector<vector<double>> data;
  float steps=4.0/10.0; // 4s in ten of a second steps;
  float da=2.0*M_PI/steps;
  float waist=0;
  for(float a=0;a<2.0*M_PI;a+=da) {
	float t=4.0*a/(2.0*M_PI);
    float l1x=-x;
    float l1y=y;
    float l1z=z;
    float l2x=x;
    float l2y=y;
    float l2z=z;
    float l3x=x;
    float l3y=-y;
    float l3z=z;
    float l4x=-x;
    float l4y=-y;
    float l4z=z;
  }
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
      mover->setup(T,angles,simTime,simTime+T);
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
    return true;
}
