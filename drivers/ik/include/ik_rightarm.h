#ifndef _IK_RIGHTARMSOLVE_HPP
#define _IK_RIGHTARMSOLVE_HPP


#include <string>
#include <stdint.h>
#include <string.h>


enum { ik_rightarmglobal_count=2 };
extern const char *ik_rightarmglobal_names[2];

enum { ik_rightarmparameter_count=11 };
extern const char *ik_rightarmparameter_names[11];

enum { ik_rightarmvariable_count=5 };
extern const char *ik_rightarmvariable_names[5];

enum { ik_rightarmequation_count=5 };
extern const char *ik_rightarmequation_names[5];
 float ik_rightarmdist2(int dim, const float *x, const float *y);
 float ik_rightarmdist(int dim, const float *x, const float *y);
 float ik_rightarmnorm2(int dim, const float *x);
 float ik_rightarmnorm(int dim, const float *x);
 void ik_rightarmlinear_solve(int dim, float *A, float *b, float *x);
void ik_rightarmsolve(
  const float globals[2],
  float parameters[11],
  float x[5]
);
void ik_rightarminitialize(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  float ik_rightarmx[5]
);
void ik_rightarmupdate(
  const float ik_rightarmglobals[2],
  float ik_rightarmparameters[11]
);
void ik_rightarmf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  const float ik_rightarmx[5],
  float ik_rightarmy[5]
);
void ik_rightarmdf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  const float ik_rightarmx[5],
  float ik_rightarmdy[25]
);
void ik_rightarmtests(const std::string &ik_rightarmname);
void ik_rightarmruns(const std::string &ik_rightarmname);

#endif
