#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <cassert>
enum { ik_leftarmglobal_count=2 };
extern const char *ik_leftarmglobal_names[2];

enum { ik_leftarmparameter_count=12 };
extern const char *ik_leftarmparameter_names[12];

enum { ik_leftarmvariable_count=5 };
extern const char *ik_leftarmvariable_names[5];

enum { ik_leftarmequation_count=5 };
extern const char *ik_leftarmequation_names[5];
const char *ik_leftarmglobal_names[]={
  "epsilon",
  "steps"
};

const char *ik_leftarmparameter_names[]={
  "shoulderio",
  "shoulderud",
  "bicep",
  "elbow",
  "forearm",
  "downx",
  "pointx",
  "px",
  "py",
  "pz",
  "waist",
  "residual"
};

const char *ik_leftarmvariable_names[]={
  "_shoulderio",
  "_shoulderud",
  "_bicep",
  "_elbow",
  "_forearm"
};

const char *ik_leftarmequation_names[]={
  "residual0",
  "residual1",
  "residual2",
  "residual3",
  "residual4"
};

class ik_leftarmStopwatch
{
public:
  struct timeval startTime, endTime, diffTime;
  void start() {
    gettimeofday(&startTime,NULL);
  }
  void stop() {
    gettimeofday(&endTime,NULL);
    timersub(&endTime,&startTime,&diffTime);
  }
  double time()
  {
    return diffTime.tv_sec + 1e-6*diffTime.tv_usec;
  }
};
 float ik_leftarmdist2(int dim, const float *x, const float *y)
{
  float sum2=0;
  for (int k=0; k<dim; ++k) {
    float dx=x[k]-y[k];
    sum2 += dx*dx;
  }
  return sum2;
} // dist2()
 float ik_leftarmdist(int dim, const float *x, const float *y)
{
  return sqrt(ik_leftarmdist2(dim,x,y));
} // dist()
 float ik_leftarmnorm2(int dim, const float *x)
{
  float sum2=0;
  for (int k=0; k<dim; ++k) {
    float dx=x[k];
    sum2 += dx*dx;
  }
  return sum2;
} // dist2()
 float ik_leftarmnorm(int dim, const float *x)
{
  return sqrt(ik_leftarmnorm2(dim,x));
} // dist()
 int ik_leftarmDoolittle_LU_Decomposition_with_Pivoting(float *A, int pivot[], int n)
{
   int i, j, k;
   float *p_k, *p_row, *p_col;
   float max;


//         For each row and column, k = 0, ..., n-1,
 
   for (k = 0, p_k = A; k < n; p_k += n, k++) {

//            find the pivot row

      pivot[k] = k;
      max = fabs( *(p_k + k) );
      for (j = k + 1, p_row = p_k + n; j < n; j++, p_row += n) {
         if ( max < fabs(*(p_row + k)) ) {
            max = fabs(*(p_row + k));
            pivot[k] = j;
            p_col = p_row;
         }
      }

//     and if the pivot row differs from the current row, then
//     interchange the two rows.
   
      if (pivot[k] != k)
         for (j = 0; j < n; j++) {
            max = *(p_k + j);
            *(p_k + j) = *(p_col + j);
            *(p_col + j) = max;
         }

//                and if the matrix is singular, return error


      if ( *(p_k + k) == 0.0 ) return -1;

//      otherwise find the lower triangular matrix elements for column k. 

      for (i = k+1, p_row = p_k + n; i < n; p_row += n, i++) {
         *(p_row + k) /= *(p_k + k);
      }  

//            update remaining matrix

      for (i = k+1, p_row = p_k + n; i < n; p_row += n, i++)
         for (j = k+1; j < n; j++)
            *(p_row + j) -= *(p_row + k) * *(p_k + j);

   }

   return 0;
}


 int ik_leftarmDoolittle_LU_with_Pivoting_Solve(float *A, float B[], int pivot[],
                                                              float x[], int n)
{
   int i, k;
   float *p_k;
   float dum;

//         Solve the linear equation Lx = B for x, where L is a lower
//         triangular matrix with an implied 1 along the diagonal.
   
   for (k = 0, p_k = A; k < n; p_k += n, k++) {
      if (pivot[k] != k) {dum = B[k]; B[k] = B[pivot[k]]; B[pivot[k]] = dum; }
      x[k] = B[k];
      for (i = 0; i < k; i++) x[k] -= x[i] * *(p_k + i);
   }

//         Solve the linear equation Ux = y, where y is the solution
//         obtained above of Lx = B and U is an upper triangular matrix.

   for (k = n-1, p_k = A + n*(n-1); k >= 0; k--, p_k -= n) {
      if (pivot[k] != k) {dum = B[k]; B[k] = B[pivot[k]]; B[pivot[k]] = dum; }
      for (i = k + 1; i < n; i++) x[k] -= x[i] * *(p_k + i);
      if (*(p_k + k) == 0.0) return -1;
      x[k] /= *(p_k + k);
   }
  
   return 0;
}

 void ik_leftarmlinear_solve(int dim, int *ipiv, float *A, float *b, float *x)
{
    float tmp;
    for (int ii=0;ii<dim; ++ii) {
      for (int jj=ii+1;jj<dim; ++jj) {
        float *x=&A[ii*dim+jj];
        float *y=&A[jj*dim+ii];
	tmp=*x;
	*x=*y;
	*y=tmp;
      }
    }
  
    ik_leftarmDoolittle_LU_Decomposition_with_Pivoting(A,ipiv,dim);
    ik_leftarmDoolittle_LU_with_Pivoting_Solve(A,b,ipiv,x,dim);
}
void ik_leftarmf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[12],
  const float ik_leftarmx[5],
  float ik_leftarmy[5]
);
void ik_leftarmdf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[12],
  const float ik_leftarmx[5],
  float ik_leftarmdy[25]
);
void ik_leftarminitialize(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[12],
  float ik_leftarmx[5]
);
void ik_leftarmupdate(
  const float ik_leftarmglobals[2],
  float ik_leftarmparameters[12]
);
// damped newton solver

   void ik_leftarmsolve(
    
    const float globals[2],
    float *parameters,
    float *x
  )
  {
    ik_leftarminitialize(globals,parameters,x);
    // newton iteration
    int steps=int(globals[1]);
    float epsilon=globals[0];
    float residual,new_residual;
  
    for (int step=0; step<steps; ++step) {
      float y[5];
      float dx[5];
      float dy[25];
      int iwork[5];
  
      ik_leftarmf(globals,parameters,x,y);
      residual = ik_leftarmnorm(5,y);
      ik_leftarmdf(globals,parameters,x,dy);

      ik_leftarmlinear_solve(5,iwork,dy,y,dx);
      for (int k=0; k<5; ++k) { x[k] -= dx[k]; }

      for (int damping=0; damping < steps; ++damping) {
	    ik_leftarmf(globals,parameters,x,y);
	    new_residual=ik_leftarmnorm(5,y);
	    if (new_residual < residual) break;
	    for (int k=0; k<5; ++k) { dx[k] /= 2; }
	    for (int k=0; k<5; ++k) { x[k] += dx[k] ; }
      }
      residual=new_residual;
      if (residual <= epsilon) break;
    }
    parameters[11]=residual;
  } //  ik_leftarmsolve()
