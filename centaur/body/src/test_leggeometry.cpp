#include "LegGeometry.h"
#include <iostream>

using namespace std;

int main()
{
  LegGeometry lg;
  float x,y,z;
  float knee,femur,hip;
  float home[4][3];
  float homea[4][3];
  for (int num=0; num<4; ++num) {
    lg.number(num);
    lg.forward3D(0,0,0,home[num][0],home[num][1],home[num][2]);
    lg.compute3D(home[num][0],home[num][1],home[num][2],knee,femur,hip);
    
    cout << "home#" << (num+1) <<": [" << home[num][0] << "," << home[num][1] << "," << home[num][2] << "]" << " knee=" << knee << " femur=" << femur << " hip=" << hip << endl;
  }

  return 0;
}
