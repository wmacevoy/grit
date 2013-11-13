#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <cassert>
enum { ik_rightarmglobal_count=2 };
extern const char *ik_rightarmglobal_names[2];

enum { ik_rightarmparameter_count=12 };
extern const char *ik_rightarmparameter_names[12];

enum { ik_rightarmvariable_count=5 };
extern const char *ik_rightarmvariable_names[5];

enum { ik_rightarmequation_count=5 };
extern const char *ik_rightarmequation_names[5];
const char *ik_rightarmglobal_names[]={
  "epsilon",
  "steps"
};

const char *ik_rightarmparameter_names[]={
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

const char *ik_rightarmvariable_names[]={
  "_shoulderio",
  "_shoulderud",
  "_bicep",
  "_elbow",
  "_forearm"
};

const char *ik_rightarmequation_names[]={
  "residual0",
  "residual1",
  "residual2",
  "residual3",
  "residual4"
};

class ik_rightarmStopwatch
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
 float ik_rightarmdist2(int dim, const float *x, const float *y)
{
  float sum2=0;
  for (int k=0; k<dim; ++k) {
    float dx=x[k]-y[k];
    sum2 += dx*dx;
  }
  return sum2;
} // dist2()
 float ik_rightarmdist(int dim, const float *x, const float *y)
{
  return sqrt(ik_rightarmdist2(dim,x,y));
} // dist()
 float ik_rightarmnorm2(int dim, const float *x)
{
  float sum2=0;
  for (int k=0; k<dim; ++k) {
    float dx=x[k];
    sum2 += dx*dx;
  }
  return sum2;
} // dist2()
 float ik_rightarmnorm(int dim, const float *x)
{
  return sqrt(ik_rightarmnorm2(dim,x));
} // dist()
 int ik_rightarmDoolittle_LU_Decomposition_with_Pivoting(float *A, int pivot[], int n)
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


 int ik_rightarmDoolittle_LU_with_Pivoting_Solve(float *A, float B[], int pivot[],
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

 void ik_rightarmlinear_solve(int dim, int *ipiv, float *A, float *b, float *x)
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
  
    ik_rightarmDoolittle_LU_Decomposition_with_Pivoting(A,ipiv,dim);
    ik_rightarmDoolittle_LU_with_Pivoting_Solve(A,b,ipiv,x,dim);
}
void ik_rightarmf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[12],
  const float ik_rightarmx[5],
  float ik_rightarmy[5]
);
void ik_rightarmdf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[12],
  const float ik_rightarmx[5],
  float ik_rightarmdy[25]
);
void ik_rightarminitialize(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[12],
  float ik_rightarmx[5]
);
void ik_rightarmupdate(
  const float ik_rightarmglobals[2],
  float ik_rightarmparameters[12]
);
// damped newton solver

   void ik_rightarmsolve(
    
    const float globals[2],
    float *parameters,
    float *x
  )
  {
    ik_rightarminitialize(globals,parameters,x);
    // newton iteration
    int steps=int(globals[1]);
    float epsilon=globals[0];
    float residual,new_residual;
  
    for (int step=0; step<steps; ++step) {
      float y[5];
      float dx[5];
      float dy[25];
      int iwork[5];
  
      ik_rightarmf(globals,parameters,x,y);
      residual = ik_rightarmnorm(5,y);
      ik_rightarmdf(globals,parameters,x,dy);

      ik_rightarmlinear_solve(5,iwork,dy,y,dx);
      for (int k=0; k<5; ++k) { x[k] -= dx[k]; }

      for (int damping=0; damping < steps; ++damping) {
	    ik_rightarmf(globals,parameters,x,y);
	    new_residual=ik_rightarmnorm(5,y);
	    if (new_residual < residual) break;
	    for (int k=0; k<5; ++k) { dx[k] /= 2; }
	    for (int k=0; k<5; ++k) { x[k] += dx[k] ; }
      }
      residual=new_residual;
      if (residual <= epsilon) break;
    }
    parameters[11]=residual;
  } //  ik_rightarmsolve()
typedef struct {
  const char *ik_rightarmname;
  float ik_rightarmglobals[2];
  float ik_rightarmparameters[12];
  float ik_rightarmx[5];
} ik_rightarmtest_data_t;

ik_rightarmtest_data_t ik_rightarmtest_data[] = {
}; // ik_rightarmtest_data
void ik_rightarmtests(const std::string &ik_rightarmname)
{
  std::string ik_rightarmcase_name;
  std::string ik_rightarmmax_x_error_name="none";
  int ik_rightarmmax_x_error_count=0;
  float ik_rightarmmax_x_error=0;
  float ik_rightarmx_error=0;
  std::string ik_rightarmmax_y_error_name="none";
  int ik_rightarmmax_y_error_count=0;
  float ik_rightarmmax_y_error=0;
  float ik_rightarmy_error=0;

  float ik_rightarmx_test[5];
  float ik_rightarmy_test[5];

  for (int ik_rightarmcase_count=0; ik_rightarmcase_count < 0; ++ik_rightarmcase_count) {
    if (ik_rightarmname == "all" || ik_rightarmname == ik_rightarmtest_data[ik_rightarmcase_count].ik_rightarmname) {
       const char *ik_rightarmname=ik_rightarmtest_data[ik_rightarmcase_count].ik_rightarmname;
       float *ik_rightarmglobals=ik_rightarmtest_data[ik_rightarmcase_count].ik_rightarmglobals;
       float *ik_rightarmparameters=ik_rightarmtest_data[ik_rightarmcase_count].ik_rightarmparameters;
       float *ik_rightarmx=ik_rightarmtest_data[ik_rightarmcase_count].ik_rightarmx;

       ik_rightarmsolve(ik_rightarmglobals,ik_rightarmparameters,ik_rightarmx_test);
       ik_rightarmf(ik_rightarmglobals,ik_rightarmparameters,ik_rightarmx_test,ik_rightarmy_test);
       ik_rightarmx_error=ik_rightarmdist(5,ik_rightarmx,ik_rightarmx_test);
       ik_rightarmy_error=ik_rightarmnorm(5,ik_rightarmy_test);

       if (ik_rightarmx_error > ik_rightarmmax_x_error) {
         ik_rightarmmax_x_error=ik_rightarmx_error;
         ik_rightarmmax_x_error_name=ik_rightarmname;
         ik_rightarmmax_x_error_count=ik_rightarmcase_count;
       }

       if (ik_rightarmy_error > ik_rightarmmax_y_error) {
         ik_rightarmmax_y_error=ik_rightarmy_error;
         ik_rightarmmax_y_error_name=ik_rightarmname;
         ik_rightarmmax_y_error_count=ik_rightarmcase_count;
       }

       std::cout << "test " << (ik_rightarmcase_count+1) << "/" << "\"" << ik_rightarmname << "\"" << ": " << "x_error=" << std::setprecision(15) << ik_rightarmx_error << "," << "y_error=" << std::setprecision(15) << ik_rightarmy_error << std::endl;
     }
   }
   if (ik_rightarmname == "all") {
     std::cout << "test summary: " 
       << "max_x_error=" << std::setprecision(15) << ik_rightarmmax_x_error
          << " from " << ik_rightarmmax_x_error_count << "/" 
          << "\"" << ik_rightarmmax_x_error_name << "\""
       << ","
       << "max_y_error=" << std::setprecision(15) << ik_rightarmmax_y_error
          << " from " << ik_rightarmmax_y_error_count << "/" 
          << "\"" << ik_rightarmmax_y_error_name << "\""
       << std::endl;
   }
}
typedef struct {
  const char *ik_rightarmname;
  float ik_rightarmglobals[2];
  float ik_rightarmparameters[12];
} ik_rightarmrun_data_t;

ik_rightarmrun_data_t ik_rightarmrun_data[] = {
}; // ik_rightarmrun_data
void ik_rightarmruns(const std::string &ik_rightarmname)
{
  std::string ik_rightarmcase_name;
  for (int ik_rightarmcase_count=0; ik_rightarmcase_count < 0; ++ik_rightarmcase_count) {
    if (ik_rightarmname == "all" || ik_rightarmname == ik_rightarmrun_data[ik_rightarmcase_count].ik_rightarmname) {
      const char *ik_rightarmcase_name=ik_rightarmrun_data[ik_rightarmcase_count].ik_rightarmname;
      float *ik_rightarmglobals=ik_rightarmrun_data[ik_rightarmcase_count].ik_rightarmglobals;
      float *ik_rightarmparameters=ik_rightarmrun_data[ik_rightarmcase_count].ik_rightarmparameters;
      float ik_rightarmx[5];

      ik_rightarmsolve(ik_rightarmglobals,ik_rightarmparameters,ik_rightarmx);
      std::cout << "[case] # " << (ik_rightarmcase_count+1) << std::endl;
      std::cout << "name=\"" << ik_rightarmcase_name << "\"" << std::endl;

      for (int i=0; i<ik_rightarmglobal_count; ++i) {
        std::cout << ik_rightarmglobal_names[i] << "=" << std::setprecision(15) << ik_rightarmglobals[i] << std::endl;
      }

      for (int i=0; i<ik_rightarmparameter_count; ++i) {
        std::cout << ik_rightarmparameter_names[i] << "=" << std::setprecision(15) << ik_rightarmparameters[i] << std::endl;
      }

      for (int i=0; i<ik_rightarmvariable_count; ++i) {
        std::cout << ik_rightarmvariable_names[i] << "=" << std::setprecision(15) << ik_rightarmx[i] << std::endl;
      }
    }
  }
} // ik_rightarmruns()
// global aliases
#define epsilon ik_rightarmglobals[0]
#define steps ik_rightarmglobals[1]