typedef struct {
  const char *ik_leftarmname;
  float ik_leftarmglobals[2];
  float ik_leftarmparameters[12];
  float ik_leftarmx[5];
} ik_leftarmtest_data_t;

ik_leftarmtest_data_t ik_leftarmtest_data[] = {
}; // ik_leftarmtest_data
void ik_leftarmtests(const std::string &ik_leftarmname)
{
  std::string ik_leftarmcase_name;
  std::string ik_leftarmmax_x_error_name="none";
  int ik_leftarmmax_x_error_count=0;
  float ik_leftarmmax_x_error=0;
  float ik_leftarmx_error=0;
  std::string ik_leftarmmax_y_error_name="none";
  int ik_leftarmmax_y_error_count=0;
  float ik_leftarmmax_y_error=0;
  float ik_leftarmy_error=0;

  float ik_leftarmx_test[5];
  float ik_leftarmy_test[5];

  for (int ik_leftarmcase_count=0; ik_leftarmcase_count < 0; ++ik_leftarmcase_count) {
    if (ik_leftarmname == "all" || ik_leftarmname == ik_leftarmtest_data[ik_leftarmcase_count].ik_leftarmname) {
       const char *ik_leftarmname=ik_leftarmtest_data[ik_leftarmcase_count].ik_leftarmname;
       float *ik_leftarmglobals=ik_leftarmtest_data[ik_leftarmcase_count].ik_leftarmglobals;
       float *ik_leftarmparameters=ik_leftarmtest_data[ik_leftarmcase_count].ik_leftarmparameters;
       float *ik_leftarmx=ik_leftarmtest_data[ik_leftarmcase_count].ik_leftarmx;

       ik_leftarmsolve(ik_leftarmglobals,ik_leftarmparameters,ik_leftarmx_test);
       ik_leftarmf(ik_leftarmglobals,ik_leftarmparameters,ik_leftarmx_test,ik_leftarmy_test);
       ik_leftarmx_error=ik_leftarmdist(5,ik_leftarmx,ik_leftarmx_test);
       ik_leftarmy_error=ik_leftarmnorm(5,ik_leftarmy_test);

       if (ik_leftarmx_error > ik_leftarmmax_x_error) {
         ik_leftarmmax_x_error=ik_leftarmx_error;
         ik_leftarmmax_x_error_name=ik_leftarmname;
         ik_leftarmmax_x_error_count=ik_leftarmcase_count;
       }

       if (ik_leftarmy_error > ik_leftarmmax_y_error) {
         ik_leftarmmax_y_error=ik_leftarmy_error;
         ik_leftarmmax_y_error_name=ik_leftarmname;
         ik_leftarmmax_y_error_count=ik_leftarmcase_count;
       }

       std::cout << "test " << (ik_leftarmcase_count+1) << "/" << "\"" << ik_leftarmname << "\"" << ": " << "x_error=" << std::setprecision(15) << ik_leftarmx_error << "," << "y_error=" << std::setprecision(15) << ik_leftarmy_error << std::endl;
     }
   }
   if (ik_leftarmname == "all") {
     std::cout << "test summary: " 
       << "max_x_error=" << std::setprecision(15) << ik_leftarmmax_x_error
          << " from " << ik_leftarmmax_x_error_count << "/" 
          << "\"" << ik_leftarmmax_x_error_name << "\""
       << ","
       << "max_y_error=" << std::setprecision(15) << ik_leftarmmax_y_error
          << " from " << ik_leftarmmax_y_error_count << "/" 
          << "\"" << ik_leftarmmax_y_error_name << "\""
       << std::endl;
   }
}
typedef struct {
  const char *ik_leftarmname;
  float ik_leftarmglobals[2];
  float ik_leftarmparameters[12];
} ik_leftarmrun_data_t;

