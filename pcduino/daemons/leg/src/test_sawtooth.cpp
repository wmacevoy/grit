#include <math.h>
#include <iostream>
#include <fstream>

#include "Sawtooth.h"

using namespace std;

int main()
{
  // length of full sawtooth step
  double L = 2*M_PI;

  // width of mollifying  (<= L)
  double w = M_PI_4;

  Sawtooth s(L,w);

  int n=100;

  ofstream out("sawtooth.csv");
  for (int i=-n; i<=2*n; ++i) {
    double x=L*double(i)/double(n);
    out << x << "," << s(x) << endl;
  }
}
