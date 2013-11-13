#include <math.h>
#include <iostream>
#include <assert.h>

#include "RightArmGeometry.h"
#include "BodyGlobals.h"
#include "ik_leftarm.h"

using namespace std;

LeftArmGeometry::LeftArmGeometry()
{
  name("LEFTARM");
  memset(&globals,0,sizeof(ik_leftarmglobals));
  memset(&parameters,0,sizeof(ik_leftarmparameters));
  memset(&variables,0,sizeof(ik_leftarmvariables));
  
  globals.as_struct.epsilon=cfg->num("ik.epsilon");
  globals.as_struct.steps=cfg->num("ik.steps");
}

void LeftArmGeometry::forward()
{
    float residuals[ik_leftarmequation_count];

    memset(&parameters,0,sizeof(ik_leftarmparameters));
    memset(&variables,0,sizeof(ik_leftarmvariables));
    parameters.as_struct.waist=mover->waist.angle();
    variables.as_struct._shoulderio=mover->left.inOut.angle();
    variables.as_struct._shoulderud=mover->left.upDown.angle();
    variables.as_struct._bicep=mover->left.bicep.angle();
    variables.as_struct._elbow=mover->left.elbow.angle();
    variables.as_struct._forearm=mover->left.forearm.angle();

    ik_leftarmf(globals.as_array,parameters.as_array,variables.as_array,residuals);

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

    ik_leftarmf(globals.as_array,parameters.as_array,variables.as_array,residuals);
    float maxresidual=0;
    for (int i=0; i<ik_leftarmequation_count; ++i) {
      if (maxresidual < fabs(residuals[i])) maxresidual=fabs(residuals[i]);
    }
    assert(maxresidual <= globals.as_struct.epsilon);
}

bool LeftArmGeometry::inverse()
{
  ik_leftarmparameters p;
  memcpy(&p,&parameters,sizeof(ik_leftarmparameters));
  ik_leftarmupdate(globals.as_array,p.as_array);
  if (p.as_struct.residual <= globals.as_struct.epsilon) {
    memcpy(&parameters,&p,sizeof(ik_leftarmparameters));
    return true;
  } else {
    return false;
  }
}

void LeftArmGeometry::compute(float x,float y,float z,float pointx, float downx, float &shoulderio,float &shoulderud,float &bicep, float &elbow, float &forearm)
{
  float dist = 0;
  dist = fmax(dist, 2.54*fabs(x-parameters.as_struct.px));
  dist = fmax(dist, 2.54*fabs(y-parameters.as_struct.py));
  dist = fmax(dist, 2.54*fabs(z-parameters.as_struct.pz));
  dist = fmax(dist, (180/M_PI)*fabs(asin(downx)-asin(parameters.as_struct.downx)));
  dist = fmax(dist, (180/M_PI)*fabs(asin(pointx)-asin(parameters.as_struct.pointx)));

  int n = ceil(dist/10);

  ik_leftarmparameters q;
  memcpy(&q,&parameters,sizeof(ik_leftarmparameters));

  for (int i=1; i<=n; ++i) {
    double s = double(i)/double(n);
    double t = double(n-i)/double(n);

    ik_leftarmparameters p;
    memcpy(&p,&parameters,sizeof(ik_leftarmparameters));
    p.as_struct.px=t*parameters.as_struct.px+s*x;
    p.as_struct.py=t*parameters.as_struct.py+s*y;
    p.as_struct.pz=t*parameters.as_struct.pz+s*z;
    p.as_struct.pointx=t*parameters.as_struct.pointx+s*pointx;
    p.as_struct.downx=t*parameters.as_struct.downx+s*downx;
    ik_leftarmupdate(globals.as_array,p.as_array);
    if (p.as_struct.residual <= globals.as_struct.epsilon) {
      memcpy(&q,&p,sizeof(ik_leftarmparameters));
    } else {
      if (verbose) {
	cout << "left arm ik did not converge." << endl;
      }
      break;
    }
  }

  memcpy(&parameters,&q,sizeof(ik_leftarmparameters));
  shoulderio=q.as_struct.shoulderio;
  shoulderud=q.as_struct.shoulderud;
  bicep=q.as_struct.bicep;
  elbow=q.as_struct.elbow;
  forearm=q.as_struct.forearm;
}