ik_leftarmrun_data_t ik_leftarmrun_data[] = {
}; // ik_leftarmrun_data
void ik_leftarmruns(const std::string &ik_leftarmname)
{
  std::string ik_leftarmcase_name;
  for (int ik_leftarmcase_count=0; ik_leftarmcase_count < 0; ++ik_leftarmcase_count) {
    if (ik_leftarmname == "all" || ik_leftarmname == ik_leftarmrun_data[ik_leftarmcase_count].ik_leftarmname) {
      const char *ik_leftarmcase_name=ik_leftarmrun_data[ik_leftarmcase_count].ik_leftarmname;
      float *ik_leftarmglobals=ik_leftarmrun_data[ik_leftarmcase_count].ik_leftarmglobals;
      float *ik_leftarmparameters=ik_leftarmrun_data[ik_leftarmcase_count].ik_leftarmparameters;
      float ik_leftarmx[5];

      ik_leftarmsolve(ik_leftarmglobals,ik_leftarmparameters,ik_leftarmx);
      std::cout << "[case] # " << (ik_leftarmcase_count+1) << std::endl;
      std::cout << "name=\"" << ik_leftarmcase_name << "\"" << std::endl;

      for (int i=0; i<ik_leftarmglobal_count; ++i) {
        std::cout << ik_leftarmglobal_names[i] << "=" << std::setprecision(15) << ik_leftarmglobals[i] << std::endl;
      }

      for (int i=0; i<ik_leftarmparameter_count; ++i) {
        std::cout << ik_leftarmparameter_names[i] << "=" << std::setprecision(15) << ik_leftarmparameters[i] << std::endl;
      }

      for (int i=0; i<ik_leftarmvariable_count; ++i) {
        std::cout << ik_leftarmvariable_names[i] << "=" << std::setprecision(15) << ik_leftarmx[i] << std::endl;
      }
    }
  }
} // ik_leftarmruns()
// global aliases
#define epsilon ik_leftarmglobals[0]
#define steps ik_leftarmglobals[1]

// parameter aliases
#define shoulderio ik_leftarmparameters[0]
#define shoulderud ik_leftarmparameters[1]
#define bicep ik_leftarmparameters[2]
#define elbow ik_leftarmparameters[3]
#define forearm ik_leftarmparameters[4]
#define downx ik_leftarmparameters[5]
#define pointx ik_leftarmparameters[6]
#define px ik_leftarmparameters[7]
#define py ik_leftarmparameters[8]
#define pz ik_leftarmparameters[9]
#define waist ik_leftarmparameters[10]
#define residual ik_leftarmparameters[11]

// variable aliases
#define _shoulderio ik_leftarmx[0]
#define _shoulderud ik_leftarmx[1]
#define _bicep ik_leftarmx[2]
#define _elbow ik_leftarmx[3]
#define _forearm ik_leftarmx[4]

// residual aliases
#define residual0 ik_leftarmy[0]
#define residual1 ik_leftarmy[1]
#define residual2 ik_leftarmy[2]
#define residual3 ik_leftarmy[3]
#define residual4 ik_leftarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[5]
#define dresidual0_d_bicep ik_leftarmdy[10]
#define dresidual0_d_elbow ik_leftarmdy[15]
#define dresidual0_d_forearm ik_leftarmdy[20]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[6]
#define dresidual1_d_bicep ik_leftarmdy[11]
#define dresidual1_d_elbow ik_leftarmdy[16]
#define dresidual1_d_forearm ik_leftarmdy[21]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[7]
#define dresidual2_d_bicep ik_leftarmdy[12]
#define dresidual2_d_elbow ik_leftarmdy[17]
#define dresidual2_d_forearm ik_leftarmdy[22]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[8]
#define dresidual3_d_bicep ik_leftarmdy[13]
#define dresidual3_d_elbow ik_leftarmdy[18]
#define dresidual3_d_forearm ik_leftarmdy[23]
#define dresidual4_d_shoulderio ik_leftarmdy[4]
#define dresidual4_d_shoulderud ik_leftarmdy[9]
#define dresidual4_d_bicep ik_leftarmdy[14]
#define dresidual4_d_elbow ik_leftarmdy[19]
#define dresidual4_d_forearm ik_leftarmdy[24]

