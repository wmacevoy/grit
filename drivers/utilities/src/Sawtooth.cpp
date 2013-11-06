#include "Sawtooth.h"

#include <math.h>

static double s(double x) { return x-floor(x); }

Sawtooth::Sawtooth(double L_, double w_) : L(L_), w(w_) {}
double Sawtooth::operator()(double x)
{
  double y=-w/2+L*(1-(s((x-w/2)/L)));
  double s=(y < w/2) ? sin(M_PI*y/w) : 1;
  double c0=(1-((x/L)-floor((x+w/2)/L)));
  double w0=(1-s)/2;
  double c1=(1-((x/L)-floor((x-w/2)/L)));
  double w1=(1+s)/2;
  return w0*c0+w1*c1;
}