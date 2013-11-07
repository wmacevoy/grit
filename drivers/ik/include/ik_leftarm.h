#ifndef _IK_LEFTARMSOLVE_HPP
#define _IK_LEFTARMSOLVE_HPP


#include <string>
#include <stdint.h>
#include <string.h>


enum { ik_leftarmglobal_count=2 };
extern const char *ik_leftarmglobal_names[2];

enum { ik_leftarmparameter_count=16 };
extern const char *ik_leftarmparameter_names[16];

enum { ik_leftarmvariable_count=5 };
extern const char *ik_leftarmvariable_names[5];

enum { ik_leftarmequation_count=5 };
extern const char *ik_leftarmequation_names[5];
 float ik_leftarmdist2(int dim, const float *x, const float *y);
 float ik_leftarmdist(int dim, const float *x, const float *y);
 float ik_leftarmnorm2(int dim, const float *x);
 float ik_leftarmnorm(int dim, const float *x);
 void ik_leftarmlinear_solve(int dim, float *A, float *b, float *x);
void ik_leftarmsolve(
  const float globals[2],
  float parameters[16],
  float x[5]
);
void ik_leftarminitialize(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[16],
  float ik_leftarmx[5]
);
void ik_leftarmupdate(
  const float ik_leftarmglobals[2],
  float ik_leftarmparameters[16]
);
void ik_leftarmf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[16],
  const float ik_leftarmx[5],
  float ik_leftarmy[5]
);
void ik_leftarmdf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[16],
  const float ik_leftarmx[5],
  float ik_leftarmdy[25]
);
void ik_leftarmtests(const std::string &ik_leftarmname);
void ik_leftarmruns(const std::string &ik_leftarmname);

#endif
