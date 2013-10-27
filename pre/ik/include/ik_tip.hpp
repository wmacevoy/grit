#ifndef _IK_TIPSOLVE_HPP
#define _IK_TIPSOLVE_HPP


#include <string>
#include <stdint.h>
#include <string.h>


enum { ik_tipglobal_count=2 };
extern const char *ik_tipglobal_names[2];

enum { ik_tipparameter_count=14 };
extern const char *ik_tipparameter_names[14];

enum { ik_tipvariable_count=3 };
extern const char *ik_tipvariable_names[3];

enum { ik_tipequation_count=3 };
extern const char *ik_tipequation_names[3];
 float ik_tipdist2(int dim, const float *x, const float *y);
 float ik_tipdist(int dim, const float *x, const float *y);
 float ik_tipnorm2(int dim, const float *x);
 float ik_tipnorm(int dim, const float *x);
 void ik_tiplinear_solve(int dim, float *A, float *b, float *x);
void ik_tipsolve(
  const float globals[2],
  float parameters[14],
  float x[3]
);
void ik_tipinitialize(
  const float ik_tipglobals[2],
  const float ik_tipparameters[14],
  float ik_tipx[3]
);
void ik_tipupdate(
  const float ik_tipglobals[2],
  float ik_tipparameters[14]
);
void ik_tipf(
  const float ik_tipglobals[2],
  const float ik_tipparameters[14],
  const float ik_tipx[3],
  float ik_tipy[3]
);
void ik_tipdf(
  const float ik_tipglobals[2],
  const float ik_tipparameters[14],
  const float ik_tipx[3],
  float ik_tipdy[9]
);
void ik_tiptests(const std::string &ik_tipname);
void ik_tipruns(const std::string &ik_tipname);

#endif
