#pragma once

#include <string>
#include <stdint.h>
#include <string.h>



typedef struct {
       float epsilon;
       float steps;
} ik_leftarmglobals_t;

typedef union 
{
  ik_leftarmglobals_t as_struct;
  float as_array[2];
} ik_leftarmglobals;

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
} ik_leftarmparameters_t;

typedef union 
{
  ik_leftarmparameters_t as_struct;
  float as_array[11];
} ik_leftarmparameters;

typedef struct {
       float _shoulderio;
       float _shoulderud;
       float _bicep;
       float _elbow;
       float _forearm;
} ik_leftarmvariables_t;

typedef union 
{
  ik_leftarmvariables_t as_struct;
  float as_array[5];
} ik_leftarmvariables;

typedef struct {
       float residual0;
       float residual1;
       float residual2;
       float residual3;
} ik_leftarmresiduals_t;

typedef union 
{
  ik_leftarmresiduals_t as_struct;
  float as_array[4];
} ik_leftarmresiduals;

enum { ik_leftarmglobal_count=2 };
extern const char *ik_leftarmglobal_names[2];

enum { ik_leftarmparameter_count=11 };
extern const char *ik_leftarmparameter_names[11];

enum { ik_leftarmvariable_count=5 };
extern const char *ik_leftarmvariable_names[5];

enum { ik_leftarmequation_count=4 };
extern const char *ik_leftarmequation_names[4];
 float ik_leftarmdist2(int dim, const float *x, const float *y);
 float ik_leftarmdist(int dim, const float *x, const float *y);
 float ik_leftarmnorm2(int dim, const float *x);
 float ik_leftarmnorm(int dim, const float *x);
 void ik_leftarmlinear_solve(int dim, float *A, float *b, float *x);
void ik_leftarmsolve(
  const float globals[2],
  float parameters[11],
  float x[5]
);
void ik_leftarminitialize(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[11],
  float ik_leftarmx[5]
);
void ik_leftarmupdate(
  const float ik_leftarmglobals[2],
  float ik_leftarmparameters[11]
);
void ik_leftarmf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[11],
  const float ik_leftarmx[5],
  float ik_leftarmy[4]
);
void ik_leftarmdf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[11],
  const float ik_leftarmx[5],
  float ik_leftarmdy[20]
);
void ik_leftarmtests(const std::string &ik_leftarmname);
void ik_leftarmruns(const std::string &ik_leftarmname);