void ik_leftarminitialize(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[12],
  float ik_leftarmx[5]
)
{
// global aliases
#define epsilon ik_leftarmglobals[0]
#define steps ik_leftarmglobals[1]

// parameter aliases
#define shoulderio ik_leftarmparameters[0]
#define shoulderud ik_leftarmparameters[1]
#define bicep ik_leftarmparameters[2]
#define elbow ik_leftarmparameters[3]
#define forearm ik_leftarmparameters[4]
#define downx ik_leftarmparameters[5]
#define pointx ik_leftarmparameters[6]
#define px ik_leftarmparameters[7]
#define py ik_leftarmparameters[8]
#define pz ik_leftarmparameters[9]
#define waist ik_leftarmparameters[10]
#define residual ik_leftarmparameters[11]

// variable aliases
#define _shoulderio ik_leftarmx[0]
#define _shoulderud ik_leftarmx[1]
#define _bicep ik_leftarmx[2]
#define _elbow ik_leftarmx[3]
#define _forearm ik_leftarmx[4]

// residual aliases
#define residual0 ik_leftarmy[0]
#define residual1 ik_leftarmy[1]
#define residual2 ik_leftarmy[2]
#define residual3 ik_leftarmy[3]
#define residual4 ik_leftarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[5]
#define dresidual0_d_bicep ik_leftarmdy[10]
#define dresidual0_d_elbow ik_leftarmdy[15]
#define dresidual0_d_forearm ik_leftarmdy[20]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[6]
#define dresidual1_d_bicep ik_leftarmdy[11]
#define dresidual1_d_elbow ik_leftarmdy[16]
#define dresidual1_d_forearm ik_leftarmdy[21]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[7]
#define dresidual2_d_bicep ik_leftarmdy[12]
#define dresidual2_d_elbow ik_leftarmdy[17]
#define dresidual2_d_forearm ik_leftarmdy[22]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[8]
#define dresidual3_d_bicep ik_leftarmdy[13]
#define dresidual3_d_elbow ik_leftarmdy[18]
#define dresidual3_d_forearm ik_leftarmdy[23]
#define dresidual4_d_shoulderio ik_leftarmdy[4]
#define dresidual4_d_shoulderud ik_leftarmdy[9]
#define dresidual4_d_bicep ik_leftarmdy[14]
#define dresidual4_d_elbow ik_leftarmdy[19]
#define dresidual4_d_forearm ik_leftarmdy[24]

  // initialize unknowns from parameters
  // _shoulderio=shoulderio;
  _shoulderio=shoulderio;
  // _shoulderud=shoulderud;
  _shoulderud=shoulderud;
  // _bicep=bicep;
  _bicep=bicep;
  // _elbow=elbow;
  _elbow=elbow;
  // _forearm=forearm;
  _forearm=forearm;
// undefine global aliases
#undef epsilon
#undef steps

// undefine parameter aliases
#undef shoulderio
#undef shoulderud
#undef bicep
#undef elbow
#undef forearm
#undef downx
#undef pointx
#undef px
#undef py
#undef pz
#undef waist
#undef residual

// undefine variable aliases
#undef _shoulderio
#undef _shoulderud
#undef _bicep
#undef _elbow
#undef _forearm

// undefine residual aliases
#undef residual0
#undef residual1
#undef residual2
#undef residual3
#undef residual4

// undefine jacobian aliases (fortran order)
#undef dresidual0_d_shoulderio
#undef dresidual0_d_shoulderud
#undef dresidual0_d_bicep
#undef dresidual0_d_elbow
#undef dresidual0_d_forearm
#undef dresidual1_d_shoulderio
#undef dresidual1_d_shoulderud
#undef dresidual1_d_bicep
#undef dresidual1_d_elbow
#undef dresidual1_d_forearm
#undef dresidual2_d_shoulderio
#undef dresidual2_d_shoulderud
#undef dresidual2_d_bicep
#undef dresidual2_d_elbow
#undef dresidual2_d_forearm
#undef dresidual3_d_shoulderio
#undef dresidual3_d_shoulderud
#undef dresidual3_d_bicep
#undef dresidual3_d_elbow
#undef dresidual3_d_forearm
#undef dresidual4_d_shoulderio
#undef dresidual4_d_shoulderud
#undef dresidual4_d_bicep
#undef dresidual4_d_elbow
#undef dresidual4_d_forearm
} // ik_leftarminitialize()
void ik_leftarmupdate(
  const float ik_leftarmglobals[2],
  float ik_leftarmparameters[12]
)
{
  float ik_leftarmx[5];
  ik_leftarmsolve(ik_leftarmglobals,ik_leftarmparameters,ik_leftarmx);
// global aliases
#define epsilon ik_leftarmglobals[0]
#define steps ik_leftarmglobals[1]

// parameter aliases
#define shoulderio ik_leftarmparameters[0]
#define shoulderud ik_leftarmparameters[1]
#define bicep ik_leftarmparameters[2]
#define elbow ik_leftarmparameters[3]
#define forearm ik_leftarmparameters[4]
#define downx ik_leftarmparameters[5]
#define pointx ik_leftarmparameters[6]
#define px ik_leftarmparameters[7]
#define py ik_leftarmparameters[8]
#define pz ik_leftarmparameters[9]
#define waist ik_leftarmparameters[10]
#define residual ik_leftarmparameters[11]

// variable aliases
#define _shoulderio ik_leftarmx[0]
#define _shoulderud ik_leftarmx[1]
#define _bicep ik_leftarmx[2]
#define _elbow ik_leftarmx[3]
#define _forearm ik_leftarmx[4]

// residual aliases
#define residual0 ik_leftarmy[0]
#define residual1 ik_leftarmy[1]
#define residual2 ik_leftarmy[2]
#define residual3 ik_leftarmy[3]
#define residual4 ik_leftarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[5]
#define dresidual0_d_bicep ik_leftarmdy[10]
#define dresidual0_d_elbow ik_leftarmdy[15]
#define dresidual0_d_forearm ik_leftarmdy[20]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[6]
#define dresidual1_d_bicep ik_leftarmdy[11]
#define dresidual1_d_elbow ik_leftarmdy[16]
#define dresidual1_d_forearm ik_leftarmdy[21]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[7]
#define dresidual2_d_bicep ik_leftarmdy[12]
#define dresidual2_d_elbow ik_leftarmdy[17]
#define dresidual2_d_forearm ik_leftarmdy[22]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[8]
#define dresidual3_d_bicep ik_leftarmdy[13]
#define dresidual3_d_elbow ik_leftarmdy[18]
#define dresidual3_d_forearm ik_leftarmdy[23]
#define dresidual4_d_shoulderio ik_leftarmdy[4]
#define dresidual4_d_shoulderud ik_leftarmdy[9]
#define dresidual4_d_bicep ik_leftarmdy[14]
#define dresidual4_d_elbow ik_leftarmdy[19]
#define dresidual4_d_forearm ik_leftarmdy[24]

  // shoulderio=_shoulderio
  shoulderio=_shoulderio;
  // shoulderud=_shoulderud
  shoulderud=_shoulderud;
  // bicep=_bicep
  bicep=_bicep;
  // elbow=_elbow
  elbow=_elbow;
  // forearm=_forearm
  forearm=_forearm;
// undefine global aliases
#undef epsilon
#undef steps

// undefine parameter aliases
#undef shoulderio
#undef shoulderud
#undef bicep
#undef elbow
#undef forearm
#undef downx
#undef pointx
#undef px
#undef py
#undef pz
#undef waist
#undef residual

// undefine variable aliases
#undef _shoulderio
#undef _shoulderud
#undef _bicep
#undef _elbow
#undef _forearm

// undefine residual aliases
#undef residual0
#undef residual1
#undef residual2
#undef residual3
#undef residual4

// undefine jacobian aliases (fortran order)
#undef dresidual0_d_shoulderio
#undef dresidual0_d_shoulderud
#undef dresidual0_d_bicep
#undef dresidual0_d_elbow
#undef dresidual0_d_forearm
#undef dresidual1_d_shoulderio
#undef dresidual1_d_shoulderud
#undef dresidual1_d_bicep
#undef dresidual1_d_elbow
#undef dresidual1_d_forearm
#undef dresidual2_d_shoulderio
#undef dresidual2_d_shoulderud
#undef dresidual2_d_bicep
#undef dresidual2_d_elbow
#undef dresidual2_d_forearm
#undef dresidual3_d_shoulderio
#undef dresidual3_d_shoulderud
#undef dresidual3_d_bicep
#undef dresidual3_d_elbow
#undef dresidual3_d_forearm
#undef dresidual4_d_shoulderio
#undef dresidual4_d_shoulderud
#undef dresidual4_d_bicep
#undef dresidual4_d_elbow
#undef dresidual4_d_forearm
} // ik_leftarmupdate()

