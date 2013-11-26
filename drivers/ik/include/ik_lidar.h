#pragma once

#include <string>
#include <stdint.h>
#include <string.h>



typedef struct {
       float epsilon;
       float steps;
} ik_lidarglobals_t;

typedef union 
{
  ik_lidarglobals_t as_struct;
  float as_array[2];
} ik_lidarglobals;

typedef struct {
       float necklr;
       float neckud;
       float px;
       float py;
       float pz;
       float waist;
       float residual;
} ik_lidarparameters_t;

typedef union 
{
  ik_lidarparameters_t as_struct;
  float as_array[7];
} ik_lidarparameters;

typedef struct {
       float _necklr;
       float _neckud;
} ik_lidarvariables_t;

typedef union 
{
  ik_lidarvariables_t as_struct;
  float as_array[2];
} ik_lidarvariables;

typedef struct {
       float residual0;
       float residual1;
} ik_lidarresiduals_t;

typedef union 
{
  ik_lidarresiduals_t as_struct;
  float as_array[2];
} ik_lidarresiduals;

enum { ik_lidarglobal_count=2 };
extern const char *ik_lidarglobal_names[2];

enum { ik_lidarparameter_count=7 };
extern const char *ik_lidarparameter_names[7];

enum { ik_lidarvariable_count=2 };
extern const char *ik_lidarvariable_names[2];

enum { ik_lidarequation_count=2 };
extern const char *ik_lidarequation_names[2];
 float ik_lidardist2(int dim, const float *x, const float *y);
 float ik_lidardist(int dim, const float *x, const float *y);
 float ik_lidarnorm2(int dim, const float *x);
 float ik_lidarnorm(int dim, const float *x);
 void ik_lidarlinear_solve(int dim, float *A, float *b, float *x);
void ik_lidarsolve(
  const float globals[2],
  float parameters[7],
  float x[2]
);
void ik_lidarinitialize(
  const float ik_lidarglobals[2],
  const float ik_lidarparameters[7],
  float ik_lidarx[2]
);
void ik_lidarupdate(
  const float ik_lidarglobals[2],
  float ik_lidarparameters[7]
);
void ik_lidarf(
  const float ik_lidarglobals[2],
  const float ik_lidarparameters[7],
  const float ik_lidarx[2],
  float ik_lidary[2]
);
void ik_lidardf(
  const float ik_lidarglobals[2],
  const float ik_lidarparameters[7],
  const float ik_lidarx[2],
  float ik_lidardy[4]
);
void ik_lidartests(const std::string &ik_lidarname);
void ik_lidarruns(const std::string &ik_lidarname);

