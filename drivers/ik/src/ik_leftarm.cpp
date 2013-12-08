#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <cassert>
enum { ik_leftarmglobal_count=2 };
extern const char *ik_leftarmglobal_names[2];

enum { ik_leftarmparameter_count=11 };
extern const char *ik_leftarmparameter_names[11];

enum { ik_leftarmvariable_count=5 };
extern const char *ik_leftarmvariable_names[5];

enum { ik_leftarmequation_count=4 };
extern const char *ik_leftarmequation_names[4];
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
  "residual3"
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
void ik_leftarminitialize(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[11],
  float ik_leftarmx[5]
);
void ik_leftarmupdate(
  const float ik_leftarmglobals[2],
  float ik_leftarmparameters[11]
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
      float y[4];
      float dx[5];
      float dy[20];
      float tmp[4];
      float dyt[20];
      float dydyt[16];
      int iwork[4];
  
      ik_leftarmf(globals,parameters,x,y);
      residual = ik_leftarmnorm(4,y);
      ik_leftarmdf(globals,parameters,x,dy);

     for (int i=0; i<4; ++i) {
       for (int j=0; j<5; ++j) {
         dyt[j+5*i]=dy[i+4*j];
       }
     }

     for (int i=0; i<4; ++i) {
       for (int j=0; j<4; ++j) {
         float sum=0;
         for (int k=0; k<5; ++k) {
            sum += dy[i+4*k]*dyt[k+5*j];
         }
         dydyt[i+4*j]=sum;
       }
     }

      ik_leftarmlinear_solve(4,iwork,dydyt,y,tmp);

     for (int i=0; i<5; ++i) {
       float sum=0;
       for (int k=0; k<4; ++k) {
          sum += dyt[i+5*k]*tmp[k];
       }
       dx[i]=sum;
     }
      
      
      for (int k=0; k<5; ++k) { x[k] -= dx[k]; }

      for (int damping=0; damping < steps; ++damping) {
	    ik_leftarmf(globals,parameters,x,y);
	    new_residual=ik_leftarmnorm(4,y);
	    if (new_residual < residual) break;
	    for (int k=0; k<5; ++k) { dx[k] /= 2; }
	    for (int k=0; k<5; ++k) { x[k] += dx[k] ; }
      }
      residual=new_residual;
      if (residual <= epsilon) break;
    }
    parameters[10]=residual;
  } //  ik_leftarmsolve()
