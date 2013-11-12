#pragma once

#include <string>
#include <stdint.h>
#include <string.h>



typedef struct {
       float epsilon;
       float steps;
} ik_legglobals_t;

typedef union 
{
  ik_legglobals_t as_struct;
  float as_array[2];
} ik_legglobals;

typedef struct {
       float knee;
       float femur;
       float hip;
       float chasis;
       float dfemur2knee;
       float dhip2femur;
       float drchasis2hip;
       float dyknee2tip;
       float dzchasis2hip;
       float dzknee2tip;
       float px;
       float py;
       float pz;
       float residual;
} ik_legparameters_t;

typedef union 
{
  ik_legparameters_t as_struct;
  float as_array[14];
} ik_legparameters;

typedef struct {
       float _knee;
       float _femur;
       float _hip;
} ik_legvariables_t;

typedef union 
{
  ik_legvariables_t as_struct;
  float as_array[3];
} ik_legvariables;

typedef struct {
       float residual0;
       float residual1;
       float residual2;
} ik_legresiduals_t;

typedef union 
{
  ik_legresiduals_t as_struct;
  float as_array[3];
} ik_legresiduals;

enum { ik_legglobal_count=2 };
extern const char *ik_legglobal_names[2];

enum { ik_legparameter_count=14 };
extern const char *ik_legparameter_names[14];

enum { ik_legvariable_count=3 };
extern const char *ik_legvariable_names[3];

enum { ik_legequation_count=3 };
extern const char *ik_legequation_names[3];
 float ik_legdist2(int dim, const float *x, const float *y);
 float ik_legdist(int dim, const float *x, const float *y);
 float ik_legnorm2(int dim, const float *x);
 float ik_legnorm(int dim, const float *x);
 void ik_leglinear_solve(int dim, float *A, float *b, float *x);
void ik_legsolve(
  const float globals[2],
  float parameters[14],
  float x[3]
);
void ik_leginitialize(
  const float ik_legglobals[2],
  const float ik_legparameters[14],
  float ik_legx[3]
);
void ik_legupdate(
  const float ik_legglobals[2],
  float ik_legparameters[14]
);
void ik_legf(
  const float ik_legglobals[2],
  const float ik_legparameters[14],
  const float ik_legx[3],
  float ik_legy[3]
);
void ik_legdf(
  const float ik_legglobals[2],
  const float ik_legparameters[14],
  const float ik_legx[3],
  float ik_legdy[9]
);
void ik_legtests(const std::string &ik_legname);
void ik_legruns(const std::string &ik_legname);

