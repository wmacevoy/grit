#include "fit.h"
#include "math.h"

static void fit0(double t[3],float p[3],float c[3],float cutoff)
{
  if (fabs(t[0]-t[1]) > 0.001) {
    if (fabs(t[1]-t[2]) > 0.001) {
      c[0]=p[1];
      c[2]=-2.0*((p[0]-p[1])/(t[0]-t[1])-(p[2]-p[1])/(t[2]-t[1]))/(t[2]-t[0]);
      c[1]=(p[0]-p[1])/(t[0]-t[1])-c[2]*(t[0]-t[1])/2.0;
      // linear fallback
      if (fabs((t[2]-t[0])*c[2]) > cutoff*fabs(c[1])) {
	c[2]=0;
	c[1]=(p[0]-p[1])/(t[0]-t[1]);
      }
      if (fabs(c[1]*(t[1]-t[0])) > 270) {
	c[1]=0;
	c[2]=0;
      }
    } else {
      c[0]=p[1];
      c[2]=0;
      c[1]=(p[0]-p[1])/(t[0]-t[1]);
    }
  } else {
    c[0]=p[1];
    c[1]=0.0;
    c[2]=0.0;
  }
}

void fit(double ts[3],float p[3],float c0[3], float c1[3], float cutoff)
{
  fit0(ts,p,c0,cutoff);
  ts[0]=-ts[0];
  ts[1]=-ts[1];
  ts[2]=-ts[2];
  fit0(ts,p,c1,cutoff);
  c1[1]=-c1[1];
  ts[0]=-ts[0];
  ts[1]=-ts[1];
  ts[2]=-ts[2];
}