typedef struct {
  const char *ik_leftarmname;
  float ik_leftarmglobals[2];
  float ik_leftarmparameters[11];
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
  float ik_leftarmy_test[4];

  for (int ik_leftarmcase_count=0; ik_leftarmcase_count < 0; ++ik_leftarmcase_count) {
    if (ik_leftarmname == "all" || ik_leftarmname == ik_leftarmtest_data[ik_leftarmcase_count].ik_leftarmname) {
       const char *ik_leftarmname=ik_leftarmtest_data[ik_leftarmcase_count].ik_leftarmname;
       float *ik_leftarmglobals=ik_leftarmtest_data[ik_leftarmcase_count].ik_leftarmglobals;
       float *ik_leftarmparameters=ik_leftarmtest_data[ik_leftarmcase_count].ik_leftarmparameters;
       float *ik_leftarmx=ik_leftarmtest_data[ik_leftarmcase_count].ik_leftarmx;

       ik_leftarmsolve(ik_leftarmglobals,ik_leftarmparameters,ik_leftarmx_test);
       ik_leftarmf(ik_leftarmglobals,ik_leftarmparameters,ik_leftarmx_test,ik_leftarmy_test);
       ik_leftarmx_error=ik_leftarmdist(5,ik_leftarmx,ik_leftarmx_test);
       ik_leftarmy_error=ik_leftarmnorm(4,ik_leftarmy_test);

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
  float ik_leftarmparameters[11];
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
#define px ik_leftarmparameters[6]
#define py ik_leftarmparameters[7]
#define pz ik_leftarmparameters[8]
#define waist ik_leftarmparameters[9]
#define residual ik_leftarmparameters[10]

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

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[4]
#define dresidual0_d_bicep ik_leftarmdy[8]
#define dresidual0_d_elbow ik_leftarmdy[12]
#define dresidual0_d_forearm ik_leftarmdy[16]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[5]
#define dresidual1_d_bicep ik_leftarmdy[9]
#define dresidual1_d_elbow ik_leftarmdy[13]
#define dresidual1_d_forearm ik_leftarmdy[17]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[6]
#define dresidual2_d_bicep ik_leftarmdy[10]
#define dresidual2_d_elbow ik_leftarmdy[14]
#define dresidual2_d_forearm ik_leftarmdy[18]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[7]
#define dresidual3_d_bicep ik_leftarmdy[11]
#define dresidual3_d_elbow ik_leftarmdy[15]
#define dresidual3_d_forearm ik_leftarmdy[19]

void ik_leftarminitialize(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[11],
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
#define px ik_leftarmparameters[6]
#define py ik_leftarmparameters[7]
#define pz ik_leftarmparameters[8]
#define waist ik_leftarmparameters[9]
#define residual ik_leftarmparameters[10]

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

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[4]
#define dresidual0_d_bicep ik_leftarmdy[8]
#define dresidual0_d_elbow ik_leftarmdy[12]
#define dresidual0_d_forearm ik_leftarmdy[16]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[5]
#define dresidual1_d_bicep ik_leftarmdy[9]
#define dresidual1_d_elbow ik_leftarmdy[13]
#define dresidual1_d_forearm ik_leftarmdy[17]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[6]
#define dresidual2_d_bicep ik_leftarmdy[10]
#define dresidual2_d_elbow ik_leftarmdy[14]
#define dresidual2_d_forearm ik_leftarmdy[18]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[7]
#define dresidual3_d_bicep ik_leftarmdy[11]
#define dresidual3_d_elbow ik_leftarmdy[15]
#define dresidual3_d_forearm ik_leftarmdy[19]

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
} // ik_leftarminitialize()
void ik_leftarmupdate(
  const float ik_leftarmglobals[2],
  float ik_leftarmparameters[11]
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
#define px ik_leftarmparameters[6]
#define py ik_leftarmparameters[7]
#define pz ik_leftarmparameters[8]
#define waist ik_leftarmparameters[9]
#define residual ik_leftarmparameters[10]

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

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[4]
#define dresidual0_d_bicep ik_leftarmdy[8]
#define dresidual0_d_elbow ik_leftarmdy[12]
#define dresidual0_d_forearm ik_leftarmdy[16]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[5]
#define dresidual1_d_bicep ik_leftarmdy[9]
#define dresidual1_d_elbow ik_leftarmdy[13]
#define dresidual1_d_forearm ik_leftarmdy[17]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[6]
#define dresidual2_d_bicep ik_leftarmdy[10]
#define dresidual2_d_elbow ik_leftarmdy[14]
#define dresidual2_d_forearm ik_leftarmdy[18]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[7]
#define dresidual3_d_bicep ik_leftarmdy[11]
#define dresidual3_d_elbow ik_leftarmdy[15]
#define dresidual3_d_forearm ik_leftarmdy[19]

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
} // ik_leftarmupdate()

void ik_leftarmf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[11],
  const float ik_leftarmx[5],
  float ik_leftarmy[4]
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
#define px ik_leftarmparameters[6]
#define py ik_leftarmparameters[7]
#define pz ik_leftarmparameters[8]
#define waist ik_leftarmparameters[9]
#define residual ik_leftarmparameters[10]

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

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[4]
#define dresidual0_d_bicep ik_leftarmdy[8]
#define dresidual0_d_elbow ik_leftarmdy[12]
#define dresidual0_d_forearm ik_leftarmdy[16]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[5]
#define dresidual1_d_bicep ik_leftarmdy[9]
#define dresidual1_d_elbow ik_leftarmdy[13]
#define dresidual1_d_forearm ik_leftarmdy[17]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[6]
#define dresidual2_d_bicep ik_leftarmdy[10]
#define dresidual2_d_elbow ik_leftarmdy[14]
#define dresidual2_d_forearm ik_leftarmdy[18]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[7]
#define dresidual3_d_bicep ik_leftarmdy[11]
#define dresidual3_d_elbow ik_leftarmdy[15]
#define dresidual3_d_forearm ik_leftarmdy[19]

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
tmp0=(-0.017453293f)*_elbow;
tmp1=tmp0+0.78539816f;
tmp2=cos(tmp1);
tmp3=0.017453293f*_shoulderud;
tmp4=tmp3+(-0.78539816f);
tmp5=cos(tmp4);
tmp6=(-0.017453293f)*_shoulderio;
tmp7=tmp6+0.78539816f;
tmp8=sin(tmp7);
tmp9=(-14.0f)*tmp2*tmp5*tmp8;
tmp10=tmp3+(-0.78539816f);
tmp11=cos(tmp10);
tmp12=tmp6+0.78539816f;
tmp13=sin(tmp12);
tmp14=(-8.5f)*tmp11*tmp13;
tmp15=tmp6+0.78539816f;
tmp16=sin(tmp15);
tmp17=(-3.625f)*tmp16;
tmp18=(-1.0f)*px;
tmp19=tmp0+0.78539816f;
tmp20=sin(tmp19);
tmp21=tmp6+0.78539816f;
tmp22=cos(tmp21);
tmp23=0.017453293f*_bicep;
tmp24=sin(tmp23);
tmp25=tmp22*tmp24;
tmp26=cos(tmp23);
tmp27=tmp6+0.78539816f;
tmp28=sin(tmp27);
tmp29=tmp3+(-0.78539816f);
tmp30=sin(tmp29);
tmp31=tmp26*tmp28*tmp30;
tmp32=tmp25+tmp31;
tmp33=14.0f*tmp20*tmp32;
tmp34=tmp9+tmp14+tmp17+tmp18+tmp33+(-4.875f);
residual0=tmp34;
tmp35=(-0.017453293f)*waist;
tmp36=sin(tmp35);
tmp37=(-14.0f)*tmp36;
tmp38=tmp3+(-0.78539816f);
tmp39=sin(tmp38);
tmp40=sin(tmp35);
tmp41=(-8.5f)*tmp39*tmp40;
tmp42=(-1.0f)*py;
tmp43=cos(tmp35);
tmp44=2.375f*tmp43;
tmp45=tmp6+0.78539816f;
tmp46=cos(tmp45);
tmp47=cos(tmp35);
tmp48=3.625f*tmp46*tmp47;
tmp49=tmp6+0.78539816f;
tmp50=cos(tmp49);
tmp51=tmp3+(-0.78539816f);
tmp52=cos(tmp51);
tmp53=cos(tmp35);
tmp54=8.5f*tmp50*tmp52*tmp53;
tmp55=tmp0+0.78539816f;
tmp56=cos(tmp55);
tmp57=tmp6+0.78539816f;
tmp58=cos(tmp57);
tmp59=tmp3+(-0.78539816f);
tmp60=cos(tmp59);
tmp61=cos(tmp35);
tmp62=tmp58*tmp60*tmp61;
tmp63=tmp3+(-0.78539816f);
tmp64=sin(tmp63);
tmp65=sin(tmp35);
tmp66=(-1.0f)*tmp64*tmp65;
tmp67=tmp62+tmp66;
tmp68=14.0f*tmp56*tmp67;
tmp69=tmp0+0.78539816f;
tmp70=sin(tmp69);
tmp71=cos(tmp35);
tmp72=tmp6+0.78539816f;
tmp73=sin(tmp72);
tmp74=sin(tmp23);
tmp75=tmp71*tmp73*tmp74;
tmp76=cos(tmp23);
tmp77=tmp6+0.78539816f;
tmp78=cos(tmp77);
tmp79=cos(tmp35);
tmp80=tmp3+(-0.78539816f);
tmp81=sin(tmp80);
tmp82=(-1.0f)*tmp78*tmp79*tmp81;
tmp83=tmp3+(-0.78539816f);
tmp84=cos(tmp83);
tmp85=sin(tmp35);
tmp86=(-1.0f)*tmp84*tmp85;
tmp87=tmp82+tmp86;
tmp88=tmp76*tmp87;
tmp89=tmp75+tmp88;
tmp90=14.0f*tmp70*tmp89;
tmp91=tmp37+tmp41+tmp42+tmp44+tmp48+tmp54+tmp68+tmp90;
residual1=tmp91;
tmp92=(-1.0f)*pz;
tmp93=sin(tmp35);
tmp94=2.375f*tmp93;
tmp95=tmp6+0.78539816f;
tmp96=cos(tmp95);
tmp97=sin(tmp35);
tmp98=3.625f*tmp96*tmp97;
tmp99=tmp6+0.78539816f;
tmp100=cos(tmp99);
tmp101=tmp3+(-0.78539816f);
tmp102=cos(tmp101);
tmp103=sin(tmp35);
tmp104=8.5f*tmp100*tmp102*tmp103;
tmp105=cos(tmp35);
tmp106=tmp3+(-0.78539816f);
tmp107=sin(tmp106);
tmp108=8.5f*tmp105*tmp107;
tmp109=tmp0+0.78539816f;
tmp110=cos(tmp109);
tmp111=tmp6+0.78539816f;
tmp112=cos(tmp111);
tmp113=tmp3+(-0.78539816f);
tmp114=cos(tmp113);
tmp115=sin(tmp35);
tmp116=tmp112*tmp114*tmp115;
tmp117=cos(tmp35);
tmp118=tmp3+(-0.78539816f);
tmp119=sin(tmp118);
tmp120=tmp117*tmp119;
tmp121=tmp116+tmp120;
tmp122=14.0f*tmp110*tmp121;
tmp123=cos(tmp35);
tmp124=14.0f*tmp123;
tmp125=tmp0+0.78539816f;
tmp126=sin(tmp125);
tmp127=cos(tmp23);
tmp128=tmp3+(-0.78539816f);
tmp129=cos(tmp128);
tmp130=cos(tmp35);
tmp131=tmp129*tmp130;
tmp132=tmp6+0.78539816f;
tmp133=cos(tmp132);
tmp134=tmp3+(-0.78539816f);
tmp135=sin(tmp134);
tmp136=sin(tmp35);
tmp137=(-1.0f)*tmp133*tmp135*tmp136;
tmp138=tmp131+tmp137;
tmp139=tmp127*tmp138;
tmp140=tmp6+0.78539816f;
tmp141=sin(tmp140);
tmp142=sin(tmp35);
tmp143=sin(tmp23);
tmp144=tmp141*tmp142*tmp143;
tmp145=tmp139+tmp144;
tmp146=14.0f*tmp126*tmp145;
tmp147=tmp92+tmp94+tmp98+tmp104+tmp108+tmp122+tmp124+tmp146+12.0f;
residual2=tmp147;
tmp148=0.017453293f*_forearm;
tmp149=tmp148+(-1.134464f);
tmp150=cos(tmp149);
tmp151=tmp0+0.78539816f;
tmp152=cos(tmp151);
tmp153=tmp6+0.78539816f;
tmp154=cos(tmp153);
tmp155=sin(tmp23);
tmp156=tmp154*tmp155;
tmp157=cos(tmp23);
tmp158=tmp6+0.78539816f;
tmp159=sin(tmp158);
tmp160=tmp3+(-0.78539816f);
tmp161=sin(tmp160);
tmp162=tmp157*tmp159*tmp161;
tmp163=tmp156+tmp162;
tmp164=tmp152*tmp163;
tmp165=tmp3+(-0.78539816f);
tmp166=cos(tmp165);
tmp167=tmp0+0.78539816f;
tmp168=sin(tmp167);
tmp169=tmp6+0.78539816f;
tmp170=sin(tmp169);
tmp171=tmp166*tmp168*tmp170;
tmp172=tmp164+tmp171;
tmp173=tmp150*tmp172;
tmp174=tmp148+(-1.134464f);
tmp175=sin(tmp174);
tmp176=tmp6+0.78539816f;
tmp177=cos(tmp176);
tmp178=cos(tmp23);
tmp179=tmp177*tmp178;
tmp180=tmp6+0.78539816f;
tmp181=sin(tmp180);
tmp182=tmp3+(-0.78539816f);
tmp183=sin(tmp182);
tmp184=sin(tmp23);
tmp185=(-1.0f)*tmp181*tmp183*tmp184;
tmp186=tmp179+tmp185;
tmp187=tmp175*tmp186;
tmp188=tmp173+tmp187+downx;
residual3=tmp188;
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
} // ik_leftarmf()

void ik_leftarmdf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[11],
  const float ik_leftarmx[5],
  float ik_leftarmdy[20]
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
#define px ik_leftarmparameters[6]
#define py ik_leftarmparameters[7]
#define pz ik_leftarmparameters[8]
#define waist ik_leftarmparameters[9]
#define residual ik_leftarmparameters[10]

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

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[4]
#define dresidual0_d_bicep ik_leftarmdy[8]
#define dresidual0_d_elbow ik_leftarmdy[12]
#define dresidual0_d_forearm ik_leftarmdy[16]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[5]
#define dresidual1_d_bicep ik_leftarmdy[9]
#define dresidual1_d_elbow ik_leftarmdy[13]
#define dresidual1_d_forearm ik_leftarmdy[17]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[6]
#define dresidual2_d_bicep ik_leftarmdy[10]
#define dresidual2_d_elbow ik_leftarmdy[14]
#define dresidual2_d_forearm ik_leftarmdy[18]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[7]
#define dresidual3_d_bicep ik_leftarmdy[11]
#define dresidual3_d_elbow ik_leftarmdy[15]
#define dresidual3_d_forearm ik_leftarmdy[19]

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
tmp0=(-0.017453293f)*_shoulderio;
tmp1=tmp0+0.78539816f;
tmp2=cos(tmp1);
tmp3=0.063268185f*tmp2;
tmp4=tmp0+0.78539816f;
tmp5=cos(tmp4);
tmp6=0.017453293f*_shoulderud;
tmp7=tmp6+(-0.78539816f);
tmp8=cos(tmp7);
tmp9=0.14835299f*tmp5*tmp8;
tmp10=(-0.017453293f)*_elbow;
tmp11=tmp10+0.78539816f;
tmp12=cos(tmp11);
tmp13=tmp0+0.78539816f;
tmp14=cos(tmp13);
tmp15=tmp6+(-0.78539816f);
tmp16=cos(tmp15);
tmp17=0.2443461f*tmp12*tmp14*tmp16;
tmp18=tmp10+0.78539816f;
tmp19=sin(tmp18);
tmp20=tmp0+0.78539816f;
tmp21=cos(tmp20);
tmp22=0.017453293f*_bicep;
tmp23=cos(tmp22);
tmp24=tmp6+(-0.78539816f);
tmp25=sin(tmp24);
tmp26=(-0.017453293f)*tmp21*tmp23*tmp25;
tmp27=tmp0+0.78539816f;
tmp28=sin(tmp27);
tmp29=sin(tmp22);
tmp30=0.017453293f*tmp28*tmp29;
tmp31=tmp26+tmp30;
tmp32=14.0f*tmp19*tmp31;
tmp33=tmp3+tmp9+tmp17+tmp32;
dresidual0_d_shoulderio=tmp33;
tmp34=tmp0+0.78539816f;
tmp35=sin(tmp34);
tmp36=tmp6+(-0.78539816f);
tmp37=sin(tmp36);
tmp38=0.14835299f*tmp35*tmp37;
tmp39=tmp10+0.78539816f;
tmp40=cos(tmp39);
tmp41=tmp0+0.78539816f;
tmp42=sin(tmp41);
tmp43=tmp6+(-0.78539816f);
tmp44=sin(tmp43);
tmp45=0.2443461f*tmp40*tmp42*tmp44;
tmp46=tmp6+(-0.78539816f);
tmp47=cos(tmp46);
tmp48=cos(tmp22);
tmp49=tmp10+0.78539816f;
tmp50=sin(tmp49);
tmp51=tmp0+0.78539816f;
tmp52=sin(tmp51);
tmp53=0.2443461f*tmp47*tmp48*tmp50*tmp52;
tmp54=tmp38+tmp45+tmp53;
dresidual0_d_shoulderud=tmp54;
tmp55=tmp10+0.78539816f;
tmp56=sin(tmp55);
tmp57=tmp0+0.78539816f;
tmp58=sin(tmp57);
tmp59=tmp6+(-0.78539816f);
tmp60=sin(tmp59);
tmp61=sin(tmp22);
tmp62=(-0.017453293f)*tmp58*tmp60*tmp61;
tmp63=tmp0+0.78539816f;
tmp64=cos(tmp63);
tmp65=cos(tmp22);
tmp66=0.017453293f*tmp64*tmp65;
tmp67=tmp62+tmp66;
tmp68=14.0f*tmp56*tmp67;
dresidual0_d_bicep=tmp68;
tmp69=tmp10+0.78539816f;
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
tmp82=(-0.2443461f)*tmp70*tmp81;
tmp83=tmp6+(-0.78539816f);
tmp84=cos(tmp83);
tmp85=tmp10+0.78539816f;
tmp86=sin(tmp85);
tmp87=tmp0+0.78539816f;
tmp88=sin(tmp87);
tmp89=(-0.2443461f)*tmp84*tmp86*tmp88;
tmp90=tmp82+tmp89;
dresidual0_d_elbow=tmp90;
dresidual0_d_forearm=0.0f;
tmp91=(-0.017453293f)*waist;
tmp92=cos(tmp91);
tmp93=tmp0+0.78539816f;
tmp94=sin(tmp93);
tmp95=0.063268185f*tmp92*tmp94;
tmp96=tmp6+(-0.78539816f);
tmp97=cos(tmp96);
tmp98=cos(tmp91);
tmp99=tmp0+0.78539816f;
tmp100=sin(tmp99);
tmp101=0.14835299f*tmp97*tmp98*tmp100;
tmp102=tmp10+0.78539816f;
tmp103=cos(tmp102);
tmp104=tmp6+(-0.78539816f);
tmp105=cos(tmp104);
tmp106=cos(tmp91);
tmp107=tmp0+0.78539816f;
tmp108=sin(tmp107);
tmp109=0.2443461f*tmp103*tmp105*tmp106*tmp108;
tmp110=tmp10+0.78539816f;
tmp111=sin(tmp110);
tmp112=tmp0+0.78539816f;
tmp113=cos(tmp112);
tmp114=cos(tmp91);
tmp115=sin(tmp22);
tmp116=(-0.017453293f)*tmp113*tmp114*tmp115;
tmp117=cos(tmp91);
tmp118=cos(tmp22);
tmp119=tmp0+0.78539816f;
tmp120=sin(tmp119);
tmp121=tmp6+(-0.78539816f);
tmp122=sin(tmp121);
tmp123=(-0.017453293f)*tmp117*tmp118*tmp120*tmp122;
tmp124=tmp116+tmp123;
tmp125=14.0f*tmp111*tmp124;
tmp126=tmp95+tmp101+tmp109+tmp125;
dresidual1_d_shoulderio=tmp126;
tmp127=tmp0+0.78539816f;
tmp128=cos(tmp127);
tmp129=cos(tmp91);
tmp130=tmp6+(-0.78539816f);
tmp131=sin(tmp130);
tmp132=(-0.14835299f)*tmp128*tmp129*tmp131;
tmp133=tmp6+(-0.78539816f);
tmp134=cos(tmp133);
tmp135=sin(tmp91);
tmp136=(-0.14835299f)*tmp134*tmp135;
tmp137=tmp10+0.78539816f;
tmp138=cos(tmp137);
tmp139=tmp0+0.78539816f;
tmp140=cos(tmp139);
tmp141=cos(tmp91);
tmp142=tmp6+(-0.78539816f);
tmp143=sin(tmp142);
tmp144=(-0.017453293f)*tmp140*tmp141*tmp143;
tmp145=tmp6+(-0.78539816f);
tmp146=cos(tmp145);
tmp147=sin(tmp91);
tmp148=(-0.017453293f)*tmp146*tmp147;
tmp149=tmp144+tmp148;
tmp150=14.0f*tmp138*tmp149;
tmp151=cos(tmp22);
tmp152=tmp10+0.78539816f;
tmp153=sin(tmp152);
tmp154=tmp0+0.78539816f;
tmp155=cos(tmp154);
tmp156=tmp6+(-0.78539816f);
tmp157=cos(tmp156);
tmp158=cos(tmp91);
tmp159=(-0.017453293f)*tmp155*tmp157*tmp158;
tmp160=tmp6+(-0.78539816f);
tmp161=sin(tmp160);
tmp162=sin(tmp91);
tmp163=0.017453293f*tmp161*tmp162;
tmp164=tmp159+tmp163;
tmp165=14.0f*tmp151*tmp153*tmp164;
tmp166=tmp132+tmp136+tmp150+tmp165;
dresidual1_d_shoulderud=tmp166;
tmp167=tmp10+0.78539816f;
tmp168=sin(tmp167);
tmp169=sin(tmp22);
tmp170=tmp0+0.78539816f;
tmp171=cos(tmp170);
tmp172=cos(tmp91);
tmp173=tmp6+(-0.78539816f);
tmp174=sin(tmp173);
tmp175=(-1.0f)*tmp171*tmp172*tmp174;
tmp176=tmp6+(-0.78539816f);
tmp177=cos(tmp176);
tmp178=sin(tmp91);
tmp179=(-1.0f)*tmp177*tmp178;
tmp180=tmp175+tmp179;
tmp181=(-0.017453293f)*tmp169*tmp180;
tmp182=cos(tmp91);
tmp183=cos(tmp22);
tmp184=tmp0+0.78539816f;
tmp185=sin(tmp184);
tmp186=0.017453293f*tmp182*tmp183*tmp185;
tmp187=tmp181+tmp186;
tmp188=14.0f*tmp168*tmp187;
dresidual1_d_bicep=tmp188;
tmp189=tmp10+0.78539816f;
tmp190=cos(tmp189);
tmp191=cos(tmp91);
tmp192=tmp0+0.78539816f;
tmp193=sin(tmp192);
tmp194=sin(tmp22);
tmp195=tmp191*tmp193*tmp194;
tmp196=cos(tmp22);
tmp197=tmp0+0.78539816f;
tmp198=cos(tmp197);
tmp199=cos(tmp91);
tmp200=tmp6+(-0.78539816f);
tmp201=sin(tmp200);
tmp202=(-1.0f)*tmp198*tmp199*tmp201;
tmp203=tmp6+(-0.78539816f);
tmp204=cos(tmp203);
tmp205=sin(tmp91);
tmp206=(-1.0f)*tmp204*tmp205;
tmp207=tmp202+tmp206;
tmp208=tmp196*tmp207;
tmp209=tmp195+tmp208;
tmp210=(-0.2443461f)*tmp190*tmp209;
tmp211=tmp10+0.78539816f;
tmp212=sin(tmp211);
tmp213=tmp0+0.78539816f;
tmp214=cos(tmp213);
tmp215=tmp6+(-0.78539816f);
tmp216=cos(tmp215);
tmp217=cos(tmp91);
tmp218=tmp214*tmp216*tmp217;
tmp219=tmp6+(-0.78539816f);
tmp220=sin(tmp219);
tmp221=sin(tmp91);
tmp222=(-1.0f)*tmp220*tmp221;
tmp223=tmp218+tmp222;
tmp224=0.2443461f*tmp212*tmp223;
tmp225=tmp210+tmp224;
dresidual1_d_elbow=tmp225;
dresidual1_d_forearm=0.0f;
tmp226=tmp0+0.78539816f;
tmp227=sin(tmp226);
tmp228=sin(tmp91);
tmp229=0.063268185f*tmp227*tmp228;
tmp230=tmp6+(-0.78539816f);
tmp231=cos(tmp230);
tmp232=tmp0+0.78539816f;
tmp233=sin(tmp232);
tmp234=sin(tmp91);
tmp235=0.14835299f*tmp231*tmp233*tmp234;
tmp236=tmp10+0.78539816f;
tmp237=cos(tmp236);
tmp238=tmp6+(-0.78539816f);
tmp239=cos(tmp238);
tmp240=tmp0+0.78539816f;
tmp241=sin(tmp240);
tmp242=sin(tmp91);
tmp243=0.2443461f*tmp237*tmp239*tmp241*tmp242;
tmp244=tmp10+0.78539816f;
tmp245=sin(tmp244);
tmp246=tmp0+0.78539816f;
tmp247=cos(tmp246);
tmp248=sin(tmp91);
tmp249=sin(tmp22);
tmp250=(-0.017453293f)*tmp247*tmp248*tmp249;
tmp251=cos(tmp22);
tmp252=tmp0+0.78539816f;
tmp253=sin(tmp252);
tmp254=tmp6+(-0.78539816f);
tmp255=sin(tmp254);
tmp256=sin(tmp91);
tmp257=(-0.017453293f)*tmp251*tmp253*tmp255*tmp256;
tmp258=tmp250+tmp257;
tmp259=14.0f*tmp245*tmp258;
tmp260=tmp229+tmp235+tmp243+tmp259;
dresidual2_d_shoulderio=tmp260;
tmp261=tmp0+0.78539816f;
tmp262=cos(tmp261);
tmp263=tmp6+(-0.78539816f);
tmp264=sin(tmp263);
tmp265=sin(tmp91);
tmp266=(-0.14835299f)*tmp262*tmp264*tmp265;
tmp267=tmp6+(-0.78539816f);
tmp268=cos(tmp267);
tmp269=cos(tmp91);
tmp270=0.14835299f*tmp268*tmp269;
tmp271=tmp10+0.78539816f;
tmp272=cos(tmp271);
tmp273=tmp0+0.78539816f;
tmp274=cos(tmp273);
tmp275=tmp6+(-0.78539816f);
tmp276=sin(tmp275);
tmp277=sin(tmp91);
tmp278=(-0.017453293f)*tmp274*tmp276*tmp277;
tmp279=tmp6+(-0.78539816f);
tmp280=cos(tmp279);
tmp281=cos(tmp91);
tmp282=0.017453293f*tmp280*tmp281;
tmp283=tmp278+tmp282;
tmp284=14.0f*tmp272*tmp283;
tmp285=cos(tmp22);
tmp286=tmp10+0.78539816f;
tmp287=sin(tmp286);
tmp288=tmp0+0.78539816f;
tmp289=cos(tmp288);
tmp290=tmp6+(-0.78539816f);
tmp291=cos(tmp290);
tmp292=sin(tmp91);
tmp293=(-0.017453293f)*tmp289*tmp291*tmp292;
tmp294=cos(tmp91);
tmp295=tmp6+(-0.78539816f);
tmp296=sin(tmp295);
tmp297=(-0.017453293f)*tmp294*tmp296;
tmp298=tmp293+tmp297;
tmp299=14.0f*tmp285*tmp287*tmp298;
tmp300=tmp266+tmp270+tmp284+tmp299;
dresidual2_d_shoulderud=tmp300;
tmp301=tmp10+0.78539816f;
tmp302=sin(tmp301);
tmp303=sin(tmp22);
tmp304=tmp6+(-0.78539816f);
tmp305=cos(tmp304);
tmp306=cos(tmp91);
tmp307=tmp305*tmp306;
tmp308=tmp0+0.78539816f;
tmp309=cos(tmp308);
tmp310=tmp6+(-0.78539816f);
tmp311=sin(tmp310);
tmp312=sin(tmp91);
tmp313=(-1.0f)*tmp309*tmp311*tmp312;
tmp314=tmp307+tmp313;
tmp315=(-0.017453293f)*tmp303*tmp314;
tmp316=cos(tmp22);
tmp317=tmp0+0.78539816f;
tmp318=sin(tmp317);
tmp319=sin(tmp91);
tmp320=0.017453293f*tmp316*tmp318*tmp319;
tmp321=tmp315+tmp320;
tmp322=14.0f*tmp302*tmp321;
dresidual2_d_bicep=tmp322;
tmp323=tmp10+0.78539816f;
tmp324=cos(tmp323);
tmp325=cos(tmp22);
tmp326=tmp6+(-0.78539816f);
tmp327=cos(tmp326);
tmp328=cos(tmp91);
tmp329=tmp327*tmp328;
tmp330=tmp0+0.78539816f;
tmp331=cos(tmp330);
tmp332=tmp6+(-0.78539816f);
tmp333=sin(tmp332);
tmp334=sin(tmp91);
tmp335=(-1.0f)*tmp331*tmp333*tmp334;
tmp336=tmp329+tmp335;
tmp337=tmp325*tmp336;
tmp338=tmp0+0.78539816f;
tmp339=sin(tmp338);
tmp340=sin(tmp91);
tmp341=sin(tmp22);
tmp342=tmp339*tmp340*tmp341;
tmp343=tmp337+tmp342;
tmp344=(-0.2443461f)*tmp324*tmp343;
tmp345=tmp10+0.78539816f;
tmp346=sin(tmp345);
tmp347=tmp0+0.78539816f;
tmp348=cos(tmp347);
tmp349=tmp6+(-0.78539816f);
tmp350=cos(tmp349);
tmp351=sin(tmp91);
tmp352=tmp348*tmp350*tmp351;
tmp353=cos(tmp91);
tmp354=tmp6+(-0.78539816f);
tmp355=sin(tmp354);
tmp356=tmp353*tmp355;
tmp357=tmp352+tmp356;
tmp358=0.2443461f*tmp346*tmp357;
tmp359=tmp344+tmp358;
dresidual2_d_elbow=tmp359;
dresidual2_d_forearm=0.0f;
tmp360=0.017453293f*_forearm;
tmp361=tmp360+(-1.134464f);
tmp362=cos(tmp361);
tmp363=tmp10+0.78539816f;
tmp364=cos(tmp363);
tmp365=tmp0+0.78539816f;
tmp366=cos(tmp365);
tmp367=cos(tmp22);
tmp368=tmp6+(-0.78539816f);
tmp369=sin(tmp368);
tmp370=(-0.017453293f)*tmp366*tmp367*tmp369;
tmp371=tmp0+0.78539816f;
tmp372=sin(tmp371);
tmp373=sin(tmp22);
tmp374=0.017453293f*tmp372*tmp373;
tmp375=tmp370+tmp374;
tmp376=tmp364*tmp375;
tmp377=tmp0+0.78539816f;
tmp378=cos(tmp377);
tmp379=tmp6+(-0.78539816f);
tmp380=cos(tmp379);
tmp381=tmp10+0.78539816f;
tmp382=sin(tmp381);
tmp383=(-0.017453293f)*tmp378*tmp380*tmp382;
tmp384=tmp376+tmp383;
tmp385=tmp362*tmp384;
tmp386=tmp360+(-1.134464f);
tmp387=sin(tmp386);
tmp388=tmp0+0.78539816f;
tmp389=cos(tmp388);
tmp390=tmp6+(-0.78539816f);
tmp391=sin(tmp390);
tmp392=sin(tmp22);
tmp393=0.017453293f*tmp389*tmp391*tmp392;
tmp394=cos(tmp22);
tmp395=tmp0+0.78539816f;
tmp396=sin(tmp395);
tmp397=0.017453293f*tmp394*tmp396;
tmp398=tmp393+tmp397;
tmp399=tmp387*tmp398;
tmp400=tmp385+tmp399;
dresidual3_d_shoulderio=tmp400;
tmp401=tmp360+(-1.134464f);
tmp402=cos(tmp401);
tmp403=tmp10+0.78539816f;
tmp404=sin(tmp403);
tmp405=tmp0+0.78539816f;
tmp406=sin(tmp405);
tmp407=tmp6+(-0.78539816f);
tmp408=sin(tmp407);
tmp409=(-0.017453293f)*tmp404*tmp406*tmp408;
tmp410=tmp10+0.78539816f;
tmp411=cos(tmp410);
tmp412=tmp6+(-0.78539816f);
tmp413=cos(tmp412);
tmp414=cos(tmp22);
tmp415=tmp0+0.78539816f;
tmp416=sin(tmp415);
tmp417=0.017453293f*tmp411*tmp413*tmp414*tmp416;
tmp418=tmp409+tmp417;
tmp419=tmp402*tmp418;
tmp420=tmp6+(-0.78539816f);
tmp421=cos(tmp420);
tmp422=tmp0+0.78539816f;
tmp423=sin(tmp422);
tmp424=tmp360+(-1.134464f);
tmp425=sin(tmp424);
tmp426=sin(tmp22);
tmp427=(-0.017453293f)*tmp421*tmp423*tmp425*tmp426;
tmp428=tmp419+tmp427;
dresidual3_d_shoulderud=tmp428;
tmp429=tmp10+0.78539816f;
tmp430=cos(tmp429);
tmp431=tmp360+(-1.134464f);
tmp432=cos(tmp431);
tmp433=tmp0+0.78539816f;
tmp434=sin(tmp433);
tmp435=tmp6+(-0.78539816f);
tmp436=sin(tmp435);
tmp437=sin(tmp22);
tmp438=(-0.017453293f)*tmp434*tmp436*tmp437;
tmp439=tmp0+0.78539816f;
tmp440=cos(tmp439);
tmp441=cos(tmp22);
tmp442=0.017453293f*tmp440*tmp441;
tmp443=tmp438+tmp442;
tmp444=tmp430*tmp432*tmp443;
tmp445=tmp360+(-1.134464f);
tmp446=sin(tmp445);
tmp447=tmp0+0.78539816f;
tmp448=cos(tmp447);
tmp449=sin(tmp22);
tmp450=(-0.017453293f)*tmp448*tmp449;
tmp451=cos(tmp22);
tmp452=tmp0+0.78539816f;
tmp453=sin(tmp452);
tmp454=tmp6+(-0.78539816f);
tmp455=sin(tmp454);
tmp456=(-0.017453293f)*tmp451*tmp453*tmp455;
tmp457=tmp450+tmp456;
tmp458=tmp446*tmp457;
tmp459=tmp444+tmp458;
dresidual3_d_bicep=tmp459;
tmp460=tmp360+(-1.134464f);
tmp461=cos(tmp460);
tmp462=tmp10+0.78539816f;
tmp463=cos(tmp462);
tmp464=tmp6+(-0.78539816f);
tmp465=cos(tmp464);
tmp466=tmp0+0.78539816f;
tmp467=sin(tmp466);
tmp468=(-0.017453293f)*tmp463*tmp465*tmp467;
tmp469=tmp10+0.78539816f;
tmp470=sin(tmp469);
tmp471=tmp0+0.78539816f;
tmp472=cos(tmp471);
tmp473=sin(tmp22);
tmp474=tmp472*tmp473;
tmp475=cos(tmp22);
tmp476=tmp0+0.78539816f;
tmp477=sin(tmp476);
tmp478=tmp6+(-0.78539816f);
tmp479=sin(tmp478);
tmp480=tmp475*tmp477*tmp479;
tmp481=tmp474+tmp480;
tmp482=0.017453293f*tmp470*tmp481;
tmp483=tmp468+tmp482;
tmp484=tmp461*tmp483;
dresidual3_d_elbow=tmp484;
tmp485=tmp360+(-1.134464f);
tmp486=sin(tmp485);
tmp487=tmp10+0.78539816f;
tmp488=cos(tmp487);
tmp489=tmp0+0.78539816f;
tmp490=cos(tmp489);
tmp491=sin(tmp22);
tmp492=tmp490*tmp491;
tmp493=cos(tmp22);
tmp494=tmp0+0.78539816f;
tmp495=sin(tmp494);
tmp496=tmp6+(-0.78539816f);
tmp497=sin(tmp496);
tmp498=tmp493*tmp495*tmp497;
tmp499=tmp492+tmp498;
tmp500=tmp488*tmp499;
tmp501=tmp6+(-0.78539816f);
tmp502=cos(tmp501);
tmp503=tmp10+0.78539816f;
tmp504=sin(tmp503);
tmp505=tmp0+0.78539816f;
tmp506=sin(tmp505);
tmp507=tmp502*tmp504*tmp506;
tmp508=tmp500+tmp507;
tmp509=(-0.017453293f)*tmp486*tmp508;
tmp510=tmp360+(-1.134464f);
tmp511=cos(tmp510);
tmp512=tmp0+0.78539816f;
tmp513=cos(tmp512);
tmp514=cos(tmp22);
tmp515=tmp513*tmp514;
tmp516=tmp0+0.78539816f;
tmp517=sin(tmp516);
tmp518=tmp6+(-0.78539816f);
tmp519=sin(tmp518);
tmp520=sin(tmp22);
tmp521=(-1.0f)*tmp517*tmp519*tmp520;
tmp522=tmp515+tmp521;
tmp523=0.017453293f*tmp511*tmp522;
tmp524=tmp509+tmp523;
dresidual3_d_forearm=tmp524;
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
} // ik_leftarmdf()