void ik_leftarmf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[12],
  const float ik_leftarmx[5],
  float ik_leftarmy[5]
)
{
// global aliases
#define epsilon ik_leftarmglobals[0]
#define steps ik_leftarmglobals[1]

// parameter aliases
#define shoulderio ik_leftarmparameters[0]
#define shoulderud ik_leftarmparameters[1]
#define bicep ik_leftarmparameters[2]
#define elbow ik_leftarmparameters[3]
#define forearm ik_leftarmparameters[4]
#define downx ik_leftarmparameters[5]
#define pointx ik_leftarmparameters[6]
#define px ik_leftarmparameters[7]
#define py ik_leftarmparameters[8]
#define pz ik_leftarmparameters[9]
#define waist ik_leftarmparameters[10]
#define residual ik_leftarmparameters[11]

// variable aliases
#define _shoulderio ik_leftarmx[0]
#define _shoulderud ik_leftarmx[1]
#define _bicep ik_leftarmx[2]
#define _elbow ik_leftarmx[3]
#define _forearm ik_leftarmx[4]

// residual aliases
#define residual0 ik_leftarmy[0]
#define residual1 ik_leftarmy[1]
#define residual2 ik_leftarmy[2]
#define residual3 ik_leftarmy[3]
#define residual4 ik_leftarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[5]
#define dresidual0_d_bicep ik_leftarmdy[10]
#define dresidual0_d_elbow ik_leftarmdy[15]
#define dresidual0_d_forearm ik_leftarmdy[20]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[6]
#define dresidual1_d_bicep ik_leftarmdy[11]
#define dresidual1_d_elbow ik_leftarmdy[16]
#define dresidual1_d_forearm ik_leftarmdy[21]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[7]
#define dresidual2_d_bicep ik_leftarmdy[12]
#define dresidual2_d_elbow ik_leftarmdy[17]
#define dresidual2_d_forearm ik_leftarmdy[22]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[8]
#define dresidual3_d_bicep ik_leftarmdy[13]
#define dresidual3_d_elbow ik_leftarmdy[18]
#define dresidual3_d_forearm ik_leftarmdy[23]
#define dresidual4_d_shoulderio ik_leftarmdy[4]
#define dresidual4_d_shoulderud ik_leftarmdy[9]
#define dresidual4_d_bicep ik_leftarmdy[14]
#define dresidual4_d_elbow ik_leftarmdy[19]
#define dresidual4_d_forearm ik_leftarmdy[24]

float tmp0;
float tmp1;
float tmp2;
float tmp3;
float tmp4;
float tmp5;
float tmp6;
float tmp7;
float tmp8;
float tmp9;
float tmp10;
float tmp11;
float tmp12;
float tmp13;
float tmp14;
float tmp15;
float tmp16;
float tmp17;
float tmp18;
float tmp19;
float tmp20;
float tmp21;
float tmp22;
float tmp23;
float tmp24;
float tmp25;
float tmp26;
float tmp27;
float tmp28;
float tmp29;
float tmp30;
float tmp31;
float tmp32;
float tmp33;
float tmp34;
float tmp35;
float tmp36;
float tmp37;
float tmp38;
float tmp39;
float tmp40;
float tmp41;
float tmp42;
float tmp43;
float tmp44;
float tmp45;
float tmp46;
float tmp47;
float tmp48;
float tmp49;
float tmp50;
float tmp51;
float tmp52;
float tmp53;
float tmp54;
float tmp55;
float tmp56;
float tmp57;
float tmp58;
float tmp59;
float tmp60;
float tmp61;
float tmp62;
float tmp63;
float tmp64;
float tmp65;
float tmp66;
float tmp67;
float tmp68;
float tmp69;
float tmp70;
float tmp71;
float tmp72;
float tmp73;
float tmp74;
float tmp75;
float tmp76;
float tmp77;
float tmp78;
float tmp79;
float tmp80;
float tmp81;
tmp0=0.017453293f*_shoulderud;
tmp1=tmp0+(-0.78539816f);
tmp2=cos(tmp1);
tmp3=(-0.017453293f)*_shoulderio;
tmp4=tmp3+0.78539816f;
tmp5=sin(tmp4);
tmp6=(-9.0f)*tmp2*tmp5;
tmp7=tmp3+0.78539816f;
tmp8=sin(tmp7);
tmp9=(-3.625f)*tmp8;
tmp10=(-1.0f)*px;
tmp11=tmp6+tmp9+tmp10+(-6.0f);
residual0=tmp11;
tmp12=0.017453293f*waist;
tmp13=tmp12+0.087266463f;
tmp14=sin(tmp13);
tmp15=(-13.5f)*tmp14;
tmp16=tmp0+(-0.78539816f);
tmp17=sin(tmp16);
tmp18=tmp12+0.087266463f;
tmp19=sin(tmp18);
tmp20=(-9.0f)*tmp17*tmp19;
tmp21=(-1.0f)*py;
tmp22=tmp12+0.087266463f;
tmp23=cos(tmp22);
tmp24=2.375f*tmp23;
tmp25=tmp3+0.78539816f;
tmp26=cos(tmp25);
tmp27=tmp12+0.087266463f;
tmp28=cos(tmp27);
tmp29=3.625f*tmp26*tmp28;
tmp30=tmp3+0.78539816f;
tmp31=cos(tmp30);
tmp32=tmp0+(-0.78539816f);
tmp33=cos(tmp32);
tmp34=tmp12+0.087266463f;
tmp35=cos(tmp34);
tmp36=9.0f*tmp31*tmp33*tmp35;
tmp37=tmp15+tmp20+tmp21+tmp24+tmp29+tmp36;
residual1=tmp37;
tmp38=(-1.0f)*pz;
tmp39=tmp12+0.087266463f;
tmp40=sin(tmp39);
tmp41=2.375f*tmp40;
tmp42=tmp3+0.78539816f;
tmp43=cos(tmp42);
tmp44=tmp12+0.087266463f;
tmp45=sin(tmp44);
tmp46=3.625f*tmp43*tmp45;
tmp47=tmp3+0.78539816f;
tmp48=cos(tmp47);
tmp49=tmp0+(-0.78539816f);
tmp50=cos(tmp49);
tmp51=tmp12+0.087266463f;
tmp52=sin(tmp51);
tmp53=9.0f*tmp48*tmp50*tmp52;
tmp54=tmp12+0.087266463f;
tmp55=cos(tmp54);
tmp56=tmp0+(-0.78539816f);
tmp57=sin(tmp56);
tmp58=9.0f*tmp55*tmp57;
tmp59=tmp12+0.087266463f;
tmp60=cos(tmp59);
tmp61=13.5f*tmp60;
tmp62=tmp38+tmp41+tmp46+tmp53+tmp58+tmp61+12.0f;
residual2=tmp62;
tmp63=tmp3+0.78539816f;
tmp64=cos(tmp63);
tmp65=0.017453293f*_bicep;
tmp66=sin(tmp65);
tmp67=tmp64*tmp66;
tmp68=cos(tmp65);
tmp69=tmp3+0.78539816f;
tmp70=sin(tmp69);
tmp71=tmp0+(-0.78539816f);
tmp72=sin(tmp71);
tmp73=tmp68*tmp70*tmp72;
tmp74=(-1.0f)*pointx;
tmp75=tmp67+tmp73+tmp74;
residual3=tmp75;
tmp76=tmp0+(-0.78539816f);
tmp77=cos(tmp76);
tmp78=tmp3+0.78539816f;
tmp79=sin(tmp78);
tmp80=(-1.0f)*tmp77*tmp79;
tmp81=tmp80+downx;
residual4=tmp81;
// undefine global aliases
#undef epsilon
#undef steps

// undefine parameter aliases
#undef shoulderio
#undef shoulderud
#undef bicep
#undef elbow
#undef forearm
#undef downx
#undef pointx
#undef px
#undef py
#undef pz
#undef waist
#undef residual

// undefine variable aliases
#undef _shoulderio
#undef _shoulderud
#undef _bicep
#undef _elbow
#undef _forearm

// undefine residual aliases
#undef residual0
#undef residual1
#undef residual2
#undef residual3
#undef residual4

// undefine jacobian aliases (fortran order)
#undef dresidual0_d_shoulderio
#undef dresidual0_d_shoulderud
#undef dresidual0_d_bicep
#undef dresidual0_d_elbow
#undef dresidual0_d_forearm
#undef dresidual1_d_shoulderio
#undef dresidual1_d_shoulderud
#undef dresidual1_d_bicep
#undef dresidual1_d_elbow
#undef dresidual1_d_forearm
#undef dresidual2_d_shoulderio
#undef dresidual2_d_shoulderud
#undef dresidual2_d_bicep
#undef dresidual2_d_elbow
#undef dresidual2_d_forearm
#undef dresidual3_d_shoulderio
#undef dresidual3_d_shoulderud
#undef dresidual3_d_bicep
#undef dresidual3_d_elbow
#undef dresidual3_d_forearm
#undef dresidual4_d_shoulderio
#undef dresidual4_d_shoulderud
#undef dresidual4_d_bicep
#undef dresidual4_d_elbow
#undef dresidual4_d_forearm
} // ik_leftarmf()

