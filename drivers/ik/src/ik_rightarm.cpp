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
tmp6=(-0.017453293f)*_shoulderio;
tmp7=tmp6+0.78539816f;
tmp8=sin(tmp7);
tmp9=(-15.0f)*tmp2*tmp5*tmp8;
tmp10=tmp3+(-0.78539816f);
tmp11=cos(tmp10);
tmp12=tmp6+0.78539816f;
tmp13=sin(tmp12);
tmp14=(-9.0f)*tmp11*tmp13;
tmp15=tmp6+0.78539816f;
tmp16=sin(tmp15);
tmp17=(-3.625f)*tmp16;
tmp18=(-1.0f)*px;
tmp19=tmp0+(-0.78539816f);
tmp20=sin(tmp19);
tmp21=tmp6+0.78539816f;
tmp22=cos(tmp21);
tmp23=(-0.017453293f)*_bicep;
tmp24=sin(tmp23);
tmp25=tmp22*tmp24;
tmp26=cos(tmp23);
tmp27=tmp6+0.78539816f;
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
tmp48=tmp6+0.78539816f;
tmp49=cos(tmp48);
tmp50=tmp35+0.087266463f;
tmp51=cos(tmp50);
tmp52=3.625f*tmp49*tmp51;
tmp53=tmp6+0.78539816f;
tmp54=cos(tmp53);
tmp55=tmp3+(-0.78539816f);
tmp56=cos(tmp55);
tmp57=tmp35+0.087266463f;
tmp58=cos(tmp57);
tmp59=9.0f*tmp54*tmp56*tmp58;
tmp60=tmp0+(-0.78539816f);
tmp61=cos(tmp60);
tmp62=tmp6+0.78539816f;
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
tmp80=tmp6+0.78539816f;
tmp81=sin(tmp80);
tmp82=sin(tmp23);
tmp83=tmp79*tmp81*tmp82;
tmp84=cos(tmp23);
tmp85=tmp6+0.78539816f;
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
tmp106=tmp6+0.78539816f;
tmp107=cos(tmp106);
tmp108=tmp35+0.087266463f;
tmp109=sin(tmp108);
tmp110=3.625f*tmp107*tmp109;
tmp111=tmp6+0.78539816f;
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
tmp128=tmp6+0.78539816f;
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
tmp150=tmp6+0.78539816f;
tmp151=cos(tmp150);
tmp152=tmp3+(-0.78539816f);
tmp153=sin(tmp152);
tmp154=tmp35+0.087266463f;
tmp155=sin(tmp154);
tmp156=(-1.0f)*tmp151*tmp153*tmp155;
tmp157=tmp149+tmp156;
tmp158=tmp144*tmp157;
tmp159=tmp6+0.78539816f;
tmp160=sin(tmp159);
tmp161=tmp35+0.087266463f;
tmp162=sin(tmp161);
tmp163=sin(tmp23);
tmp164=tmp160*tmp162*tmp163;
tmp165=tmp158+tmp164;
tmp166=15.0f*tmp143*tmp165;
tmp167=tmp102+tmp105+tmp110+tmp117+tmp122+tmp125+tmp141+tmp166+12.0f;
residual2=tmp167;
tmp168=(-0.017453293f)*_forearm;
tmp169=tmp168+(-0.6981317f);
tmp170=cos(tmp169);
tmp171=tmp0+(-0.78539816f);
tmp172=cos(tmp171);
tmp173=tmp6+0.78539816f;
tmp174=cos(tmp173);
tmp175=sin(tmp23);
tmp176=tmp174*tmp175;
tmp177=cos(tmp23);
tmp178=tmp6+0.78539816f;
tmp179=sin(tmp178);
tmp180=tmp3+(-0.78539816f);
tmp181=sin(tmp180);
tmp182=tmp177*tmp179*tmp181;
tmp183=tmp176+tmp182;
tmp184=tmp172*tmp183;
tmp185=tmp3+(-0.78539816f);
tmp186=cos(tmp185);
tmp187=tmp6+0.78539816f;
tmp188=sin(tmp187);
tmp189=tmp0+(-0.78539816f);
tmp190=sin(tmp189);
tmp191=tmp186*tmp188*tmp190;
tmp192=tmp184+tmp191;
tmp193=tmp170*tmp192;
tmp194=tmp168+(-0.6981317f);
tmp195=sin(tmp194);
tmp196=tmp6+0.78539816f;
tmp197=cos(tmp196);
tmp198=cos(tmp23);
tmp199=tmp197*tmp198;
tmp200=tmp6+0.78539816f;
tmp201=sin(tmp200);
tmp202=tmp3+(-0.78539816f);
tmp203=sin(tmp202);
tmp204=sin(tmp23);
tmp205=(-1.0f)*tmp201*tmp203*tmp204;
tmp206=tmp199+tmp205;
tmp207=tmp195*tmp206;
tmp208=(-1.0f)*pointx;
tmp209=tmp193+tmp207+tmp208;
residual3=tmp209;
tmp210=tmp0+(-0.78539816f);
tmp211=sin(tmp210);
tmp212=tmp6+0.78539816f;
tmp213=cos(tmp212);
tmp214=sin(tmp23);
tmp215=tmp213*tmp214;
tmp216=cos(tmp23);
tmp217=tmp6+0.78539816f;
tmp218=sin(tmp217);
tmp219=tmp3+(-0.78539816f);
tmp220=sin(tmp219);
tmp221=tmp216*tmp218*tmp220;
tmp222=tmp215+tmp221;
tmp223=tmp211*tmp222;
tmp224=tmp0+(-0.78539816f);
tmp225=cos(tmp224);
tmp226=tmp3+(-0.78539816f);
tmp227=cos(tmp226);
tmp228=tmp6+0.78539816f;
tmp229=sin(tmp228);
tmp230=(-1.0f)*tmp225*tmp227*tmp229;
tmp231=tmp223+tmp230+downx;
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
tmp10=tmp0+0.78539816f;
tmp11=cos(tmp10);
tmp12=0.017453293f*_elbow;
tmp13=tmp12+(-0.78539816f);
tmp14=cos(tmp13);
tmp15=tmp6+(-0.78539816f);
tmp16=cos(tmp15);
tmp17=0.26179939f*tmp11*tmp14*tmp16;
tmp18=tmp12+(-0.78539816f);
tmp19=sin(tmp18);
tmp20=tmp0+0.78539816f;
tmp21=cos(tmp20);
tmp22=(-0.017453293f)*_bicep;
tmp23=cos(tmp22);
tmp24=tmp6+(-0.78539816f);
tmp25=sin(tmp24);
tmp26=(-0.017453293f)*tmp21*tmp23*tmp25;
tmp27=tmp0+0.78539816f;
tmp28=sin(tmp27);
tmp29=sin(tmp22);
tmp30=0.017453293f*tmp28*tmp29;
tmp31=tmp26+tmp30;
tmp32=15.0f*tmp19*tmp31;
tmp33=tmp3+tmp9+tmp17+tmp32;
dresidual0_d_shoulderio=tmp33;
tmp34=tmp0+0.78539816f;
tmp35=sin(tmp34);
tmp36=tmp6+(-0.78539816f);
tmp37=sin(tmp36);
tmp38=0.15707963f*tmp35*tmp37;
tmp39=tmp12+(-0.78539816f);
tmp40=cos(tmp39);
tmp41=tmp0+0.78539816f;
tmp42=sin(tmp41);
tmp43=tmp6+(-0.78539816f);
tmp44=sin(tmp43);
tmp45=0.26179939f*tmp40*tmp42*tmp44;
tmp46=tmp6+(-0.78539816f);
tmp47=cos(tmp46);
tmp48=cos(tmp22);
tmp49=tmp0+0.78539816f;
tmp50=sin(tmp49);
tmp51=tmp12+(-0.78539816f);
tmp52=sin(tmp51);
tmp53=0.26179939f*tmp47*tmp48*tmp50*tmp52;
tmp54=tmp38+tmp45+tmp53;
dresidual0_d_shoulderud=tmp54;
tmp55=tmp12+(-0.78539816f);
tmp56=sin(tmp55);
tmp57=tmp0+0.78539816f;
tmp58=cos(tmp57);
tmp59=cos(tmp22);
tmp60=(-0.017453293f)*tmp58*tmp59;
tmp61=tmp0+0.78539816f;
tmp62=sin(tmp61);
tmp63=tmp6+(-0.78539816f);
tmp64=sin(tmp63);
tmp65=sin(tmp22);
tmp66=0.017453293f*tmp62*tmp64*tmp65;
tmp67=tmp60+tmp66;
tmp68=15.0f*tmp56*tmp67;
dresidual0_d_bicep=tmp68;
tmp69=tmp12+(-0.78539816f);
tmp70=cos(tmp69);
tmp71=tmp0+0.78539816f;
tmp72=cos(tmp71);
tmp73=sin(tmp22);
tmp74=tmp72*tmp73;
tmp75=cos(tmp22);
tmp76=tmp0+0.78539816f;
tmp77=sin(tmp76);
tmp78=tmp6+(-0.78539816f);
tmp79=sin(tmp78);
tmp80=tmp75*tmp77*tmp79;
tmp81=tmp74+tmp80;
tmp82=0.26179939f*tmp70*tmp81;
tmp83=tmp6+(-0.78539816f);
tmp84=cos(tmp83);
tmp85=tmp0+0.78539816f;
tmp86=sin(tmp85);
tmp87=tmp12+(-0.78539816f);
tmp88=sin(tmp87);
tmp89=0.26179939f*tmp84*tmp86*tmp88;
tmp90=tmp82+tmp89;
dresidual0_d_elbow=tmp90;
dresidual0_d_forearm=0.0f;
tmp91=0.017453293f*waist;
tmp92=tmp91+0.087266463f;
tmp93=cos(tmp92);
tmp94=tmp0+0.78539816f;
tmp95=sin(tmp94);
tmp96=0.063268185f*tmp93*tmp95;
tmp97=tmp6+(-0.78539816f);
tmp98=cos(tmp97);
tmp99=tmp91+0.087266463f;
tmp100=cos(tmp99);
tmp101=tmp0+0.78539816f;
tmp102=sin(tmp101);
tmp103=0.15707963f*tmp98*tmp100*tmp102;
tmp104=tmp12+(-0.78539816f);
tmp105=cos(tmp104);
tmp106=tmp6+(-0.78539816f);
tmp107=cos(tmp106);
tmp108=tmp91+0.087266463f;
tmp109=cos(tmp108);
tmp110=tmp0+0.78539816f;
tmp111=sin(tmp110);
tmp112=0.26179939f*tmp105*tmp107*tmp109*tmp111;
tmp113=tmp12+(-0.78539816f);
tmp114=sin(tmp113);
tmp115=tmp0+0.78539816f;
tmp116=cos(tmp115);
tmp117=tmp91+0.087266463f;
tmp118=cos(tmp117);
tmp119=sin(tmp22);
tmp120=(-0.017453293f)*tmp116*tmp118*tmp119;
tmp121=tmp91+0.087266463f;
tmp122=cos(tmp121);
tmp123=cos(tmp22);
tmp124=tmp0+0.78539816f;
tmp125=sin(tmp124);
tmp126=tmp6+(-0.78539816f);
tmp127=sin(tmp126);
tmp128=(-0.017453293f)*tmp122*tmp123*tmp125*tmp127;
tmp129=tmp120+tmp128;
tmp130=15.0f*tmp114*tmp129;
tmp131=tmp96+tmp103+tmp112+tmp130;
dresidual1_d_shoulderio=tmp131;
tmp132=tmp0+0.78539816f;
tmp133=cos(tmp132);
tmp134=tmp91+0.087266463f;
tmp135=cos(tmp134);
tmp136=tmp6+(-0.78539816f);
tmp137=sin(tmp136);
tmp138=(-0.15707963f)*tmp133*tmp135*tmp137;
tmp139=tmp6+(-0.78539816f);
tmp140=cos(tmp139);
tmp141=tmp91+0.087266463f;
tmp142=sin(tmp141);
tmp143=(-0.15707963f)*tmp140*tmp142;
tmp144=tmp12+(-0.78539816f);
tmp145=cos(tmp144);
tmp146=tmp0+0.78539816f;
tmp147=cos(tmp146);
tmp148=tmp91+0.087266463f;
tmp149=cos(tmp148);
tmp150=tmp6+(-0.78539816f);
tmp151=sin(tmp150);
tmp152=(-0.017453293f)*tmp147*tmp149*tmp151;
tmp153=tmp6+(-0.78539816f);
tmp154=cos(tmp153);
tmp155=tmp91+0.087266463f;
tmp156=sin(tmp155);
tmp157=(-0.017453293f)*tmp154*tmp156;
tmp158=tmp152+tmp157;
tmp159=15.0f*tmp145*tmp158;
tmp160=cos(tmp22);
tmp161=tmp12+(-0.78539816f);
tmp162=sin(tmp161);
tmp163=tmp0+0.78539816f;
tmp164=cos(tmp163);
tmp165=tmp6+(-0.78539816f);
tmp166=cos(tmp165);
tmp167=tmp91+0.087266463f;
tmp168=cos(tmp167);
tmp169=(-0.017453293f)*tmp164*tmp166*tmp168;
tmp170=tmp6+(-0.78539816f);
tmp171=sin(tmp170);
tmp172=tmp91+0.087266463f;
tmp173=sin(tmp172);
tmp174=0.017453293f*tmp171*tmp173;
tmp175=tmp169+tmp174;
tmp176=15.0f*tmp160*tmp162*tmp175;
tmp177=tmp138+tmp143+tmp159+tmp176;
dresidual1_d_shoulderud=tmp177;
tmp178=tmp12+(-0.78539816f);
tmp179=sin(tmp178);
tmp180=tmp91+0.087266463f;
tmp181=cos(tmp180);
tmp182=cos(tmp22);
tmp183=tmp0+0.78539816f;
tmp184=sin(tmp183);
tmp185=(-0.017453293f)*tmp181*tmp182*tmp184;
tmp186=sin(tmp22);
tmp187=tmp0+0.78539816f;
tmp188=cos(tmp187);
tmp189=tmp91+0.087266463f;
tmp190=cos(tmp189);
tmp191=tmp6+(-0.78539816f);
tmp192=sin(tmp191);
tmp193=(-1.0f)*tmp188*tmp190*tmp192;
tmp194=tmp6+(-0.78539816f);
tmp195=cos(tmp194);
tmp196=tmp91+0.087266463f;
tmp197=sin(tmp196);
tmp198=(-1.0f)*tmp195*tmp197;
tmp199=tmp193+tmp198;
tmp200=0.017453293f*tmp186*tmp199;
tmp201=tmp185+tmp200;
tmp202=15.0f*tmp179*tmp201;
dresidual1_d_bicep=tmp202;
tmp203=tmp12+(-0.78539816f);
tmp204=sin(tmp203);
tmp205=tmp0+0.78539816f;
tmp206=cos(tmp205);
tmp207=tmp6+(-0.78539816f);
tmp208=cos(tmp207);
tmp209=tmp91+0.087266463f;
tmp210=cos(tmp209);
tmp211=tmp206*tmp208*tmp210;
tmp212=tmp6+(-0.78539816f);
tmp213=sin(tmp212);
tmp214=tmp91+0.087266463f;
tmp215=sin(tmp214);
tmp216=(-1.0f)*tmp213*tmp215;
tmp217=tmp211+tmp216;
tmp218=(-0.26179939f)*tmp204*tmp217;
tmp219=tmp12+(-0.78539816f);
tmp220=cos(tmp219);
tmp221=tmp91+0.087266463f;
tmp222=cos(tmp221);
tmp223=tmp0+0.78539816f;
tmp224=sin(tmp223);
tmp225=sin(tmp22);
tmp226=tmp222*tmp224*tmp225;
tmp227=cos(tmp22);
tmp228=tmp0+0.78539816f;
tmp229=cos(tmp228);
tmp230=tmp91+0.087266463f;
tmp231=cos(tmp230);
tmp232=tmp6+(-0.78539816f);
tmp233=sin(tmp232);
tmp234=(-1.0f)*tmp229*tmp231*tmp233;
tmp235=tmp6+(-0.78539816f);
tmp236=cos(tmp235);
tmp237=tmp91+0.087266463f;
tmp238=sin(tmp237);
tmp239=(-1.0f)*tmp236*tmp238;
tmp240=tmp234+tmp239;
tmp241=tmp227*tmp240;
tmp242=tmp226+tmp241;
tmp243=0.26179939f*tmp220*tmp242;
tmp244=tmp218+tmp243;
dresidual1_d_elbow=tmp244;
dresidual1_d_forearm=0.0f;
tmp245=tmp0+0.78539816f;
tmp246=sin(tmp245);
tmp247=tmp91+0.087266463f;
tmp248=sin(tmp247);
tmp249=0.063268185f*tmp246*tmp248;
tmp250=tmp6+(-0.78539816f);
tmp251=cos(tmp250);
tmp252=tmp0+0.78539816f;
tmp253=sin(tmp252);
tmp254=tmp91+0.087266463f;
tmp255=sin(tmp254);
tmp256=0.15707963f*tmp251*tmp253*tmp255;
tmp257=tmp12+(-0.78539816f);
tmp258=cos(tmp257);
tmp259=tmp6+(-0.78539816f);
tmp260=cos(tmp259);
tmp261=tmp0+0.78539816f;
tmp262=sin(tmp261);
tmp263=tmp91+0.087266463f;
tmp264=sin(tmp263);
tmp265=0.26179939f*tmp258*tmp260*tmp262*tmp264;
tmp266=tmp12+(-0.78539816f);
tmp267=sin(tmp266);
tmp268=tmp0+0.78539816f;
tmp269=cos(tmp268);
tmp270=tmp91+0.087266463f;
tmp271=sin(tmp270);
tmp272=sin(tmp22);
tmp273=(-0.017453293f)*tmp269*tmp271*tmp272;
tmp274=cos(tmp22);
tmp275=tmp0+0.78539816f;
tmp276=sin(tmp275);
tmp277=tmp6+(-0.78539816f);
tmp278=sin(tmp277);
tmp279=tmp91+0.087266463f;
tmp280=sin(tmp279);
tmp281=(-0.017453293f)*tmp274*tmp276*tmp278*tmp280;
tmp282=tmp273+tmp281;
tmp283=15.0f*tmp267*tmp282;
tmp284=tmp249+tmp256+tmp265+tmp283;
dresidual2_d_shoulderio=tmp284;
tmp285=tmp0+0.78539816f;
tmp286=cos(tmp285);
tmp287=tmp6+(-0.78539816f);
tmp288=sin(tmp287);
tmp289=tmp91+0.087266463f;
tmp290=sin(tmp289);
tmp291=(-0.15707963f)*tmp286*tmp288*tmp290;
tmp292=tmp6+(-0.78539816f);
tmp293=cos(tmp292);
tmp294=tmp91+0.087266463f;
tmp295=cos(tmp294);
tmp296=0.15707963f*tmp293*tmp295;
tmp297=tmp12+(-0.78539816f);
tmp298=cos(tmp297);
tmp299=tmp0+0.78539816f;
tmp300=cos(tmp299);
tmp301=tmp6+(-0.78539816f);
tmp302=sin(tmp301);
tmp303=tmp91+0.087266463f;
tmp304=sin(tmp303);
tmp305=(-0.017453293f)*tmp300*tmp302*tmp304;
tmp306=tmp6+(-0.78539816f);
tmp307=cos(tmp306);
tmp308=tmp91+0.087266463f;
tmp309=cos(tmp308);
tmp310=0.017453293f*tmp307*tmp309;
tmp311=tmp305+tmp310;
tmp312=15.0f*tmp298*tmp311;
tmp313=cos(tmp22);
tmp314=tmp12+(-0.78539816f);
tmp315=sin(tmp314);
tmp316=tmp0+0.78539816f;
tmp317=cos(tmp316);
tmp318=tmp6+(-0.78539816f);
tmp319=cos(tmp318);
tmp320=tmp91+0.087266463f;
tmp321=sin(tmp320);
tmp322=(-0.017453293f)*tmp317*tmp319*tmp321;
tmp323=tmp91+0.087266463f;
tmp324=cos(tmp323);
tmp325=tmp6+(-0.78539816f);
tmp326=sin(tmp325);
tmp327=(-0.017453293f)*tmp324*tmp326;
tmp328=tmp322+tmp327;
tmp329=15.0f*tmp313*tmp315*tmp328;
tmp330=tmp291+tmp296+tmp312+tmp329;
dresidual2_d_shoulderud=tmp330;
tmp331=tmp12+(-0.78539816f);
tmp332=sin(tmp331);
tmp333=cos(tmp22);
tmp334=tmp0+0.78539816f;
tmp335=sin(tmp334);
tmp336=tmp91+0.087266463f;
tmp337=sin(tmp336);
tmp338=(-0.017453293f)*tmp333*tmp335*tmp337;
tmp339=sin(tmp22);
tmp340=tmp6+(-0.78539816f);
tmp341=cos(tmp340);
tmp342=tmp91+0.087266463f;
tmp343=cos(tmp342);
tmp344=tmp341*tmp343;
tmp345=tmp0+0.78539816f;
tmp346=cos(tmp345);
tmp347=tmp6+(-0.78539816f);
tmp348=sin(tmp347);
tmp349=tmp91+0.087266463f;
tmp350=sin(tmp349);
tmp351=(-1.0f)*tmp346*tmp348*tmp350;
tmp352=tmp344+tmp351;
tmp353=0.017453293f*tmp339*tmp352;
tmp354=tmp338+tmp353;
tmp355=15.0f*tmp332*tmp354;
dresidual2_d_bicep=tmp355;
tmp356=tmp12+(-0.78539816f);
tmp357=sin(tmp356);
tmp358=tmp0+0.78539816f;
tmp359=cos(tmp358);
tmp360=tmp6+(-0.78539816f);
tmp361=cos(tmp360);
tmp362=tmp91+0.087266463f;
tmp363=sin(tmp362);
tmp364=tmp359*tmp361*tmp363;
tmp365=tmp91+0.087266463f;
tmp366=cos(tmp365);
tmp367=tmp6+(-0.78539816f);
tmp368=sin(tmp367);
tmp369=tmp366*tmp368;
tmp370=tmp364+tmp369;
tmp371=(-0.26179939f)*tmp357*tmp370;
tmp372=tmp12+(-0.78539816f);
tmp373=cos(tmp372);
tmp374=cos(tmp22);
tmp375=tmp6+(-0.78539816f);
tmp376=cos(tmp375);
tmp377=tmp91+0.087266463f;
tmp378=cos(tmp377);
tmp379=tmp376*tmp378;
tmp380=tmp0+0.78539816f;
tmp381=cos(tmp380);
tmp382=tmp6+(-0.78539816f);
tmp383=sin(tmp382);
tmp384=tmp91+0.087266463f;
tmp385=sin(tmp384);
tmp386=(-1.0f)*tmp381*tmp383*tmp385;
tmp387=tmp379+tmp386;
tmp388=tmp374*tmp387;
tmp389=tmp0+0.78539816f;
tmp390=sin(tmp389);
tmp391=tmp91+0.087266463f;
tmp392=sin(tmp391);
tmp393=sin(tmp22);
tmp394=tmp390*tmp392*tmp393;
tmp395=tmp388+tmp394;
tmp396=0.26179939f*tmp373*tmp395;
tmp397=tmp371+tmp396;
dresidual2_d_elbow=tmp397;
dresidual2_d_forearm=0.0f;
tmp398=(-0.017453293f)*_forearm;
tmp399=tmp398+(-0.6981317f);
tmp400=cos(tmp399);
tmp401=tmp12+(-0.78539816f);
tmp402=cos(tmp401);
tmp403=tmp0+0.78539816f;
tmp404=cos(tmp403);
tmp405=cos(tmp22);
tmp406=tmp6+(-0.78539816f);
tmp407=sin(tmp406);
tmp408=(-0.017453293f)*tmp404*tmp405*tmp407;
tmp409=tmp0+0.78539816f;
tmp410=sin(tmp409);
tmp411=sin(tmp22);
tmp412=0.017453293f*tmp410*tmp411;
tmp413=tmp408+tmp412;
tmp414=tmp402*tmp413;
tmp415=tmp0+0.78539816f;
tmp416=cos(tmp415);
tmp417=tmp6+(-0.78539816f);
tmp418=cos(tmp417);
tmp419=tmp12+(-0.78539816f);
tmp420=sin(tmp419);
tmp421=(-0.017453293f)*tmp416*tmp418*tmp420;
tmp422=tmp414+tmp421;
tmp423=tmp400*tmp422;
tmp424=tmp398+(-0.6981317f);
tmp425=sin(tmp424);
tmp426=tmp0+0.78539816f;
tmp427=cos(tmp426);
tmp428=tmp6+(-0.78539816f);
tmp429=sin(tmp428);
tmp430=sin(tmp22);
tmp431=0.017453293f*tmp427*tmp429*tmp430;
tmp432=cos(tmp22);
tmp433=tmp0+0.78539816f;
tmp434=sin(tmp433);
tmp435=0.017453293f*tmp432*tmp434;
tmp436=tmp431+tmp435;
tmp437=tmp425*tmp436;
tmp438=tmp423+tmp437;
dresidual3_d_shoulderio=tmp438;
tmp439=tmp398+(-0.6981317f);
tmp440=cos(tmp439);
tmp441=tmp0+0.78539816f;
tmp442=sin(tmp441);
tmp443=tmp12+(-0.78539816f);
tmp444=sin(tmp443);
tmp445=tmp6+(-0.78539816f);
tmp446=sin(tmp445);
tmp447=(-0.017453293f)*tmp442*tmp444*tmp446;
tmp448=tmp12+(-0.78539816f);
tmp449=cos(tmp448);
tmp450=tmp6+(-0.78539816f);
tmp451=cos(tmp450);
tmp452=cos(tmp22);
tmp453=tmp0+0.78539816f;
tmp454=sin(tmp453);
tmp455=0.017453293f*tmp449*tmp451*tmp452*tmp454;
tmp456=tmp447+tmp455;
tmp457=tmp440*tmp456;
tmp458=tmp6+(-0.78539816f);
tmp459=cos(tmp458);
tmp460=tmp398+(-0.6981317f);
tmp461=sin(tmp460);
tmp462=tmp0+0.78539816f;
tmp463=sin(tmp462);
tmp464=sin(tmp22);
tmp465=(-0.017453293f)*tmp459*tmp461*tmp463*tmp464;
tmp466=tmp457+tmp465;
dresidual3_d_shoulderud=tmp466;
tmp467=tmp398+(-0.6981317f);
tmp468=cos(tmp467);
tmp469=tmp12+(-0.78539816f);
tmp470=cos(tmp469);
tmp471=tmp0+0.78539816f;
tmp472=cos(tmp471);
tmp473=cos(tmp22);
tmp474=(-0.017453293f)*tmp472*tmp473;
tmp475=tmp0+0.78539816f;
tmp476=sin(tmp475);
tmp477=tmp6+(-0.78539816f);
tmp478=sin(tmp477);
tmp479=sin(tmp22);
tmp480=0.017453293f*tmp476*tmp478*tmp479;
tmp481=tmp474+tmp480;
tmp482=tmp468*tmp470*tmp481;
tmp483=tmp398+(-0.6981317f);
tmp484=sin(tmp483);
tmp485=tmp0+0.78539816f;
tmp486=cos(tmp485);
tmp487=sin(tmp22);
tmp488=0.017453293f*tmp486*tmp487;
tmp489=cos(tmp22);
tmp490=tmp0+0.78539816f;
tmp491=sin(tmp490);
tmp492=tmp6+(-0.78539816f);
tmp493=sin(tmp492);
tmp494=0.017453293f*tmp489*tmp491*tmp493;
tmp495=tmp488+tmp494;
tmp496=tmp484*tmp495;
tmp497=tmp482+tmp496;
dresidual3_d_bicep=tmp497;
tmp498=tmp398+(-0.6981317f);
tmp499=cos(tmp498);
tmp500=tmp12+(-0.78539816f);
tmp501=sin(tmp500);
tmp502=tmp0+0.78539816f;
tmp503=cos(tmp502);
tmp504=sin(tmp22);
tmp505=tmp503*tmp504;
tmp506=cos(tmp22);
tmp507=tmp0+0.78539816f;
tmp508=sin(tmp507);
tmp509=tmp6+(-0.78539816f);
tmp510=sin(tmp509);
tmp511=tmp506*tmp508*tmp510;
tmp512=tmp505+tmp511;
tmp513=(-0.017453293f)*tmp501*tmp512;
tmp514=tmp12+(-0.78539816f);
tmp515=cos(tmp514);
tmp516=tmp6+(-0.78539816f);
tmp517=cos(tmp516);
tmp518=tmp0+0.78539816f;
tmp519=sin(tmp518);
tmp520=0.017453293f*tmp515*tmp517*tmp519;
tmp521=tmp513+tmp520;
tmp522=tmp499*tmp521;
dresidual3_d_elbow=tmp522;
tmp523=tmp398+(-0.6981317f);
tmp524=cos(tmp523);
tmp525=tmp0+0.78539816f;
tmp526=cos(tmp525);
tmp527=cos(tmp22);
tmp528=tmp526*tmp527;
tmp529=tmp0+0.78539816f;
tmp530=sin(tmp529);
tmp531=tmp6+(-0.78539816f);
tmp532=sin(tmp531);
tmp533=sin(tmp22);
tmp534=(-1.0f)*tmp530*tmp532*tmp533;
tmp535=tmp528+tmp534;
tmp536=(-0.017453293f)*tmp524*tmp535;
tmp537=tmp398+(-0.6981317f);
tmp538=sin(tmp537);
tmp539=tmp12+(-0.78539816f);
tmp540=cos(tmp539);
tmp541=tmp0+0.78539816f;
tmp542=cos(tmp541);
tmp543=sin(tmp22);
tmp544=tmp542*tmp543;
tmp545=cos(tmp22);
tmp546=tmp0+0.78539816f;
tmp547=sin(tmp546);
tmp548=tmp6+(-0.78539816f);
tmp549=sin(tmp548);
tmp550=tmp545*tmp547*tmp549;
tmp551=tmp544+tmp550;
tmp552=tmp540*tmp551;
tmp553=tmp6+(-0.78539816f);
tmp554=cos(tmp553);
tmp555=tmp0+0.78539816f;
tmp556=sin(tmp555);
tmp557=tmp12+(-0.78539816f);
tmp558=sin(tmp557);
tmp559=tmp554*tmp556*tmp558;
tmp560=tmp552+tmp559;
tmp561=0.017453293f*tmp538*tmp560;
tmp562=tmp536+tmp561;
dresidual3_d_forearm=tmp562;
tmp563=tmp12+(-0.78539816f);
tmp564=sin(tmp563);
tmp565=tmp0+0.78539816f;
tmp566=cos(tmp565);
tmp567=cos(tmp22);
tmp568=tmp6+(-0.78539816f);
tmp569=sin(tmp568);
tmp570=(-0.017453293f)*tmp566*tmp567*tmp569;
tmp571=tmp0+0.78539816f;
tmp572=sin(tmp571);
tmp573=sin(tmp22);
tmp574=0.017453293f*tmp572*tmp573;
tmp575=tmp570+tmp574;
tmp576=tmp564*tmp575;
tmp577=tmp0+0.78539816f;
tmp578=cos(tmp577);
tmp579=tmp12+(-0.78539816f);
tmp580=cos(tmp579);
tmp581=tmp6+(-0.78539816f);
tmp582=cos(tmp581);
tmp583=0.017453293f*tmp578*tmp580*tmp582;
tmp584=tmp576+tmp583;
dresidual4_d_shoulderio=tmp584;
tmp585=tmp12+(-0.78539816f);
tmp586=cos(tmp585);
tmp587=tmp0+0.78539816f;
tmp588=sin(tmp587);
tmp589=tmp6+(-0.78539816f);
tmp590=sin(tmp589);
tmp591=0.017453293f*tmp586*tmp588*tmp590;
tmp592=tmp6+(-0.78539816f);
tmp593=cos(tmp592);
tmp594=cos(tmp22);
tmp595=tmp0+0.78539816f;
tmp596=sin(tmp595);
tmp597=tmp12+(-0.78539816f);
tmp598=sin(tmp597);
tmp599=0.017453293f*tmp593*tmp594*tmp596*tmp598;
tmp600=tmp591+tmp599;
dresidual4_d_shoulderud=tmp600;
tmp601=tmp12+(-0.78539816f);
tmp602=sin(tmp601);
tmp603=tmp0+0.78539816f;
tmp604=cos(tmp603);
tmp605=cos(tmp22);
tmp606=(-0.017453293f)*tmp604*tmp605;
tmp607=tmp0+0.78539816f;
tmp608=sin(tmp607);
tmp609=tmp6+(-0.78539816f);
tmp610=sin(tmp609);
tmp611=sin(tmp22);
tmp612=0.017453293f*tmp608*tmp610*tmp611;
tmp613=tmp606+tmp612;
tmp614=tmp602*tmp613;
dresidual4_d_bicep=tmp614;
tmp615=tmp12+(-0.78539816f);
tmp616=cos(tmp615);
tmp617=tmp0+0.78539816f;
tmp618=cos(tmp617);
tmp619=sin(tmp22);
tmp620=tmp618*tmp619;
tmp621=cos(tmp22);
tmp622=tmp0+0.78539816f;
tmp623=sin(tmp622);
tmp624=tmp6+(-0.78539816f);
tmp625=sin(tmp624);
tmp626=tmp621*tmp623*tmp625;
tmp627=tmp620+tmp626;
tmp628=0.017453293f*tmp616*tmp627;
tmp629=tmp6+(-0.78539816f);
tmp630=cos(tmp629);
tmp631=tmp0+0.78539816f;
tmp632=sin(tmp631);
tmp633=tmp12+(-0.78539816f);
tmp634=sin(tmp633);
tmp635=0.017453293f*tmp630*tmp632*tmp634;
tmp636=tmp628+tmp635;
dresidual4_d_elbow=tmp636;
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
} // ik_rightarmdf()
