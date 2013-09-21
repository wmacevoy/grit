#include <vector>
#include <string>

#include "BodyMover.h"
#include "CSVRead.h"
#include "BodyGlobals.h"
#include <cmath>

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