void ik_leftarmdf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[12],
  const float ik_leftarmx[5],
  float ik_leftarmdy[25]
)
{
// global aliases
#define epsilon ik_leftarmglobals[0]
#define steps ik_leftarmglobals[1]

// parameter aliases
#define shoulderio ik_leftarmparameters[0]
#define shoulderud ik_leftarmparameters[1]
#define bicep ik_leftarmparameters[2]
#define elbow ik_leftarmparameters[3]
#define forearm ik_leftarmparameters[4]
#define downx ik_leftarmparameters[5]
#define pointx ik_leftarmparameters[6]
#define px ik_leftarmparameters[7]
#define py ik_leftarmparameters[8]
#define pz ik_leftarmparameters[9]
#define waist ik_leftarmparameters[10]
#define residual ik_leftarmparameters[11]

// variable aliases
#define _shoulderio ik_leftarmx[0]
#define _shoulderud ik_leftarmx[1]
#define _bicep ik_leftarmx[2]
#define _elbow ik_leftarmx[3]
#define _forearm ik_leftarmx[4]

// residual aliases
#define residual0 ik_leftarmy[0]
#define residual1 ik_leftarmy[1]
#define residual2 ik_leftarmy[2]
#define residual3 ik_leftarmy[3]
#define residual4 ik_leftarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[5]
#define dresidual0_d_bicep ik_leftarmdy[10]
#define dresidual0_d_elbow ik_leftarmdy[15]
#define dresidual0_d_forearm ik_leftarmdy[20]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[6]
#define dresidual1_d_bicep ik_leftarmdy[11]
#define dresidual1_d_elbow ik_leftarmdy[16]
#define dresidual1_d_forearm ik_leftarmdy[21]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[7]
#define dresidual2_d_bicep ik_leftarmdy[12]
#define dresidual2_d_elbow ik_leftarmdy[17]
#define dresidual2_d_forearm ik_leftarmdy[22]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[8]
#define dresidual3_d_bicep ik_leftarmdy[13]
#define dresidual3_d_elbow ik_leftarmdy[18]
#define dresidual3_d_forearm ik_leftarmdy[23]
#define dresidual4_d_shoulderio ik_leftarmdy[4]
#define dresidual4_d_shoulderud ik_leftarmdy[9]
#define dresidual4_d_bicep ik_leftarmdy[14]
#define dresidual4_d_elbow ik_leftarmdy[19]
#define dresidual4_d_forearm ik_leftarmdy[24]

float tmp0;
float tmp1;
float tmp2;
float tmp3;
float tmp4;
float tmp5;
float tmp6;
float tmp7;
float tmp8;
float tmp9;
float tmp10;
float tmp11;
float tmp12;
float tmp13;
float tmp14;
float tmp15;
float tmp16;
float tmp17;
float tmp18;
float tmp19;
float tmp20;
float tmp21;
float tmp22;
float tmp23;
float tmp24;
float tmp25;
float tmp26;
float tmp27;
float tmp28;
float tmp29;
float tmp30;
float tmp31;
float tmp32;
float tmp33;
float tmp34;
float tmp35;
float tmp36;
float tmp37;
float tmp38;
float tmp39;
float tmp40;
float tmp41;
float tmp42;
float tmp43;
float tmp44;
float tmp45;
float tmp46;
float tmp47;
float tmp48;
float tmp49;
float tmp50;
float tmp51;
float tmp52;
float tmp53;
float tmp54;
float tmp55;
float tmp56;
float tmp57;
float tmp58;
float tmp59;
float tmp60;
float tmp61;
float tmp62;
float tmp63;
float tmp64;
float tmp65;
float tmp66;
float tmp67;
float tmp68;
float tmp69;
float tmp70;
float tmp71;
float tmp72;
float tmp73;
float tmp74;
float tmp75;
float tmp76;
float tmp77;
float tmp78;
float tmp79;
float tmp80;
float tmp81;
float tmp82;
float tmp83;
float tmp84;
float tmp85;
float tmp86;
float tmp87;
float tmp88;
float tmp89;
float tmp90;
float tmp91;
float tmp92;
float tmp93;
float tmp94;
float tmp95;
float tmp96;
float tmp97;
float tmp98;
float tmp99;
float tmp100;
float tmp101;
float tmp102;
float tmp103;
float tmp104;
float tmp105;
float tmp106;
float tmp107;
tmp0=(-0.017453293f)*_shoulderio;
tmp1=tmp0+0.78539816f;
tmp2=cos(tmp1);
tmp3=0.063268185f*tmp2;
tmp4=tmp0+0.78539816f;
tmp5=cos(tmp4);
tmp6=0.017453293f*_shoulderud;
tmp7=tmp6+(-0.78539816f);
tmp8=cos(tmp7);
tmp9=0.15707963f*tmp5*tmp8;
tmp10=tmp3+tmp9;
dresidual0_d_shoulderio=tmp10;
tmp11=tmp0+0.78539816f;
tmp12=sin(tmp11);
tmp13=tmp6+(-0.78539816f);
tmp14=sin(tmp13);
tmp15=0.15707963f*tmp12*tmp14;
dresidual0_d_shoulderud=tmp15;
dresidual0_d_bicep=0.0f;
dresidual0_d_elbow=0.0f;
dresidual0_d_forearm=0.0f;
tmp16=0.017453293f*waist;
tmp17=tmp16+0.087266463f;
tmp18=cos(tmp17);
tmp19=tmp0+0.78539816f;
tmp20=sin(tmp19);
tmp21=0.063268185f*tmp18*tmp20;
tmp22=tmp6+(-0.78539816f);
tmp23=cos(tmp22);
tmp24=tmp16+0.087266463f;
tmp25=cos(tmp24);
tmp26=tmp0+0.78539816f;
tmp27=sin(tmp26);
tmp28=0.15707963f*tmp23*tmp25*tmp27;
tmp29=tmp21+tmp28;
dresidual1_d_shoulderio=tmp29;
tmp30=tmp0+0.78539816f;
tmp31=cos(tmp30);
tmp32=tmp16+0.087266463f;
tmp33=cos(tmp32);
tmp34=tmp6+(-0.78539816f);
tmp35=sin(tmp34);
tmp36=(-0.15707963f)*tmp31*tmp33*tmp35;
tmp37=tmp6+(-0.78539816f);
tmp38=cos(tmp37);
tmp39=tmp16+0.087266463f;
tmp40=sin(tmp39);
tmp41=(-0.15707963f)*tmp38*tmp40;
tmp42=tmp36+tmp41;
dresidual1_d_shoulderud=tmp42;
dresidual1_d_bicep=0.0f;
dresidual1_d_elbow=0.0f;
dresidual1_d_forearm=0.0f;
tmp43=tmp0+0.78539816f;
tmp44=sin(tmp43);
tmp45=tmp16+0.087266463f;
tmp46=sin(tmp45);
tmp47=0.063268185f*tmp44*tmp46;
tmp48=tmp6+(-0.78539816f);
tmp49=cos(tmp48);
tmp50=tmp0+0.78539816f;
tmp51=sin(tmp50);
tmp52=tmp16+0.087266463f;
tmp53=sin(tmp52);
tmp54=0.15707963f*tmp49*tmp51*tmp53;
tmp55=tmp47+tmp54;
dresidual2_d_shoulderio=tmp55;
tmp56=tmp0+0.78539816f;
tmp57=cos(tmp56);
tmp58=tmp6+(-0.78539816f);
tmp59=sin(tmp58);
tmp60=tmp16+0.087266463f;
tmp61=sin(tmp60);
tmp62=(-0.15707963f)*tmp57*tmp59*tmp61;
tmp63=tmp6+(-0.78539816f);
tmp64=cos(tmp63);
tmp65=tmp16+0.087266463f;
tmp66=cos(tmp65);
tmp67=0.15707963f*tmp64*tmp66;
tmp68=tmp62+tmp67;
dresidual2_d_shoulderud=tmp68;
dresidual2_d_bicep=0.0f;
dresidual2_d_elbow=0.0f;
dresidual2_d_forearm=0.0f;
tmp69=tmp0+0.78539816f;
tmp70=cos(tmp69);
tmp71=0.017453293f*_bicep;
tmp72=cos(tmp71);
tmp73=tmp6+(-0.78539816f);
tmp74=sin(tmp73);
tmp75=(-0.017453293f)*tmp70*tmp72*tmp74;
tmp76=tmp0+0.78539816f;
tmp77=sin(tmp76);
tmp78=sin(tmp71);
tmp79=0.017453293f*tmp77*tmp78;
tmp80=tmp75+tmp79;
dresidual3_d_shoulderio=tmp80;
tmp81=tmp6+(-0.78539816f);
tmp82=cos(tmp81);
tmp83=cos(tmp71);
tmp84=tmp0+0.78539816f;
tmp85=sin(tmp84);
tmp86=0.017453293f*tmp82*tmp83*tmp85;
dresidual3_d_shoulderud=tmp86;
tmp87=tmp0+0.78539816f;
tmp88=sin(tmp87);
tmp89=tmp6+(-0.78539816f);
tmp90=sin(tmp89);
tmp91=sin(tmp71);
tmp92=(-0.017453293f)*tmp88*tmp90*tmp91;
tmp93=tmp0+0.78539816f;
tmp94=cos(tmp93);
tmp95=cos(tmp71);
tmp96=0.017453293f*tmp94*tmp95;
tmp97=tmp92+tmp96;
dresidual3_d_bicep=tmp97;
dresidual3_d_elbow=0.0f;
dresidual3_d_forearm=0.0f;
tmp98=tmp0+0.78539816f;
tmp99=cos(tmp98);
tmp100=tmp6+(-0.78539816f);
tmp101=cos(tmp100);
tmp102=0.017453293f*tmp99*tmp101;
dresidual4_d_shoulderio=tmp102;
tmp103=tmp0+0.78539816f;
tmp104=sin(tmp103);
tmp105=tmp6+(-0.78539816f);
tmp106=sin(tmp105);
tmp107=0.017453293f*tmp104*tmp106;
dresidual4_d_shoulderud=tmp107;
dresidual4_d_bicep=0.0f;
dresidual4_d_elbow=0.0f;
dresidual4_d_forearm=0.0f;
// undefine global aliases
#undef epsilon
#undef steps

// undefine parameter aliases
#undef shoulderio
#undef shoulderud
#undef bicep
#undef elbow
#undef forearm
#undef downx
#undef pointx
#undef px
#undef py
#undef pz
#undef waist
#undef residual

// undefine variable aliases
#undef _shoulderio
#undef _shoulderud
#undef _bicep
#undef _elbow
#undef _forearm

// undefine residual aliases
#undef residual0
#undef residual1
#undef residual2
#undef residual3
#undef residual4

// undefine jacobian aliases (fortran order)
#undef dresidual0_d_shoulderio
#undef dresidual0_d_shoulderud
#undef dresidual0_d_bicep
#undef dresidual0_d_elbow
#undef dresidual0_d_forearm
#undef dresidual1_d_shoulderio
#undef dresidual1_d_shoulderud
#undef dresidual1_d_bicep
#undef dresidual1_d_elbow
#undef dresidual1_d_forearm
#undef dresidual2_d_shoulderio
#undef dresidual2_d_shoulderud
#undef dresidual2_d_bicep
#undef dresidual2_d_elbow
#undef dresidual2_d_forearm
#undef dresidual3_d_shoulderio
#undef dresidual3_d_shoulderud
#undef dresidual3_d_bicep
#undef dresidual3_d_elbow
#undef dresidual3_d_forearm
#undef dresidual4_d_shoulderio
#undef dresidual4_d_shoulderud
#undef dresidual4_d_bicep
#undef dresidual4_d_elbow
#undef dresidual4_d_forearm
} // ik_leftarmdf()