// parameter aliases
#define shoulderio ik_rightarmparameters[0]
#define shoulderud ik_rightarmparameters[1]
#define bicep ik_rightarmparameters[2]
#define elbow ik_rightarmparameters[3]
#define forearm ik_rightarmparameters[4]
#define downx ik_rightarmparameters[5]
#define pointx ik_rightarmparameters[6]
#define px ik_rightarmparameters[7]
#define py ik_rightarmparameters[8]
#define pz ik_rightarmparameters[9]
#define waist ik_rightarmparameters[10]
#define residual ik_rightarmparameters[11]

// variable aliases
#define _shoulderio ik_rightarmx[0]
#define _shoulderud ik_rightarmx[1]
#define _bicep ik_rightarmx[2]
#define _elbow ik_rightarmx[3]
#define _forearm ik_rightarmx[4]

// residual aliases
#define residual0 ik_rightarmy[0]
#define residual1 ik_rightarmy[1]
#define residual2 ik_rightarmy[2]
#define residual3 ik_rightarmy[3]
#define residual4 ik_rightarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_rightarmdy[0]
#define dresidual0_d_shoulderud ik_rightarmdy[5]
#define dresidual0_d_bicep ik_rightarmdy[10]
#define dresidual0_d_elbow ik_rightarmdy[15]
#define dresidual0_d_forearm ik_rightarmdy[20]
#define dresidual1_d_shoulderio ik_rightarmdy[1]
#define dresidual1_d_shoulderud ik_rightarmdy[6]
#define dresidual1_d_bicep ik_rightarmdy[11]
#define dresidual1_d_elbow ik_rightarmdy[16]
#define dresidual1_d_forearm ik_rightarmdy[21]
#define dresidual2_d_shoulderio ik_rightarmdy[2]
#define dresidual2_d_shoulderud ik_rightarmdy[7]
#define dresidual2_d_bicep ik_rightarmdy[12]
#define dresidual2_d_elbow ik_rightarmdy[17]
#define dresidual2_d_forearm ik_rightarmdy[22]
#define dresidual3_d_shoulderio ik_rightarmdy[3]
#define dresidual3_d_shoulderud ik_rightarmdy[8]
#define dresidual3_d_bicep ik_rightarmdy[13]
#define dresidual3_d_elbow ik_rightarmdy[18]
#define dresidual3_d_forearm ik_rightarmdy[23]
#define dresidual4_d_shoulderio ik_rightarmdy[4]
#define dresidual4_d_shoulderud ik_rightarmdy[9]
#define dresidual4_d_bicep ik_rightarmdy[14]
#define dresidual4_d_elbow ik_rightarmdy[19]
#define dresidual4_d_forearm ik_rightarmdy[24]

void ik_rightarminitialize(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[12],
  float ik_rightarmx[5]
)
{
// global aliases
#define epsilon ik_rightarmglobals[0]
#define steps ik_rightarmglobals[1]

// parameter aliases
#define shoulderio ik_rightarmparameters[0]
#define shoulderud ik_rightarmparameters[1]
#define bicep ik_rightarmparameters[2]
#define elbow ik_rightarmparameters[3]
#define forearm ik_rightarmparameters[4]
#define downx ik_rightarmparameters[5]
#define pointx ik_rightarmparameters[6]
#define px ik_rightarmparameters[7]
#define py ik_rightarmparameters[8]
#define pz ik_rightarmparameters[9]
#define waist ik_rightarmparameters[10]
#define residual ik_rightarmparameters[11]

// variable aliases
#define _shoulderio ik_rightarmx[0]
#define _shoulderud ik_rightarmx[1]
#define _bicep ik_rightarmx[2]
#define _elbow ik_rightarmx[3]
#define _forearm ik_rightarmx[4]

// residual aliases
#define residual0 ik_rightarmy[0]
#define residual1 ik_rightarmy[1]
#define residual2 ik_rightarmy[2]
#define residual3 ik_rightarmy[3]
#define residual4 ik_rightarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_rightarmdy[0]
#define dresidual0_d_shoulderud ik_rightarmdy[5]
#define dresidual0_d_bicep ik_rightarmdy[10]
#define dresidual0_d_elbow ik_rightarmdy[15]
#define dresidual0_d_forearm ik_rightarmdy[20]
#define dresidual1_d_shoulderio ik_rightarmdy[1]
#define dresidual1_d_shoulderud ik_rightarmdy[6]
#define dresidual1_d_bicep ik_rightarmdy[11]
#define dresidual1_d_elbow ik_rightarmdy[16]
#define dresidual1_d_forearm ik_rightarmdy[21]
#define dresidual2_d_shoulderio ik_rightarmdy[2]
#define dresidual2_d_shoulderud ik_rightarmdy[7]
#define dresidual2_d_bicep ik_rightarmdy[12]
#define dresidual2_d_elbow ik_rightarmdy[17]
#define dresidual2_d_forearm ik_rightarmdy[22]
#define dresidual3_d_shoulderio ik_rightarmdy[3]
#define dresidual3_d_shoulderud ik_rightarmdy[8]
#define dresidual3_d_bicep ik_rightarmdy[13]
#define dresidual3_d_elbow ik_rightarmdy[18]
#define dresidual3_d_forearm ik_rightarmdy[23]
#define dresidual4_d_shoulderio ik_rightarmdy[4]
#define dresidual4_d_shoulderud ik_rightarmdy[9]
#define dresidual4_d_bicep ik_rightarmdy[14]
#define dresidual4_d_elbow ik_rightarmdy[19]
#define dresidual4_d_forearm ik_rightarmdy[24]

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
} // ik_rightarminitialize()
void ik_rightarmupdate(
  const float ik_rightarmglobals[2],
  float ik_rightarmparameters[12]
)
{
  float ik_rightarmx[5];
  ik_rightarmsolve(ik_rightarmglobals,ik_rightarmparameters,ik_rightarmx);
// global aliases
#define epsilon ik_rightarmglobals[0]
#define steps ik_rightarmglobals[1]

// parameter aliases
#define shoulderio ik_rightarmparameters[0]
#define shoulderud ik_rightarmparameters[1]
#define bicep ik_rightarmparameters[2]
#define elbow ik_rightarmparameters[3]
#define forearm ik_rightarmparameters[4]
#define downx ik_rightarmparameters[5]
#define pointx ik_rightarmparameters[6]
#define px ik_rightarmparameters[7]
#define py ik_rightarmparameters[8]
#define pz ik_rightarmparameters[9]
#define waist ik_rightarmparameters[10]
#define residual ik_rightarmparameters[11]

// variable aliases
#define _shoulderio ik_rightarmx[0]
#define _shoulderud ik_rightarmx[1]
#define _bicep ik_rightarmx[2]
#define _elbow ik_rightarmx[3]
#define _forearm ik_rightarmx[4]

// residual aliases
#define residual0 ik_rightarmy[0]
#define residual1 ik_rightarmy[1]
#define residual2 ik_rightarmy[2]
#define residual3 ik_rightarmy[3]
#define residual4 ik_rightarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_rightarmdy[0]
#define dresidual0_d_shoulderud ik_rightarmdy[5]
#define dresidual0_d_bicep ik_rightarmdy[10]
#define dresidual0_d_elbow ik_rightarmdy[15]
#define dresidual0_d_forearm ik_rightarmdy[20]
#define dresidual1_d_shoulderio ik_rightarmdy[1]
#define dresidual1_d_shoulderud ik_rightarmdy[6]
#define dresidual1_d_bicep ik_rightarmdy[11]
#define dresidual1_d_elbow ik_rightarmdy[16]
#define dresidual1_d_forearm ik_rightarmdy[21]
#define dresidual2_d_shoulderio ik_rightarmdy[2]
#define dresidual2_d_shoulderud ik_rightarmdy[7]
#define dresidual2_d_bicep ik_rightarmdy[12]
#define dresidual2_d_elbow ik_rightarmdy[17]
#define dresidual2_d_forearm ik_rightarmdy[22]
#define dresidual3_d_shoulderio ik_rightarmdy[3]
#define dresidual3_d_shoulderud ik_rightarmdy[8]
#define dresidual3_d_bicep ik_rightarmdy[13]
#define dresidual3_d_elbow ik_rightarmdy[18]
#define dresidual3_d_forearm ik_rightarmdy[23]
#define dresidual4_d_shoulderio ik_rightarmdy[4]
#define dresidual4_d_shoulderud ik_rightarmdy[9]
#define dresidual4_d_bicep ik_rightarmdy[14]
#define dresidual4_d_elbow ik_rightarmdy[19]
#define dresidual4_d_forearm ik_rightarmdy[24]

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
} // ik_rightarmupdate()

