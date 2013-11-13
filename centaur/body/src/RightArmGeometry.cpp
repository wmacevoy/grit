#include <math.h>
#include <iostream>
#include <assert.h>

#include "RightArmGeometry.h"
#include "BodyGlobals.h"
#include "ik_rightarm.h"

using namespace std;

RightArmGeometry::RightArmGeometry()
{
  name("RIGHTARM");
  memset(&globals,0,sizeof(ik_rightarmglobals));
  memset(&parameters,0,sizeof(ik_rightarmparameters));
  memset(&variables,0,sizeof(ik_rightarmvariables));
  
  globals.as_struct.epsilon=cfg->num("ik.epsilon");
  globals.as_struct.steps=cfg->num("ik.steps");
}

void RightArmGeometry::forward()
{
    float residuals[ik_rightarmequation_count];

    memset(&parameters,0,sizeof(ik_rightarmparameters));
    memset(&variables,0,sizeof(ik_rightarmvariables));
    parameters.as_struct.waist=mover->waist.angle();
    variables.as_struct._shoulderio=mover->right.inOut.angle();
    variables.as_struct._shoulderud=mover->right.upDown.angle();
    variables.as_struct._bicep=mover->right.bicep.angle();
    variables.as_struct._elbow=mover->right.elbow.angle();
    variables.as_struct._forearm=mover->right.forearm.angle();

    ik_rightarmf(globals.as_array,parameters.as_array,variables.as_array,residuals);

    parameters.as_struct.shoulderio=variables.as_struct._shoulderio;
    parameters.as_struct.shoulderud=variables.as_struct._shoulderud;
    parameters.as_struct.bicep=variables.as_struct._bicep;
    parameters.as_struct.elbow=variables.as_struct._elbow;
    parameters.as_struct.forearm=variables.as_struct._forearm;
    parameters.as_struct.px=residuals[0];
    parameters.as_struct.py=residuals[1];
    parameters.as_struct.pz=residuals[2];
    parameters.as_struct.pointx=residuals[3];
    parameters.as_struct.downx=-residuals[4];

    ik_rightarmf(globals.as_array,parameters.as_array,variables.as_array,residuals);
    float maxresidual=0;
    for (int i=0; i<ik_rightarmequation_count; ++i) {
      if (maxresidual < fabs(residuals[i])) maxresidual=fabs(residuals[i]);
    }
    assert(maxresidual <= globals.as_struct.epsilon);
}

bool RightArmGeometry::inverse()
{
  ik_rightarmparameters p;
  memcpy(&p,&parameters,sizeof(ik_rightarmparameters));
  ik_rightarmupdate(globals.as_array,p.as_array);
  if (p.as_struct.residual <= globals.as_struct.epsilon) {
    memcpy(&parameters,&p,sizeof(ik_rightarmparameters));
    return true;
  } else {
    return false;
  }
}

void RightArmGeometry::compute(float x,float y,float z,float pointx, float downx, float &shoulderio,float &shoulderud,float &bicep, float &elbow, float &forearm)
{
  float dist = 0;
  dist = fmax(dist, 2.54*fabs(x-parameters.as_struct.px));
  dist = fmax(dist, 2.54*fabs(y-parameters.as_struct.py));
  dist = fmax(dist, 2.54*fabs(z-parameters.as_struct.pz));
  dist = fmax(dist, (180/M_PI)*fabs(asin(downx)-asin(parameters.as_struct.downx)));
  dist = fmax(dist, (180/M_PI)*fabs(asin(pointx)-asin(parameters.as_struct.pointx)));

  int n = ceil(dist/10);

  ik_rightarmparameters p,q;
  memcpy(&q,&parameters,sizeof(ik_rightarmparameters));
  memcpy(&p,&parameters,sizeof(ik_rightarmparameters));

  for (int i=1; i<=n; ++i) {
    double s = double(i)/double(n);
    double t = double(n-i)/double(n);

    p.as_struct.px=t*parameters.as_struct.px+s*x;
    p.as_struct.py=t*parameters.as_struct.py+s*y;
    p.as_struct.pz=t*parameters.as_struct.pz+s*z;
    p.as_struct.pointx=t*parameters.as_struct.pointx+s*pointx;
    p.as_struct.downx=t*parameters.as_struct.downx+s*downx;
    ik_rightarmupdate(globals.as_array,p.as_array);
    if (p.as_struct.residual <= globals.as_struct.epsilon) {
      memcpy(&q,&p,sizeof(ik_rightarmparameters));
    } else {
      if (verbose) {
	cout << "right arm ik did not converge." << endl;
      }
      break;
    }
  }

  memcpy(&parameters,&q,sizeof(ik_rightarmparameters));
  shoulderio=q.as_struct.shoulderio;
  shoulderud=q.as_struct.shoulderud;
  bicep=q.as_struct.bicep;
  elbow=q.as_struct.elbow;
  forearm=q.as_struct.forearm;
}
