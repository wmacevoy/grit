#pragma once

#include <string>
#include <stdint.h>
#include <string.h>



typedef struct {
       float epsilon;
       float steps;
} ik_rightarmglobals_t;

typedef union 
{
  ik_rightarmglobals_t as_struct;
  float as_array[2];
} ik_rightarmglobals;

typedef struct {
       float shoulderio;
       float shoulderud;
       float bicep;
       float elbow;
       float forearm;
       float downx;
       float px;
       float py;
       float pz;
       float waist;
       float residual;
} ik_rightarmparameters_t;

typedef union 
{
  ik_rightarmparameters_t as_struct;
  float as_array[11];
} ik_rightarmparameters;

typedef struct {
       float _shoulderio;
       float _shoulderud;
       float _bicep;
       float _elbow;
       float _forearm;
} ik_rightarmvariables_t;

typedef union 
{
  ik_rightarmvariables_t as_struct;
  float as_array[5];
} ik_rightarmvariables;

typedef struct {
       float residual0;
       float residual1;
       float residual2;
       float residual3;
} ik_rightarmresiduals_t;

typedef union 
{
  ik_rightarmresiduals_t as_struct;
  float as_array[4];
} ik_rightarmresiduals;

enum { ik_rightarmglobal_count=2 };
extern const char *ik_rightarmglobal_names[2];

enum { ik_rightarmparameter_count=11 };
extern const char *ik_rightarmparameter_names[11];

enum { ik_rightarmvariable_count=5 };
extern const char *ik_rightarmvariable_names[5];

enum { ik_rightarmequation_count=4 };
extern const char *ik_rightarmequation_names[4];
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
  float ik_rightarmy[4]
);
void ik_rightarmdf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  const float ik_rightarmx[5],
  float ik_rightarmdy[20]
);
void ik_rightarmtests(const std::string &ik_rightarmname);
void ik_rightarmruns(const std::string &ik_rightarmname);