void ik_rightarmf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[12],
  const float ik_rightarmx[5],
  float ik_rightarmy[5]
)
{
// global aliases
#define epsilon ik_rightarmglobals[0]
#define steps ik_rightarmglobals[1]

// parameter aliases
#define shoulderio ik_rightarmparameters[0]
#define shoulderud ik_rightarmparameters[1]
#define bicep ik_rightarmparameters[2]
#define elbow ik_rightarmparameters[3]
#define forearm ik_rightarmparameters[4]
#define downx ik_rightarmparameters[5]
#define pointx ik_rightarmparameters[6]
#define px ik_rightarmparameters[7]
#define py ik_rightarmparameters[8]
#define pz ik_rightarmparameters[9]
#define waist ik_rightarmparameters[10]
#define residual ik_rightarmparameters[11]

// variable aliases
#define _shoulderio ik_rightarmx[0]
#define _shoulderud ik_rightarmx[1]
#define _bicep ik_rightarmx[2]
#define _elbow ik_rightarmx[3]
#define _forearm ik_rightarmx[4]

// residual aliases
#define residual0 ik_rightarmy[0]
#define residual1 ik_rightarmy[1]
#define residual2 ik_rightarmy[2]
#define residual3 ik_rightarmy[3]
#define residual4 ik_rightarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_rightarmdy[0]
#define dresidual0_d_shoulderud ik_rightarmdy[5]
#define dresidual0_d_bicep ik_rightarmdy[10]
#define dresidual0_d_elbow ik_rightarmdy[15]
#define dresidual0_d_forearm ik_rightarmdy[20]
#define dresidual1_d_shoulderio ik_rightarmdy[1]
#define dresidual1_d_shoulderud ik_rightarmdy[6]
#define dresidual1_d_bicep ik_rightarmdy[11]
#define dresidual1_d_elbow ik_rightarmdy[16]
#define dresidual1_d_forearm ik_rightarmdy[21]
#define dresidual2_d_shoulderio ik_rightarmdy[2]
#define dresidual2_d_shoulderud ik_rightarmdy[7]
#define dresidual2_d_bicep ik_rightarmdy[12]
#define dresidual2_d_elbow ik_rightarmdy[17]
#define dresidual2_d_forearm ik_rightarmdy[22]
#define dresidual3_d_shoulderio ik_rightarmdy[3]
#define dresidual3_d_shoulderud ik_rightarmdy[8]
#define dresidual3_d_bicep ik_rightarmdy[13]
#define dresidual3_d_elbow ik_rightarmdy[18]
#define dresidual3_d_forearm ik_rightarmdy[23]
#define dresidual4_d_shoulderio ik_rightarmdy[4]
#define dresidual4_d_shoulderud ik_rightarmdy[9]
#define dresidual4_d_bicep ik_rightarmdy[14]
#define dresidual4_d_elbow ik_rightarmdy[19]
#define dresidual4_d_forearm ik_rightarmdy[24]

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
float tmp108;
float tmp109;
float tmp110;
float tmp111;
float tmp112;
float tmp113;
float tmp114;
float tmp115;
float tmp116;
float tmp117;
float tmp118;
float tmp119;
float tmp120;
float tmp121;
float tmp122;
float tmp123;
float tmp124;
float tmp125;
float tmp126;
float tmp127;
float tmp128;
float tmp129;
float tmp130;
float tmp131;
float tmp132;
float tmp133;
float tmp134;
float tmp135;
float tmp136;
float tmp137;
float tmp138;
float tmp139;
float tmp140;
float tmp141;
float tmp142;
float tmp143;
float tmp144;
float tmp145;
float tmp146;
float tmp147;
float tmp148;
float tmp149;
float tmp150;
float tmp151;
float tmp152;
float tmp153;
float tmp154;
float tmp155;
float tmp156;
float tmp157;
float tmp158;
float tmp159;
float tmp160;
float tmp161;
float tmp162;
float tmp163;
float tmp164;
float tmp165;
float tmp166;
float tmp167;
float tmp168;
float tmp169;
float tmp170;
float tmp171;
float tmp172;
float tmp173;
float tmp174;
float tmp175;
float tmp176;
float tmp177;
float tmp178;
float tmp179;
float tmp180;
float tmp181;
float tmp182;
float tmp183;
float tmp184;
float tmp185;
float tmp186;
float tmp187;
float tmp188;
float tmp189;
float tmp190;
float tmp191;
float tmp192;
float tmp193;
float tmp194;
float tmp195;
float tmp196;
float tmp197;
float tmp198;
float tmp199;
float tmp200;
float tmp201;
float tmp202;
float tmp203;
float tmp204;
float tmp205;
float tmp206;
float tmp207;
float tmp208;
float tmp209;
float tmp210;
float tmp211;
float tmp212;
float tmp213;
float tmp214;
float tmp215;
float tmp216;
float tmp217;
float tmp218;
float tmp219;
float tmp220;
float tmp221;
float tmp222;
float tmp223;
float tmp224;
float tmp225;
float tmp226;
float tmp227;
float tmp228;
float tmp229;
float tmp230;
float tmp231;
tmp0=0.017453293f*_elbow;
tmp1=tmp0+(-0.78539816f);
tmp2=cos(tmp1);
tmp3=0.017453293f*_shoulderud;
tmp4=tmp3+(-0.78539816f);
tmp5=cos(tmp4);
tmp6=0.017453293f*_shoulderio;
tmp7=tmp6+(-0.78539816f);
tmp8=sin(tmp7);
tmp9=(-15.0f)*tmp2*tmp5*tmp8;
tmp10=tmp3+(-0.78539816f);
tmp11=cos(tmp10);
tmp12=tmp6+(-0.78539816f);
tmp13=sin(tmp12);
tmp14=(-9.0f)*tmp11*tmp13;
tmp15=tmp6+(-0.78539816f);
tmp16=sin(tmp15);
tmp17=(-3.625f)*tmp16;
tmp18=(-1.0f)*px;
tmp19=tmp0+(-0.78539816f);
tmp20=sin(tmp19);
tmp21=tmp6+(-0.78539816f);
tmp22=cos(tmp21);
tmp23=(-0.017453293f)*_bicep;
tmp24=sin(tmp23);
tmp25=tmp22*tmp24;
tmp26=cos(tmp23);
tmp27=tmp6+(-0.78539816f);
tmp28=sin(tmp27);
tmp29=tmp3+(-0.78539816f);
tmp30=sin(tmp29);
tmp31=tmp26*tmp28*tmp30;
tmp32=tmp25+tmp31;
tmp33=15.0f*tmp20*tmp32;
tmp34=tmp9+tmp14+tmp17+tmp18+tmp33+6.0f;
residual0=tmp34;
tmp35=0.017453293f*waist;
tmp36=tmp35+0.087266463f;
tmp37=sin(tmp36);
tmp38=(-13.5f)*tmp37;
tmp39=tmp3+(-0.78539816f);
tmp40=sin(tmp39);
tmp41=tmp35+0.087266463f;
tmp42=sin(tmp41);
tmp43=(-9.0f)*tmp40*tmp42;
tmp44=(-1.0f)*py;
tmp45=tmp35+0.087266463f;
tmp46=cos(tmp45);
tmp47=2.375f*tmp46;
tmp48=tmp6+(-0.78539816f);
tmp49=cos(tmp48);
tmp50=tmp35+0.087266463f;
tmp51=cos(tmp50);
tmp52=3.625f*tmp49*tmp51;
tmp53=tmp6+(-0.78539816f);
tmp54=cos(tmp53);
tmp55=tmp3+(-0.78539816f);
tmp56=cos(tmp55);
tmp57=tmp35+0.087266463f;
tmp58=cos(tmp57);
tmp59=9.0f*tmp54*tmp56*tmp58;
tmp60=tmp0+(-0.78539816f);
tmp61=cos(tmp60);
tmp62=tmp6+(-0.78539816f);
tmp63=cos(tmp62);
tmp64=tmp3+(-0.78539816f);
tmp65=cos(tmp64);
tmp66=tmp35+0.087266463f;
tmp67=cos(tmp66);
tmp68=tmp63*tmp65*tmp67;
tmp69=tmp3+(-0.78539816f);
tmp70=sin(tmp69);
tmp71=tmp35+0.087266463f;
tmp72=sin(tmp71);
tmp73=(-1.0f)*tmp70*tmp72;
tmp74=tmp68+tmp73;
tmp75=15.0f*tmp61*tmp74;
tmp76=tmp0+(-0.78539816f);
tmp77=sin(tmp76);
tmp78=tmp35+0.087266463f;
tmp79=cos(tmp78);
tmp80=tmp6+(-0.78539816f);
tmp81=sin(tmp80);
tmp82=sin(tmp23);
tmp83=tmp79*tmp81*tmp82;
tmp84=cos(tmp23);
tmp85=tmp6+(-0.78539816f);
tmp86=cos(tmp85);
tmp87=tmp35+0.087266463f;
tmp88=cos(tmp87);
tmp89=tmp3+(-0.78539816f);
tmp90=sin(tmp89);
tmp91=(-1.0f)*tmp86*tmp88*tmp90;
tmp92=tmp3+(-0.78539816f);
tmp93=cos(tmp92);
tmp94=tmp35+0.087266463f;
tmp95=sin(tmp94);
tmp96=(-1.0f)*tmp93*tmp95;
tmp97=tmp91+tmp96;
tmp98=tmp84*tmp97;
tmp99=tmp83+tmp98;
tmp100=15.0f*tmp77*tmp99;
tmp101=tmp38+tmp43+tmp44+tmp47+tmp52+tmp59+tmp75+tmp100;
residual1=tmp101;
tmp102=(-1.0f)*pz;
tmp103=tmp35+0.087266463f;
tmp104=sin(tmp103);
tmp105=2.375f*tmp104;
tmp106=tmp6+(-0.78539816f);
tmp107=cos(tmp106);
tmp108=tmp35+0.087266463f;
tmp109=sin(tmp108);
tmp110=3.625f*tmp107*tmp109;
tmp111=tmp6+(-0.78539816f);
tmp112=cos(tmp111);
tmp113=tmp3+(-0.78539816f);
tmp114=cos(tmp113);
tmp115=tmp35+0.087266463f;
tmp116=sin(tmp115);
tmp117=9.0f*tmp112*tmp114*tmp116;
tmp118=tmp35+0.087266463f;
tmp119=cos(tmp118);
tmp120=tmp3+(-0.78539816f);
tmp121=sin(tmp120);
tmp122=9.0f*tmp119*tmp121;
tmp123=tmp35+0.087266463f;
tmp124=cos(tmp123);
tmp125=13.5f*tmp124;
tmp126=tmp0+(-0.78539816f);
tmp127=cos(tmp126);
tmp128=tmp6+(-0.78539816f);
tmp129=cos(tmp128);
tmp130=tmp3+(-0.78539816f);
tmp131=cos(tmp130);
tmp132=tmp35+0.087266463f;
tmp133=sin(tmp132);
tmp134=tmp129*tmp131*tmp133;
tmp135=tmp35+0.087266463f;
tmp136=cos(tmp135);
tmp137=tmp3+(-0.78539816f);
tmp138=sin(tmp137);
tmp139=tmp136*tmp138;
tmp140=tmp134+tmp139;
tmp141=15.0f*tmp127*tmp140;
tmp142=tmp0+(-0.78539816f);
tmp143=sin(tmp142);
tmp144=cos(tmp23);
tmp145=tmp3+(-0.78539816f);
tmp146=cos(tmp145);
tmp147=tmp35+0.087266463f;
tmp148=cos(tmp147);
tmp149=tmp146*tmp148;
tmp150=tmp6+(-0.78539816f);
tmp151=cos(tmp150);
tmp152=tmp3+(-0.78539816f);
tmp153=sin(tmp152);
tmp154=tmp35+0.087266463f;
tmp155=sin(tmp154);
tmp156=(-1.0f)*tmp151*tmp153*tmp155;
tmp157=tmp149+tmp156;
tmp158=tmp144*tmp157;
tmp159=tmp6+(-0.78539816f);
tmp160=sin(tmp159);
tmp161=tmp35+0.087266463f;
tmp162=sin(tmp161);
tmp163=sin(tmp23);
tmp164=tmp160*tmp162*tmp163;
tmp165=tmp158+tmp164;
tmp166=15.0f*tmp143*tmp165;
tmp167=tmp102+tmp105+tmp110+tmp117+tmp122+tmp125+tmp141+tmp166+12.0f;
residual2=tmp167;
tmp168=tmp0+(-0.78539816f);
tmp169=sin(tmp168);
tmp170=tmp6+(-0.78539816f);
tmp171=cos(tmp170);
tmp172=sin(tmp23);
tmp173=tmp171*tmp172;
tmp174=cos(tmp23);
tmp175=tmp6+(-0.78539816f);
tmp176=sin(tmp175);
tmp177=tmp3+(-0.78539816f);
tmp178=sin(tmp177);
tmp179=tmp174*tmp176*tmp178;
tmp180=tmp173+tmp179;
tmp181=tmp169*tmp180;
tmp182=tmp0+(-0.78539816f);
tmp183=cos(tmp182);
tmp184=tmp3+(-0.78539816f);
tmp185=cos(tmp184);
tmp186=tmp6+(-0.78539816f);
tmp187=sin(tmp186);
tmp188=(-1.0f)*tmp183*tmp185*tmp187;
tmp189=(-1.0f)*pointx;
tmp190=tmp181+tmp188+tmp189;
residual3=tmp190;
tmp191=(-0.017453293f)*_forearm;
tmp192=tmp191+0.6981317f;
tmp193=cos(tmp192);
tmp194=tmp0+(-0.78539816f);
tmp195=cos(tmp194);
tmp196=tmp6+(-0.78539816f);
tmp197=cos(tmp196);
tmp198=sin(tmp23);
tmp199=tmp197*tmp198;
tmp200=cos(tmp23);
tmp201=tmp6+(-0.78539816f);
tmp202=sin(tmp201);
tmp203=tmp3+(-0.78539816f);
tmp204=sin(tmp203);
tmp205=tmp200*tmp202*tmp204;
tmp206=tmp199+tmp205;
tmp207=tmp195*tmp206;
tmp208=tmp3+(-0.78539816f);
tmp209=cos(tmp208);
tmp210=tmp0+(-0.78539816f);
tmp211=sin(tmp210);
tmp212=tmp6+(-0.78539816f);
tmp213=sin(tmp212);
tmp214=tmp209*tmp211*tmp213;
tmp215=tmp207+tmp214;
tmp216=tmp193*tmp215;
tmp217=tmp191+0.6981317f;
tmp218=sin(tmp217);
tmp219=tmp6+(-0.78539816f);
tmp220=cos(tmp219);
tmp221=cos(tmp23);
tmp222=tmp220*tmp221;
tmp223=tmp6+(-0.78539816f);
tmp224=sin(tmp223);
tmp225=tmp3+(-0.78539816f);
tmp226=sin(tmp225);
tmp227=sin(tmp23);
tmp228=(-1.0f)*tmp224*tmp226*tmp227;
tmp229=tmp222+tmp228;
tmp230=tmp218*tmp229;
tmp231=tmp216+tmp230+downx;
residual4=tmp231;
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
} // ik_rightarmf()

