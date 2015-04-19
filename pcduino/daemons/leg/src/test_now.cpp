#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "now.h"

using namespace std;

int main(int argc, char **argv)
{
  double t0=now();
  double dt=0.314;
  usleep(int(dt*1000000));
  double t1=now();
  assert(fabs((t1-t0)-dt) < 0.05);
  cout << "ok" << endl;
  return 0;
}
