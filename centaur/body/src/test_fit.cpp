#include "fit.h"

#include <iostream>
using namespace std;

int main()
{
  double t;
  double ts[3];
  float p[3];
  float c0[3];
  float c1[3];

  for (int i=0; i<3; ++i) {
    t=10+0.1*i;
    ts[i]=t;
    p[i]=t+3*t*t;
  }

  
  fit(ts,p,c0,c1);

  for (int i=0; i<3; ++i) {  
    cout << "p(" << ts[i] << ")=" << p[i] << endl;
  }

  for (int i=0; i<3; ++i) {  
    cout << "c0[i]=" << c0[i] << endl;
  }

  for (int i=0; i<3; ++i) {  
    cout << "c1[i]=" << c1[i] << endl;
  }
  
  return 0;
 }