void ik_rightarmdf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[12],
  const float ik_rightarmx[5],
  float ik_rightarmdy[25]
)
{
// global aliases
#define epsilon ik_rightarmglobals[0]
#define steps ik_rightarmglobals[1]

// parameter aliases
#define shoulderio ik_rightarmparameters[0]
#define shoulderud ik_rightarmparameters[1]
#define bicep ik_rightarmparameters[2]
#define elbow ik_rightarmparameters[3]
#define forearm ik_rightarmparameters[4]
#define downx ik_rightarmparameters[5]
#define pointx ik_rightarmparameters[6]
#define px ik_rightarmparameters[7]
#define py ik_rightarmparameters[8]
#define pz ik_rightarmparameters[9]
#define waist ik_rightarmparameters[10]
#define residual ik_rightarmparameters[11]

// variable aliases
#define _shoulderio ik_rightarmx[0]
#define _shoulderud ik_rightarmx[1]
#define _bicep ik_rightarmx[2]
#define _elbow ik_rightarmx[3]
#define _forearm ik_rightarmx[4]

// residual aliases
#define residual0 ik_rightarmy[0]
#define residual1 ik_rightarmy[1]
#define residual2 ik_rightarmy[2]
#define residual3 ik_rightarmy[3]
#define residual4 ik_rightarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_rightarmdy[0]
#define dresidual0_d_shoulderud ik_rightarmdy[5]
#define dresidual0_d_bicep ik_rightarmdy[10]
#define dresidual0_d_elbow ik_rightarmdy[15]
#define dresidual0_d_forearm ik_rightarmdy[20]
#define dresidual1_d_shoulderio ik_rightarmdy[1]
#define dresidual1_d_shoulderud ik_rightarmdy[6]
#define dresidual1_d_bicep ik_rightarmdy[11]
#define dresidual1_d_elbow ik_rightarmdy[16]
#define dresidual1_d_forearm ik_rightarmdy[21]
#define dresidual2_d_shoulderio ik_rightarmdy[2]
#define dresidual2_d_shoulderud ik_rightarmdy[7]
#define dresidual2_d_bicep ik_rightarmdy[12]
#define dresidual2_d_elbow ik_rightarmdy[17]
#define dresidual2_d_forearm ik_rightarmdy[22]
#define dresidual3_d_shoulderio ik_rightarmdy[3]
#define dresidual3_d_shoulderud ik_rightarmdy[8]
#define dresidual3_d_bicep ik_rightarmdy[13]
#define dresidual3_d_elbow ik_rightarmdy[18]
#define dresidual3_d_forearm ik_rightarmdy[23]
#define dresidual4_d_shoulderio ik_rightarmdy[4]
#define dresidual4_d_shoulderud ik_rightarmdy[9]
#define dresidual4_d_bicep ik_rightarmdy[14]
#define dresidual4_d_elbow ik_rightarmdy[19]
#define dresidual4_d_forearm ik_rightarmdy[24]

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
float tmp108;
float tmp109;
float tmp110;
float tmp111;
float tmp112;
float tmp113;
float tmp114;
float tmp115;
float tmp116;
float tmp117;
float tmp118;
float tmp119;
float tmp120;
float tmp121;
float tmp122;
float tmp123;
float tmp124;
float tmp125;
float tmp126;
float tmp127;
float tmp128;
float tmp129;
float tmp130;
float tmp131;
float tmp132;
float tmp133;
float tmp134;
float tmp135;
float tmp136;
float tmp137;
float tmp138;
float tmp139;
float tmp140;
float tmp141;
float tmp142;
float tmp143;
float tmp144;
float tmp145;
float tmp146;
float tmp147;
float tmp148;
float tmp149;
float tmp150;
float tmp151;
float tmp152;
float tmp153;
float tmp154;
float tmp155;
float tmp156;
float tmp157;
float tmp158;
float tmp159;
float tmp160;
float tmp161;
float tmp162;
float tmp163;
float tmp164;
float tmp165;
float tmp166;
float tmp167;
float tmp168;
float tmp169;
float tmp170;
float tmp171;
float tmp172;
float tmp173;
float tmp174;
float tmp175;
float tmp176;
float tmp177;
float tmp178;
float tmp179;
float tmp180;
float tmp181;
float tmp182;
float tmp183;
float tmp184;
float tmp185;
float tmp186;
float tmp187;
float tmp188;
float tmp189;
float tmp190;
float tmp191;
float tmp192;
float tmp193;
float tmp194;
float tmp195;
float tmp196;
float tmp197;
float tmp198;
float tmp199;
float tmp200;
float tmp201;
float tmp202;
float tmp203;
float tmp204;
float tmp205;
float tmp206;
float tmp207;
float tmp208;
float tmp209;
float tmp210;
float tmp211;
float tmp212;
float tmp213;
float tmp214;
float tmp215;
float tmp216;
float tmp217;
float tmp218;
float tmp219;
float tmp220;
float tmp221;
float tmp222;
float tmp223;
float tmp224;
float tmp225;
float tmp226;
float tmp227;
float tmp228;
float tmp229;
float tmp230;
float tmp231;
float tmp232;
float tmp233;
float tmp234;
float tmp235;
float tmp236;
float tmp237;
float tmp238;
float tmp239;
float tmp240;
float tmp241;
float tmp242;
float tmp243;
float tmp244;
float tmp245;
float tmp246;
float tmp247;
float tmp248;
float tmp249;
float tmp250;
float tmp251;
float tmp252;
float tmp253;
float tmp254;
float tmp255;
float tmp256;
float tmp257;
float tmp258;
float tmp259;
float tmp260;
float tmp261;
float tmp262;
float tmp263;
float tmp264;
float tmp265;
float tmp266;
float tmp267;
float tmp268;
float tmp269;
float tmp270;
float tmp271;
float tmp272;
float tmp273;
float tmp274;
float tmp275;
float tmp276;
float tmp277;
float tmp278;
float tmp279;
float tmp280;
float tmp281;
float tmp282;
float tmp283;
float tmp284;
float tmp285;
float tmp286;
float tmp287;
float tmp288;
float tmp289;
float tmp290;
float tmp291;
float tmp292;
float tmp293;
float tmp294;
float tmp295;
float tmp296;
float tmp297;
float tmp298;
float tmp299;
float tmp300;
float tmp301;
float tmp302;
float tmp303;
float tmp304;
float tmp305;
float tmp306;
float tmp307;
float tmp308;
float tmp309;
float tmp310;
float tmp311;
float tmp312;
float tmp313;
float tmp314;
float tmp315;
float tmp316;
float tmp317;
float tmp318;
float tmp319;
float tmp320;
float tmp321;
float tmp322;
float tmp323;
float tmp324;
float tmp325;
float tmp326;
float tmp327;
float tmp328;
float tmp329;
float tmp330;
float tmp331;
float tmp332;
float tmp333;
float tmp334;
float tmp335;
float tmp336;
float tmp337;
float tmp338;
float tmp339;
float tmp340;
float tmp341;
float tmp342;
float tmp343;
float tmp344;
float tmp345;
float tmp346;
float tmp347;
float tmp348;
float tmp349;
float tmp350;
float tmp351;
float tmp352;
float tmp353;
float tmp354;
float tmp355;
float tmp356;
float tmp357;
float tmp358;
float tmp359;
float tmp360;
float tmp361;
float tmp362;
float tmp363;
float tmp364;
float tmp365;
float tmp366;
float tmp367;
float tmp368;
float tmp369;
float tmp370;
float tmp371;
float tmp372;
float tmp373;
float tmp374;
float tmp375;
float tmp376;
float tmp377;
float tmp378;
float tmp379;
float tmp380;
float tmp381;
float tmp382;
float tmp383;
float tmp384;
float tmp385;
float tmp386;
float tmp387;
float tmp388;
float tmp389;
float tmp390;
float tmp391;
float tmp392;
float tmp393;
float tmp394;
float tmp395;
float tmp396;
float tmp397;
float tmp398;
float tmp399;
float tmp400;
float tmp401;
float tmp402;
float tmp403;
float tmp404;
float tmp405;
float tmp406;
float tmp407;
float tmp408;
float tmp409;
float tmp410;
float tmp411;
float tmp412;
float tmp413;
float tmp414;
float tmp415;
float tmp416;
float tmp417;
float tmp418;
float tmp419;
float tmp420;
float tmp421;
float tmp422;
float tmp423;
float tmp424;
float tmp425;
float tmp426;
float tmp427;
float tmp428;
float tmp429;
float tmp430;
float tmp431;
float tmp432;
float tmp433;
float tmp434;
float tmp435;
float tmp436;
float tmp437;
float tmp438;
float tmp439;
float tmp440;
float tmp441;
float tmp442;
float tmp443;
float tmp444;
float tmp445;
float tmp446;
float tmp447;
float tmp448;
float tmp449;
float tmp450;
float tmp451;
float tmp452;
float tmp453;
float tmp454;
float tmp455;
float tmp456;
float tmp457;
float tmp458;
float tmp459;
float tmp460;
float tmp461;
float tmp462;
float tmp463;
float tmp464;
float tmp465;
float tmp466;
float tmp467;
float tmp468;
float tmp469;
float tmp470;
float tmp471;
float tmp472;
float tmp473;
float tmp474;
float tmp475;
float tmp476;
float tmp477;
float tmp478;
float tmp479;
float tmp480;
float tmp481;
float tmp482;
float tmp483;
float tmp484;
float tmp485;
float tmp486;
float tmp487;
float tmp488;
float tmp489;
float tmp490;
float tmp491;
float tmp492;
float tmp493;
float tmp494;
float tmp495;
float tmp496;
float tmp497;
float tmp498;
float tmp499;
float tmp500;
float tmp501;
float tmp502;
float tmp503;
float tmp504;
float tmp505;
float tmp506;
float tmp507;
float tmp508;
float tmp509;
float tmp510;
float tmp511;
float tmp512;
float tmp513;
float tmp514;
float tmp515;
float tmp516;
float tmp517;
float tmp518;
float tmp519;
float tmp520;
float tmp521;
float tmp522;
float tmp523;
float tmp524;
float tmp525;
float tmp526;
float tmp527;
float tmp528;
float tmp529;
float tmp530;
float tmp531;
float tmp532;
float tmp533;
float tmp534;
float tmp535;
float tmp536;
float tmp537;
float tmp538;
float tmp539;
float tmp540;
float tmp541;
float tmp542;
float tmp543;
float tmp544;
float tmp545;
float tmp546;
float tmp547;
float tmp548;
float tmp549;
float tmp550;
float tmp551;
float tmp552;
float tmp553;
float tmp554;
float tmp555;
float tmp556;
float tmp557;
float tmp558;
float tmp559;
float tmp560;
float tmp561;
float tmp562;
float tmp563;
float tmp564;
float tmp565;
float tmp566;
float tmp567;
float tmp568;
float tmp569;
float tmp570;
float tmp571;
float tmp572;
float tmp573;
float tmp574;
float tmp575;
float tmp576;
float tmp577;
float tmp578;
float tmp579;
float tmp580;
float tmp581;
float tmp582;
float tmp583;
float tmp584;
float tmp585;
float tmp586;
float tmp587;
float tmp588;
float tmp589;
float tmp590;
float tmp591;
float tmp592;
float tmp593;
float tmp594;
float tmp595;
float tmp596;
float tmp597;
float tmp598;
float tmp599;
float tmp600;
float tmp601;
float tmp602;
float tmp603;
float tmp604;
float tmp605;
float tmp606;
float tmp607;
float tmp608;
float tmp609;
float tmp610;
float tmp611;
float tmp612;
float tmp613;
float tmp614;
float tmp615;
float tmp616;
float tmp617;
float tmp618;
float tmp619;
float tmp620;
float tmp621;
float tmp622;
float tmp623;
float tmp624;
float tmp625;
float tmp626;
float tmp627;
float tmp628;
float tmp629;
float tmp630;
float tmp631;
float tmp632;
float tmp633;
float tmp634;
float tmp635;
float tmp636;
tmp0=0.017453293f*_elbow;
tmp1=tmp0+(-0.78539816f);
tmp2=cos(tmp1);
tmp3=0.017453293f*_shoulderio;
tmp4=tmp3+(-0.78539816f);
tmp5=cos(tmp4);
tmp6=0.017453293f*_shoulderud;
tmp7=tmp6+(-0.78539816f);
tmp8=cos(tmp7);
tmp9=(-0.26179939f)*tmp2*tmp5*tmp8;
tmp10=tmp3+(-0.78539816f);
tmp11=cos(tmp10);
tmp12=tmp6+(-0.78539816f);
tmp13=cos(tmp12);
tmp14=(-0.15707963f)*tmp11*tmp13;
tmp15=tmp3+(-0.78539816f);
tmp16=cos(tmp15);
tmp17=(-0.063268185f)*tmp16;
tmp18=tmp0+(-0.78539816f);
tmp19=sin(tmp18);
tmp20=tmp3+(-0.78539816f);
tmp21=sin(tmp20);
tmp22=(-0.017453293f)*_bicep;
tmp23=sin(tmp22);
tmp24=(-0.017453293f)*tmp21*tmp23;
tmp25=tmp3+(-0.78539816f);
tmp26=cos(tmp25);
tmp27=cos(tmp22);
tmp28=tmp6+(-0.78539816f);
tmp29=sin(tmp28);
tmp30=0.017453293f*tmp26*tmp27*tmp29;
tmp31=tmp24+tmp30;
tmp32=15.0f*tmp19*tmp31;
tmp33=tmp9+tmp14+tmp17+tmp32;
dresidual0_d_shoulderio=tmp33;
tmp34=tmp3+(-0.78539816f);
tmp35=sin(tmp34);
tmp36=tmp6+(-0.78539816f);
tmp37=sin(tmp36);
tmp38=0.15707963f*tmp35*tmp37;
tmp39=tmp0+(-0.78539816f);
tmp40=cos(tmp39);
tmp41=tmp3+(-0.78539816f);
tmp42=sin(tmp41);
tmp43=tmp6+(-0.78539816f);
tmp44=sin(tmp43);
tmp45=0.26179939f*tmp40*tmp42*tmp44;
tmp46=tmp6+(-0.78539816f);
tmp47=cos(tmp46);
tmp48=cos(tmp22);
tmp49=tmp0+(-0.78539816f);
tmp50=sin(tmp49);
tmp51=tmp3+(-0.78539816f);
tmp52=sin(tmp51);
tmp53=0.26179939f*tmp47*tmp48*tmp50*tmp52;
tmp54=tmp38+tmp45+tmp53;
dresidual0_d_shoulderud=tmp54;
tmp55=tmp0+(-0.78539816f);
tmp56=sin(tmp55);
tmp57=tmp3+(-0.78539816f);
tmp58=cos(tmp57);
tmp59=cos(tmp22);
tmp60=(-0.017453293f)*tmp58*tmp59;
tmp61=tmp3+(-0.78539816f);
tmp62=sin(tmp61);
tmp63=tmp6+(-0.78539816f);
tmp64=sin(tmp63);
tmp65=sin(tmp22);
tmp66=0.017453293f*tmp62*tmp64*tmp65;
tmp67=tmp60+tmp66;
tmp68=15.0f*tmp56*tmp67;
dresidual0_d_bicep=tmp68;
tmp69=tmp0+(-0.78539816f);
tmp70=cos(tmp69);
tmp71=tmp3+(-0.78539816f);
tmp72=cos(tmp71);
tmp73=sin(tmp22);
tmp74=tmp72*tmp73;
tmp75=cos(tmp22);
tmp76=tmp3+(-0.78539816f);
tmp77=sin(tmp76);
tmp78=tmp6+(-0.78539816f);
tmp79=sin(tmp78);
tmp80=tmp75*tmp77*tmp79;
tmp81=tmp74+tmp80;
tmp82=0.26179939f*tmp70*tmp81;
tmp83=tmp6+(-0.78539816f);
tmp84=cos(tmp83);
tmp85=tmp0+(-0.78539816f);
tmp86=sin(tmp85);
tmp87=tmp3+(-0.78539816f);
tmp88=sin(tmp87);
tmp89=0.26179939f*tmp84*tmp86*tmp88;
tmp90=tmp82+tmp89;
dresidual0_d_elbow=tmp90;
dresidual0_d_forearm=0.0f;
tmp91=tmp0+(-0.78539816f);
tmp92=cos(tmp91);
tmp93=tmp6+(-0.78539816f);
tmp94=cos(tmp93);
tmp95=0.017453293f*waist;
tmp96=tmp95+0.087266463f;
tmp97=cos(tmp96);
tmp98=tmp3+(-0.78539816f);
tmp99=sin(tmp98);
tmp100=(-0.26179939f)*tmp92*tmp94*tmp97*tmp99;
tmp101=tmp6+(-0.78539816f);
tmp102=cos(tmp101);
tmp103=tmp95+0.087266463f;
tmp104=cos(tmp103);
tmp105=tmp3+(-0.78539816f);
tmp106=sin(tmp105);
tmp107=(-0.15707963f)*tmp102*tmp104*tmp106;
tmp108=tmp95+0.087266463f;
tmp109=cos(tmp108);
tmp110=tmp3+(-0.78539816f);
tmp111=sin(tmp110);
tmp112=(-0.063268185f)*tmp109*tmp111;
tmp113=tmp0+(-0.78539816f);
tmp114=sin(tmp113);
tmp115=tmp3+(-0.78539816f);
tmp116=cos(tmp115);
tmp117=tmp95+0.087266463f;
tmp118=cos(tmp117);
tmp119=sin(tmp22);
tmp120=0.017453293f*tmp116*tmp118*tmp119;
tmp121=tmp95+0.087266463f;
tmp122=cos(tmp121);
tmp123=cos(tmp22);
tmp124=tmp3+(-0.78539816f);
tmp125=sin(tmp124);
tmp126=tmp6+(-0.78539816f);
tmp127=sin(tmp126);
tmp128=0.017453293f*tmp122*tmp123*tmp125*tmp127;
tmp129=tmp120+tmp128;
tmp130=15.0f*tmp114*tmp129;
tmp131=tmp100+tmp107+tmp112+tmp130;
dresidual1_d_shoulderio=tmp131;
tmp132=tmp3+(-0.78539816f);
tmp133=cos(tmp132);
tmp134=tmp95+0.087266463f;
tmp135=cos(tmp134);
tmp136=tmp6+(-0.78539816f);
tmp137=sin(tmp136);
tmp138=(-0.15707963f)*tmp133*tmp135*tmp137;
tmp139=tmp6+(-0.78539816f);
tmp140=cos(tmp139);
tmp141=tmp95+0.087266463f;
tmp142=sin(tmp141);
tmp143=(-0.15707963f)*tmp140*tmp142;
tmp144=tmp0+(-0.78539816f);
tmp145=cos(tmp144);
tmp146=tmp3+(-0.78539816f);
tmp147=cos(tmp146);
tmp148=tmp95+0.087266463f;
tmp149=cos(tmp148);
tmp150=tmp6+(-0.78539816f);
tmp151=sin(tmp150);
tmp152=(-0.017453293f)*tmp147*tmp149*tmp151;
tmp153=tmp6+(-0.78539816f);
tmp154=cos(tmp153);
tmp155=tmp95+0.087266463f;
tmp156=sin(tmp155);
tmp157=(-0.017453293f)*tmp154*tmp156;
tmp158=tmp152+tmp157;
tmp159=15.0f*tmp145*tmp158;
tmp160=cos(tmp22);
tmp161=tmp0+(-0.78539816f);
tmp162=sin(tmp161);
tmp163=tmp3+(-0.78539816f);
tmp164=cos(tmp163);
tmp165=tmp6+(-0.78539816f);
tmp166=cos(tmp165);
tmp167=tmp95+0.087266463f;
tmp168=cos(tmp167);
tmp169=(-0.017453293f)*tmp164*tmp166*tmp168;
tmp170=tmp6+(-0.78539816f);
tmp171=sin(tmp170);
tmp172=tmp95+0.087266463f;
tmp173=sin(tmp172);
tmp174=0.017453293f*tmp171*tmp173;
tmp175=tmp169+tmp174;
tmp176=15.0f*tmp160*tmp162*tmp175;
tmp177=tmp138+tmp143+tmp159+tmp176;
dresidual1_d_shoulderud=tmp177;
tmp178=tmp0+(-0.78539816f);
tmp179=sin(tmp178);
tmp180=tmp95+0.087266463f;
tmp181=cos(tmp180);
tmp182=cos(tmp22);
tmp183=tmp3+(-0.78539816f);
tmp184=sin(tmp183);
tmp185=(-0.017453293f)*tmp181*tmp182*tmp184;
tmp186=sin(tmp22);
tmp187=tmp3+(-0.78539816f);
tmp188=cos(tmp187);
tmp189=tmp95+0.087266463f;
tmp190=cos(tmp189);
tmp191=tmp6+(-0.78539816f);
tmp192=sin(tmp191);
tmp193=(-1.0f)*tmp188*tmp190*tmp192;
tmp194=tmp6+(-0.78539816f);
tmp195=cos(tmp194);
tmp196=tmp95+0.087266463f;
tmp197=sin(tmp196);
tmp198=(-1.0f)*tmp195*tmp197;
tmp199=tmp193+tmp198;
tmp200=0.017453293f*tmp186*tmp199;
tmp201=tmp185+tmp200;
tmp202=15.0f*tmp179*tmp201;
dresidual1_d_bicep=tmp202;
tmp203=tmp0+(-0.78539816f);
tmp204=sin(tmp203);
tmp205=tmp3+(-0.78539816f);
tmp206=cos(tmp205);
tmp207=tmp6+(-0.78539816f);
tmp208=cos(tmp207);
tmp209=tmp95+0.087266463f;
tmp210=cos(tmp209);
tmp211=tmp206*tmp208*tmp210;
tmp212=tmp6+(-0.78539816f);
tmp213=sin(tmp212);
tmp214=tmp95+0.087266463f;
tmp215=sin(tmp214);
tmp216=(-1.0f)*tmp213*tmp215;
tmp217=tmp211+tmp216;
tmp218=(-0.26179939f)*tmp204*tmp217;
tmp219=tmp0+(-0.78539816f);
tmp220=cos(tmp219);
tmp221=tmp95+0.087266463f;
tmp222=cos(tmp221);
tmp223=tmp3+(-0.78539816f);
tmp224=sin(tmp223);
tmp225=sin(tmp22);
tmp226=tmp222*tmp224*tmp225;
tmp227=cos(tmp22);
tmp228=tmp3+(-0.78539816f);
tmp229=cos(tmp228);
tmp230=tmp95+0.087266463f;
tmp231=cos(tmp230);
tmp232=tmp6+(-0.78539816f);
tmp233=sin(tmp232);
tmp234=(-1.0f)*tmp229*tmp231*tmp233;
tmp235=tmp6+(-0.78539816f);
tmp236=cos(tmp235);
tmp237=tmp95+0.087266463f;
tmp238=sin(tmp237);
tmp239=(-1.0f)*tmp236*tmp238;
tmp240=tmp234+tmp239;
tmp241=tmp227*tmp240;
tmp242=tmp226+tmp241;
tmp243=0.26179939f*tmp220*tmp242;
tmp244=tmp218+tmp243;
dresidual1_d_elbow=tmp244;
dresidual1_d_forearm=0.0f;
tmp245=tmp0+(-0.78539816f);
tmp246=cos(tmp245);
tmp247=tmp6+(-0.78539816f);
tmp248=cos(tmp247);
tmp249=tmp3+(-0.78539816f);
tmp250=sin(tmp249);
tmp251=tmp95+0.087266463f;
tmp252=sin(tmp251);
tmp253=(-0.26179939f)*tmp246*tmp248*tmp250*tmp252;
tmp254=tmp6+(-0.78539816f);
tmp255=cos(tmp254);
tmp256=tmp3+(-0.78539816f);
tmp257=sin(tmp256);
tmp258=tmp95+0.087266463f;
tmp259=sin(tmp258);
tmp260=(-0.15707963f)*tmp255*tmp257*tmp259;
tmp261=tmp3+(-0.78539816f);
tmp262=sin(tmp261);
tmp263=tmp95+0.087266463f;
tmp264=sin(tmp263);
tmp265=(-0.063268185f)*tmp262*tmp264;
tmp266=tmp0+(-0.78539816f);
tmp267=sin(tmp266);
tmp268=tmp3+(-0.78539816f);
tmp269=cos(tmp268);
tmp270=tmp95+0.087266463f;
tmp271=sin(tmp270);
tmp272=sin(tmp22);
tmp273=0.017453293f*tmp269*tmp271*tmp272;
tmp274=cos(tmp22);
tmp275=tmp3+(-0.78539816f);
tmp276=sin(tmp275);
tmp277=tmp6+(-0.78539816f);
tmp278=sin(tmp277);
tmp279=tmp95+0.087266463f;
tmp280=sin(tmp279);
tmp281=0.017453293f*tmp274*tmp276*tmp278*tmp280;
tmp282=tmp273+tmp281;
tmp283=15.0f*tmp267*tmp282;
tmp284=tmp253+tmp260+tmp265+tmp283;
dresidual2_d_shoulderio=tmp284;
tmp285=tmp3+(-0.78539816f);
tmp286=cos(tmp285);
tmp287=tmp6+(-0.78539816f);
tmp288=sin(tmp287);
tmp289=tmp95+0.087266463f;
tmp290=sin(tmp289);
tmp291=(-0.15707963f)*tmp286*tmp288*tmp290;
tmp292=tmp6+(-0.78539816f);
tmp293=cos(tmp292);
tmp294=tmp95+0.087266463f;
tmp295=cos(tmp294);
tmp296=0.15707963f*tmp293*tmp295;
tmp297=tmp0+(-0.78539816f);
tmp298=cos(tmp297);
tmp299=tmp3+(-0.78539816f);
tmp300=cos(tmp299);
tmp301=tmp6+(-0.78539816f);
tmp302=sin(tmp301);
tmp303=tmp95+0.087266463f;
tmp304=sin(tmp303);
tmp305=(-0.017453293f)*tmp300*tmp302*tmp304;
tmp306=tmp6+(-0.78539816f);
tmp307=cos(tmp306);
tmp308=tmp95+0.087266463f;
tmp309=cos(tmp308);
tmp310=0.017453293f*tmp307*tmp309;
tmp311=tmp305+tmp310;
tmp312=15.0f*tmp298*tmp311;
tmp313=cos(tmp22);
tmp314=tmp0+(-0.78539816f);
tmp315=sin(tmp314);
tmp316=tmp3+(-0.78539816f);
tmp317=cos(tmp316);
tmp318=tmp6+(-0.78539816f);
tmp319=cos(tmp318);
tmp320=tmp95+0.087266463f;
tmp321=sin(tmp320);
tmp322=(-0.017453293f)*tmp317*tmp319*tmp321;
tmp323=tmp95+0.087266463f;
tmp324=cos(tmp323);
tmp325=tmp6+(-0.78539816f);
tmp326=sin(tmp325);
tmp327=(-0.017453293f)*tmp324*tmp326;
tmp328=tmp322+tmp327;
tmp329=15.0f*tmp313*tmp315*tmp328;
tmp330=tmp291+tmp296+tmp312+tmp329;
dresidual2_d_shoulderud=tmp330;
tmp331=tmp0+(-0.78539816f);
tmp332=sin(tmp331);
tmp333=cos(tmp22);
tmp334=tmp3+(-0.78539816f);
tmp335=sin(tmp334);
tmp336=tmp95+0.087266463f;
tmp337=sin(tmp336);
tmp338=(-0.017453293f)*tmp333*tmp335*tmp337;
tmp339=sin(tmp22);
tmp340=tmp6+(-0.78539816f);
tmp341=cos(tmp340);
tmp342=tmp95+0.087266463f;
tmp343=cos(tmp342);
tmp344=tmp341*tmp343;
tmp345=tmp3+(-0.78539816f);
tmp346=cos(tmp345);
tmp347=tmp6+(-0.78539816f);
tmp348=sin(tmp347);
tmp349=tmp95+0.087266463f;
tmp350=sin(tmp349);
tmp351=(-1.0f)*tmp346*tmp348*tmp350;
tmp352=tmp344+tmp351;
tmp353=0.017453293f*tmp339*tmp352;
tmp354=tmp338+tmp353;
tmp355=15.0f*tmp332*tmp354;
dresidual2_d_bicep=tmp355;
tmp356=tmp0+(-0.78539816f);
tmp357=sin(tmp356);
tmp358=tmp3+(-0.78539816f);
tmp359=cos(tmp358);
tmp360=tmp6+(-0.78539816f);
tmp361=cos(tmp360);
tmp362=tmp95+0.087266463f;
tmp363=sin(tmp362);
tmp364=tmp359*tmp361*tmp363;
tmp365=tmp95+0.087266463f;
tmp366=cos(tmp365);
tmp367=tmp6+(-0.78539816f);
tmp368=sin(tmp367);
tmp369=tmp366*tmp368;
tmp370=tmp364+tmp369;
tmp371=(-0.26179939f)*tmp357*tmp370;
tmp372=tmp0+(-0.78539816f);
tmp373=cos(tmp372);
tmp374=cos(tmp22);
tmp375=tmp6+(-0.78539816f);
tmp376=cos(tmp375);
tmp377=tmp95+0.087266463f;
tmp378=cos(tmp377);
tmp379=tmp376*tmp378;
tmp380=tmp3+(-0.78539816f);
tmp381=cos(tmp380);
tmp382=tmp6+(-0.78539816f);
tmp383=sin(tmp382);
tmp384=tmp95+0.087266463f;
tmp385=sin(tmp384);
tmp386=(-1.0f)*tmp381*tmp383*tmp385;
tmp387=tmp379+tmp386;
tmp388=tmp374*tmp387;
tmp389=tmp3+(-0.78539816f);
tmp390=sin(tmp389);
tmp391=tmp95+0.087266463f;
tmp392=sin(tmp391);
tmp393=sin(tmp22);
tmp394=tmp390*tmp392*tmp393;
tmp395=tmp388+tmp394;
tmp396=0.26179939f*tmp373*tmp395;
tmp397=tmp371+tmp396;
dresidual2_d_elbow=tmp397;
dresidual2_d_forearm=0.0f;
tmp398=tmp0+(-0.78539816f);
tmp399=sin(tmp398);
tmp400=tmp3+(-0.78539816f);
tmp401=sin(tmp400);
tmp402=sin(tmp22);
tmp403=(-0.017453293f)*tmp401*tmp402;
tmp404=tmp3+(-0.78539816f);
tmp405=cos(tmp404);
tmp406=cos(tmp22);
tmp407=tmp6+(-0.78539816f);
tmp408=sin(tmp407);
tmp409=0.017453293f*tmp405*tmp406*tmp408;
tmp410=tmp403+tmp409;
tmp411=tmp399*tmp410;
tmp412=tmp0+(-0.78539816f);
tmp413=cos(tmp412);
tmp414=tmp3+(-0.78539816f);
tmp415=cos(tmp414);
tmp416=tmp6+(-0.78539816f);
tmp417=cos(tmp416);
tmp418=(-0.017453293f)*tmp413*tmp415*tmp417;
tmp419=tmp411+tmp418;
dresidual3_d_shoulderio=tmp419;
tmp420=tmp0+(-0.78539816f);
tmp421=cos(tmp420);
tmp422=tmp3+(-0.78539816f);
tmp423=sin(tmp422);
tmp424=tmp6+(-0.78539816f);
tmp425=sin(tmp424);
tmp426=0.017453293f*tmp421*tmp423*tmp425;
tmp427=tmp6+(-0.78539816f);
tmp428=cos(tmp427);
tmp429=cos(tmp22);
tmp430=tmp0+(-0.78539816f);
tmp431=sin(tmp430);
tmp432=tmp3+(-0.78539816f);
tmp433=sin(tmp432);
tmp434=0.017453293f*tmp428*tmp429*tmp431*tmp433;
tmp435=tmp426+tmp434;
dresidual3_d_shoulderud=tmp435;
tmp436=tmp0+(-0.78539816f);
tmp437=sin(tmp436);
tmp438=tmp3+(-0.78539816f);
tmp439=cos(tmp438);
tmp440=cos(tmp22);
tmp441=(-0.017453293f)*tmp439*tmp440;
tmp442=tmp3+(-0.78539816f);
tmp443=sin(tmp442);
tmp444=tmp6+(-0.78539816f);
tmp445=sin(tmp444);
tmp446=sin(tmp22);
tmp447=0.017453293f*tmp443*tmp445*tmp446;
tmp448=tmp441+tmp447;
tmp449=tmp437*tmp448;
dresidual3_d_bicep=tmp449;
tmp450=tmp0+(-0.78539816f);
tmp451=cos(tmp450);
tmp452=tmp3+(-0.78539816f);
tmp453=cos(tmp452);
tmp454=sin(tmp22);
tmp455=tmp453*tmp454;
tmp456=cos(tmp22);
tmp457=tmp3+(-0.78539816f);
tmp458=sin(tmp457);
tmp459=tmp6+(-0.78539816f);
tmp460=sin(tmp459);
tmp461=tmp456*tmp458*tmp460;
tmp462=tmp455+tmp461;
tmp463=0.017453293f*tmp451*tmp462;
tmp464=tmp6+(-0.78539816f);
tmp465=cos(tmp464);
tmp466=tmp0+(-0.78539816f);
tmp467=sin(tmp466);
tmp468=tmp3+(-0.78539816f);
tmp469=sin(tmp468);
tmp470=0.017453293f*tmp465*tmp467*tmp469;
tmp471=tmp463+tmp470;
dresidual3_d_elbow=tmp471;
dresidual3_d_forearm=0.0f;
tmp472=(-0.017453293f)*_forearm;
tmp473=tmp472+0.6981317f;
tmp474=cos(tmp473);
tmp475=tmp0+(-0.78539816f);
tmp476=cos(tmp475);
tmp477=tmp3+(-0.78539816f);
tmp478=sin(tmp477);
tmp479=sin(tmp22);
tmp480=(-0.017453293f)*tmp478*tmp479;
tmp481=tmp3+(-0.78539816f);
tmp482=cos(tmp481);
tmp483=cos(tmp22);
tmp484=tmp6+(-0.78539816f);
tmp485=sin(tmp484);
tmp486=0.017453293f*tmp482*tmp483*tmp485;
tmp487=tmp480+tmp486;
tmp488=tmp476*tmp487;
tmp489=tmp3+(-0.78539816f);
tmp490=cos(tmp489);
tmp491=tmp6+(-0.78539816f);
tmp492=cos(tmp491);
tmp493=tmp0+(-0.78539816f);
tmp494=sin(tmp493);
tmp495=0.017453293f*tmp490*tmp492*tmp494;
tmp496=tmp488+tmp495;
tmp497=tmp474*tmp496;
tmp498=tmp472+0.6981317f;
tmp499=sin(tmp498);
tmp500=tmp3+(-0.78539816f);
tmp501=cos(tmp500);
tmp502=tmp6+(-0.78539816f);
tmp503=sin(tmp502);
tmp504=sin(tmp22);
tmp505=(-0.017453293f)*tmp501*tmp503*tmp504;
tmp506=cos(tmp22);
tmp507=tmp3+(-0.78539816f);
tmp508=sin(tmp507);
tmp509=(-0.017453293f)*tmp506*tmp508;
tmp510=tmp505+tmp509;
tmp511=tmp499*tmp510;
tmp512=tmp497+tmp511;
dresidual4_d_shoulderio=tmp512;
tmp513=tmp472+0.6981317f;
tmp514=cos(tmp513);
tmp515=tmp0+(-0.78539816f);
tmp516=sin(tmp515);
tmp517=tmp3+(-0.78539816f);
tmp518=sin(tmp517);
tmp519=tmp6+(-0.78539816f);
tmp520=sin(tmp519);
tmp521=(-0.017453293f)*tmp516*tmp518*tmp520;
tmp522=tmp0+(-0.78539816f);
tmp523=cos(tmp522);
tmp524=tmp6+(-0.78539816f);
tmp525=cos(tmp524);
tmp526=cos(tmp22);
tmp527=tmp3+(-0.78539816f);
tmp528=sin(tmp527);
tmp529=0.017453293f*tmp523*tmp525*tmp526*tmp528;
tmp530=tmp521+tmp529;
tmp531=tmp514*tmp530;
tmp532=tmp6+(-0.78539816f);
tmp533=cos(tmp532);
tmp534=tmp472+0.6981317f;
tmp535=sin(tmp534);
tmp536=tmp3+(-0.78539816f);
tmp537=sin(tmp536);
tmp538=sin(tmp22);
tmp539=(-0.017453293f)*tmp533*tmp535*tmp537*tmp538;
tmp540=tmp531+tmp539;
dresidual4_d_shoulderud=tmp540;
tmp541=tmp472+0.6981317f;
tmp542=cos(tmp541);
tmp543=tmp0+(-0.78539816f);
tmp544=cos(tmp543);
tmp545=tmp3+(-0.78539816f);
tmp546=cos(tmp545);
tmp547=cos(tmp22);
tmp548=(-0.017453293f)*tmp546*tmp547;
tmp549=tmp3+(-0.78539816f);
tmp550=sin(tmp549);
tmp551=tmp6+(-0.78539816f);
tmp552=sin(tmp551);
tmp553=sin(tmp22);
tmp554=0.017453293f*tmp550*tmp552*tmp553;
tmp555=tmp548+tmp554;
tmp556=tmp542*tmp544*tmp555;
tmp557=tmp472+0.6981317f;
tmp558=sin(tmp557);
tmp559=tmp3+(-0.78539816f);
tmp560=cos(tmp559);
tmp561=sin(tmp22);
tmp562=0.017453293f*tmp560*tmp561;
tmp563=cos(tmp22);
tmp564=tmp3+(-0.78539816f);
tmp565=sin(tmp564);
tmp566=tmp6+(-0.78539816f);
tmp567=sin(tmp566);
tmp568=0.017453293f*tmp563*tmp565*tmp567;
tmp569=tmp562+tmp568;
tmp570=tmp558*tmp569;
tmp571=tmp556+tmp570;
dresidual4_d_bicep=tmp571;
tmp572=tmp472+0.6981317f;
tmp573=cos(tmp572);
tmp574=tmp0+(-0.78539816f);
tmp575=sin(tmp574);
tmp576=tmp3+(-0.78539816f);
tmp577=cos(tmp576);
tmp578=sin(tmp22);
tmp579=tmp577*tmp578;
tmp580=cos(tmp22);
tmp581=tmp3+(-0.78539816f);
tmp582=sin(tmp581);
tmp583=tmp6+(-0.78539816f);
tmp584=sin(tmp583);
tmp585=tmp580*tmp582*tmp584;
tmp586=tmp579+tmp585;
tmp587=(-0.017453293f)*tmp575*tmp586;
tmp588=tmp0+(-0.78539816f);
tmp589=cos(tmp588);
tmp590=tmp6+(-0.78539816f);
tmp591=cos(tmp590);
tmp592=tmp3+(-0.78539816f);
tmp593=sin(tmp592);
tmp594=0.017453293f*tmp589*tmp591*tmp593;
tmp595=tmp587+tmp594;
tmp596=tmp573*tmp595;
dresidual4_d_elbow=tmp596;
tmp597=tmp472+0.6981317f;
tmp598=cos(tmp597);
tmp599=tmp3+(-0.78539816f);
tmp600=cos(tmp599);
tmp601=cos(tmp22);
tmp602=tmp600*tmp601;
tmp603=tmp3+(-0.78539816f);
tmp604=sin(tmp603);
tmp605=tmp6+(-0.78539816f);
tmp606=sin(tmp605);
tmp607=sin(tmp22);
tmp608=(-1.0f)*tmp604*tmp606*tmp607;
tmp609=tmp602+tmp608;
tmp610=(-0.017453293f)*tmp598*tmp609;
tmp611=tmp472+0.6981317f;
tmp612=sin(tmp611);
tmp613=tmp0+(-0.78539816f);
tmp614=cos(tmp613);
tmp615=tmp3+(-0.78539816f);
tmp616=cos(tmp615);
tmp617=sin(tmp22);
tmp618=tmp616*tmp617;
tmp619=cos(tmp22);
tmp620=tmp3+(-0.78539816f);
tmp621=sin(tmp620);
tmp622=tmp6+(-0.78539816f);
tmp623=sin(tmp622);
tmp624=tmp619*tmp621*tmp623;
tmp625=tmp618+tmp624;
tmp626=tmp614*tmp625;
tmp627=tmp6+(-0.78539816f);
tmp628=cos(tmp627);
tmp629=tmp0+(-0.78539816f);
tmp630=sin(tmp629);
tmp631=tmp3+(-0.78539816f);
tmp632=sin(tmp631);
tmp633=tmp628*tmp630*tmp632;
tmp634=tmp626+tmp633;
tmp635=0.017453293f*tmp612*tmp634;
tmp636=tmp610+tmp635;
dresidual4_d_forearm=tmp636;
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
} // ik_rightarmdf()
