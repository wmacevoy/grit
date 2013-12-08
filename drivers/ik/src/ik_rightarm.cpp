#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <cassert>
enum { ik_rightarmglobal_count=2 };
extern const char *ik_rightarmglobal_names[2];

enum { ik_rightarmparameter_count=11 };
extern const char *ik_rightarmparameter_names[11];

enum { ik_rightarmvariable_count=5 };
extern const char *ik_rightarmvariable_names[5];

enum { ik_rightarmequation_count=4 };
extern const char *ik_rightarmequation_names[4];
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
  "residual3"
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
void ik_rightarminitialize(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  float ik_rightarmx[5]
);
void ik_rightarmupdate(
  const float ik_rightarmglobals[2],
  float ik_rightarmparameters[11]
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
      float y[4];
      float dx[5];
      float dy[20];
      float tmp[4];
      float dyt[20];
      float dydyt[16];
      int iwork[4];
  
      ik_rightarmf(globals,parameters,x,y);
      residual = ik_rightarmnorm(4,y);
      ik_rightarmdf(globals,parameters,x,dy);

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

      ik_rightarmlinear_solve(4,iwork,dydyt,y,tmp);

     for (int i=0; i<5; ++i) {
       float sum=0;
       for (int k=0; k<4; ++k) {
          sum += dyt[i+5*k]*tmp[k];
       }
       dx[i]=sum;
     }
      
      
      for (int k=0; k<5; ++k) { x[k] -= dx[k]; }

      for (int damping=0; damping < steps; ++damping) {
	    ik_rightarmf(globals,parameters,x,y);
	    new_residual=ik_rightarmnorm(4,y);
	    if (new_residual < residual) break;
	    for (int k=0; k<5; ++k) { dx[k] /= 2; }
	    for (int k=0; k<5; ++k) { x[k] += dx[k] ; }
      }
      residual=new_residual;
      if (residual <= epsilon) break;
    }
    parameters[10]=residual;
  } //  ik_rightarmsolve()
typedef struct {
  const char *ik_rightarmname;
  float ik_rightarmglobals[2];
  float ik_rightarmparameters[11];
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
  float ik_rightarmy_test[4];

  for (int ik_rightarmcase_count=0; ik_rightarmcase_count < 0; ++ik_rightarmcase_count) {
    if (ik_rightarmname == "all" || ik_rightarmname == ik_rightarmtest_data[ik_rightarmcase_count].ik_rightarmname) {
       const char *ik_rightarmname=ik_rightarmtest_data[ik_rightarmcase_count].ik_rightarmname;
       float *ik_rightarmglobals=ik_rightarmtest_data[ik_rightarmcase_count].ik_rightarmglobals;
       float *ik_rightarmparameters=ik_rightarmtest_data[ik_rightarmcase_count].ik_rightarmparameters;
       float *ik_rightarmx=ik_rightarmtest_data[ik_rightarmcase_count].ik_rightarmx;

       ik_rightarmsolve(ik_rightarmglobals,ik_rightarmparameters,ik_rightarmx_test);
       ik_rightarmf(ik_rightarmglobals,ik_rightarmparameters,ik_rightarmx_test,ik_rightarmy_test);
       ik_rightarmx_error=ik_rightarmdist(5,ik_rightarmx,ik_rightarmx_test);
       ik_rightarmy_error=ik_rightarmnorm(4,ik_rightarmy_test);

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
  float ik_rightarmparameters[11];
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
#define px ik_rightarmparameters[6]
#define py ik_rightarmparameters[7]
#define pz ik_rightarmparameters[8]
#define waist ik_rightarmparameters[9]
#define residual ik_rightarmparameters[10]

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

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_rightarmdy[0]
#define dresidual0_d_shoulderud ik_rightarmdy[4]
#define dresidual0_d_bicep ik_rightarmdy[8]
#define dresidual0_d_elbow ik_rightarmdy[12]
#define dresidual0_d_forearm ik_rightarmdy[16]
#define dresidual1_d_shoulderio ik_rightarmdy[1]
#define dresidual1_d_shoulderud ik_rightarmdy[5]
#define dresidual1_d_bicep ik_rightarmdy[9]
#define dresidual1_d_elbow ik_rightarmdy[13]
#define dresidual1_d_forearm ik_rightarmdy[17]
#define dresidual2_d_shoulderio ik_rightarmdy[2]
#define dresidual2_d_shoulderud ik_rightarmdy[6]
#define dresidual2_d_bicep ik_rightarmdy[10]
#define dresidual2_d_elbow ik_rightarmdy[14]
#define dresidual2_d_forearm ik_rightarmdy[18]
#define dresidual3_d_shoulderio ik_rightarmdy[3]
#define dresidual3_d_shoulderud ik_rightarmdy[7]
#define dresidual3_d_bicep ik_rightarmdy[11]
#define dresidual3_d_elbow ik_rightarmdy[15]
#define dresidual3_d_forearm ik_rightarmdy[19]

void ik_rightarminitialize(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
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
#define px ik_rightarmparameters[6]
#define py ik_rightarmparameters[7]
#define pz ik_rightarmparameters[8]
#define waist ik_rightarmparameters[9]
#define residual ik_rightarmparameters[10]

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

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_rightarmdy[0]
#define dresidual0_d_shoulderud ik_rightarmdy[4]
#define dresidual0_d_bicep ik_rightarmdy[8]
#define dresidual0_d_elbow ik_rightarmdy[12]
#define dresidual0_d_forearm ik_rightarmdy[16]
#define dresidual1_d_shoulderio ik_rightarmdy[1]
#define dresidual1_d_shoulderud ik_rightarmdy[5]
#define dresidual1_d_bicep ik_rightarmdy[9]
#define dresidual1_d_elbow ik_rightarmdy[13]
#define dresidual1_d_forearm ik_rightarmdy[17]
#define dresidual2_d_shoulderio ik_rightarmdy[2]
#define dresidual2_d_shoulderud ik_rightarmdy[6]
#define dresidual2_d_bicep ik_rightarmdy[10]
#define dresidual2_d_elbow ik_rightarmdy[14]
#define dresidual2_d_forearm ik_rightarmdy[18]
#define dresidual3_d_shoulderio ik_rightarmdy[3]
#define dresidual3_d_shoulderud ik_rightarmdy[7]
#define dresidual3_d_bicep ik_rightarmdy[11]
#define dresidual3_d_elbow ik_rightarmdy[15]
#define dresidual3_d_forearm ik_rightarmdy[19]

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
} // ik_rightarminitialize()
void ik_rightarmupdate(
  const float ik_rightarmglobals[2],
  float ik_rightarmparameters[11]
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
#define px ik_rightarmparameters[6]
#define py ik_rightarmparameters[7]
#define pz ik_rightarmparameters[8]
#define waist ik_rightarmparameters[9]
#define residual ik_rightarmparameters[10]

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

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_rightarmdy[0]
#define dresidual0_d_shoulderud ik_rightarmdy[4]
#define dresidual0_d_bicep ik_rightarmdy[8]
#define dresidual0_d_elbow ik_rightarmdy[12]
#define dresidual0_d_forearm ik_rightarmdy[16]
#define dresidual1_d_shoulderio ik_rightarmdy[1]
#define dresidual1_d_shoulderud ik_rightarmdy[5]
#define dresidual1_d_bicep ik_rightarmdy[9]
#define dresidual1_d_elbow ik_rightarmdy[13]
#define dresidual1_d_forearm ik_rightarmdy[17]
#define dresidual2_d_shoulderio ik_rightarmdy[2]
#define dresidual2_d_shoulderud ik_rightarmdy[6]
#define dresidual2_d_bicep ik_rightarmdy[10]
#define dresidual2_d_elbow ik_rightarmdy[14]
#define dresidual2_d_forearm ik_rightarmdy[18]
#define dresidual3_d_shoulderio ik_rightarmdy[3]
#define dresidual3_d_shoulderud ik_rightarmdy[7]
#define dresidual3_d_bicep ik_rightarmdy[11]
#define dresidual3_d_elbow ik_rightarmdy[15]
#define dresidual3_d_forearm ik_rightarmdy[19]

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
} // ik_rightarmupdate()

void ik_rightarmf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  const float ik_rightarmx[5],
  float ik_rightarmy[4]
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
#define px ik_rightarmparameters[6]
#define py ik_rightarmparameters[7]
#define pz ik_rightarmparameters[8]
#define waist ik_rightarmparameters[9]
#define residual ik_rightarmparameters[10]

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

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_rightarmdy[0]
#define dresidual0_d_shoulderud ik_rightarmdy[4]
#define dresidual0_d_bicep ik_rightarmdy[8]
#define dresidual0_d_elbow ik_rightarmdy[12]
#define dresidual0_d_forearm ik_rightarmdy[16]
#define dresidual1_d_shoulderio ik_rightarmdy[1]
#define dresidual1_d_shoulderud ik_rightarmdy[5]
#define dresidual1_d_bicep ik_rightarmdy[9]
#define dresidual1_d_elbow ik_rightarmdy[13]
#define dresidual1_d_forearm ik_rightarmdy[17]
#define dresidual2_d_shoulderio ik_rightarmdy[2]
#define dresidual2_d_shoulderud ik_rightarmdy[6]
#define dresidual2_d_bicep ik_rightarmdy[10]
#define dresidual2_d_elbow ik_rightarmdy[14]
#define dresidual2_d_forearm ik_rightarmdy[18]
#define dresidual3_d_shoulderio ik_rightarmdy[3]
#define dresidual3_d_shoulderud ik_rightarmdy[7]
#define dresidual3_d_bicep ik_rightarmdy[11]
#define dresidual3_d_elbow ik_rightarmdy[15]
#define dresidual3_d_forearm ik_rightarmdy[19]

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
tmp6=0.017453293f*_shoulderio;
tmp7=tmp6+(-0.78539816f);
tmp8=sin(tmp7);
tmp9=(-14.0f)*tmp2*tmp5*tmp8;
tmp10=tmp3+(-0.78539816f);
tmp11=cos(tmp10);
tmp12=tmp6+(-0.78539816f);
tmp13=sin(tmp12);
tmp14=(-8.5f)*tmp11*tmp13;
tmp15=tmp6+(-0.78539816f);
tmp16=sin(tmp15);
tmp17=(-3.625f)*tmp16;
tmp18=(-1.0f)*px;
tmp19=tmp0+0.78539816f;
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
tmp33=14.0f*tmp20*tmp32;
tmp34=tmp9+tmp14+tmp17+tmp18+tmp33+4.875f;
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
tmp45=tmp6+(-0.78539816f);
tmp46=cos(tmp45);
tmp47=cos(tmp35);
tmp48=3.625f*tmp46*tmp47;
tmp49=tmp6+(-0.78539816f);
tmp50=cos(tmp49);
tmp51=tmp3+(-0.78539816f);
tmp52=cos(tmp51);
tmp53=cos(tmp35);
tmp54=8.5f*tmp50*tmp52*tmp53;
tmp55=tmp0+0.78539816f;
tmp56=cos(tmp55);
tmp57=tmp6+(-0.78539816f);
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
tmp71=cos(tmp23);
tmp72=tmp6+(-0.78539816f);
tmp73=cos(tmp72);
tmp74=cos(tmp35);
tmp75=tmp3+(-0.78539816f);
tmp76=sin(tmp75);
tmp77=(-1.0f)*tmp73*tmp74*tmp76;
tmp78=tmp3+(-0.78539816f);
tmp79=cos(tmp78);
tmp80=sin(tmp35);
tmp81=(-1.0f)*tmp79*tmp80;
tmp82=tmp77+tmp81;
tmp83=tmp71*tmp82;
tmp84=cos(tmp35);
tmp85=tmp6+(-0.78539816f);
tmp86=sin(tmp85);
tmp87=sin(tmp23);
tmp88=tmp84*tmp86*tmp87;
tmp89=tmp83+tmp88;
tmp90=14.0f*tmp70*tmp89;
tmp91=tmp37+tmp41+tmp42+tmp44+tmp48+tmp54+tmp68+tmp90;
residual1=tmp91;
tmp92=(-1.0f)*pz;
tmp93=sin(tmp35);
tmp94=2.375f*tmp93;
tmp95=tmp6+(-0.78539816f);
tmp96=cos(tmp95);
tmp97=sin(tmp35);
tmp98=3.625f*tmp96*tmp97;
tmp99=tmp6+(-0.78539816f);
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
tmp111=tmp6+(-0.78539816f);
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
tmp132=tmp6+(-0.78539816f);
tmp133=cos(tmp132);
tmp134=tmp3+(-0.78539816f);
tmp135=sin(tmp134);
tmp136=sin(tmp35);
tmp137=(-1.0f)*tmp133*tmp135*tmp136;
tmp138=tmp131+tmp137;
tmp139=tmp127*tmp138;
tmp140=tmp6+(-0.78539816f);
tmp141=sin(tmp140);
tmp142=sin(tmp23);
tmp143=sin(tmp35);
tmp144=tmp141*tmp142*tmp143;
tmp145=tmp139+tmp144;
tmp146=14.0f*tmp126*tmp145;
tmp147=tmp92+tmp94+tmp98+tmp104+tmp108+tmp122+tmp124+tmp146+12.0f;
residual2=tmp147;
tmp148=(-0.017453293f)*_forearm;
tmp149=tmp148+1.134464f;
tmp150=cos(tmp149);
tmp151=tmp0+0.78539816f;
tmp152=cos(tmp151);
tmp153=tmp6+(-0.78539816f);
tmp154=cos(tmp153);
tmp155=sin(tmp23);
tmp156=tmp154*tmp155;
tmp157=cos(tmp23);
tmp158=tmp6+(-0.78539816f);
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
tmp169=tmp6+(-0.78539816f);
tmp170=sin(tmp169);
tmp171=tmp166*tmp168*tmp170;
tmp172=tmp164+tmp171;
tmp173=tmp150*tmp172;
tmp174=tmp148+1.134464f;
tmp175=sin(tmp174);
tmp176=tmp6+(-0.78539816f);
tmp177=cos(tmp176);
tmp178=cos(tmp23);
tmp179=tmp177*tmp178;
tmp180=tmp6+(-0.78539816f);
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
} // ik_rightarmf()

void ik_rightarmdf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  const float ik_rightarmx[5],
  float ik_rightarmdy[20]
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
#define px ik_rightarmparameters[6]
#define py ik_rightarmparameters[7]
#define pz ik_rightarmparameters[8]
#define waist ik_rightarmparameters[9]
#define residual ik_rightarmparameters[10]

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

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_rightarmdy[0]
#define dresidual0_d_shoulderud ik_rightarmdy[4]
#define dresidual0_d_bicep ik_rightarmdy[8]
#define dresidual0_d_elbow ik_rightarmdy[12]
#define dresidual0_d_forearm ik_rightarmdy[16]
#define dresidual1_d_shoulderio ik_rightarmdy[1]
#define dresidual1_d_shoulderud ik_rightarmdy[5]
#define dresidual1_d_bicep ik_rightarmdy[9]
#define dresidual1_d_elbow ik_rightarmdy[13]
#define dresidual1_d_forearm ik_rightarmdy[17]
#define dresidual2_d_shoulderio ik_rightarmdy[2]
#define dresidual2_d_shoulderud ik_rightarmdy[6]
#define dresidual2_d_bicep ik_rightarmdy[10]
#define dresidual2_d_elbow ik_rightarmdy[14]
#define dresidual2_d_forearm ik_rightarmdy[18]
#define dresidual3_d_shoulderio ik_rightarmdy[3]
#define dresidual3_d_shoulderud ik_rightarmdy[7]
#define dresidual3_d_bicep ik_rightarmdy[11]
#define dresidual3_d_elbow ik_rightarmdy[15]
#define dresidual3_d_forearm ik_rightarmdy[19]

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
tmp0=(-0.017453293f)*_elbow;
tmp1=tmp0+0.78539816f;
tmp2=cos(tmp1);
tmp3=0.017453293f*_shoulderio;
tmp4=tmp3+(-0.78539816f);
tmp5=cos(tmp4);
tmp6=0.017453293f*_shoulderud;
tmp7=tmp6+(-0.78539816f);
tmp8=cos(tmp7);
tmp9=(-0.2443461f)*tmp2*tmp5*tmp8;
tmp10=tmp3+(-0.78539816f);
tmp11=cos(tmp10);
tmp12=tmp6+(-0.78539816f);
tmp13=cos(tmp12);
tmp14=(-0.14835299f)*tmp11*tmp13;
tmp15=tmp3+(-0.78539816f);
tmp16=cos(tmp15);
tmp17=(-0.063268185f)*tmp16;
tmp18=tmp0+0.78539816f;
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
tmp32=14.0f*tmp19*tmp31;
tmp33=tmp9+tmp14+tmp17+tmp32;
dresidual0_d_shoulderio=tmp33;
tmp34=tmp3+(-0.78539816f);
tmp35=sin(tmp34);
tmp36=tmp6+(-0.78539816f);
tmp37=sin(tmp36);
tmp38=0.14835299f*tmp35*tmp37;
tmp39=tmp0+0.78539816f;
tmp40=cos(tmp39);
tmp41=tmp3+(-0.78539816f);
tmp42=sin(tmp41);
tmp43=tmp6+(-0.78539816f);
tmp44=sin(tmp43);
tmp45=0.2443461f*tmp40*tmp42*tmp44;
tmp46=tmp6+(-0.78539816f);
tmp47=cos(tmp46);
tmp48=cos(tmp22);
tmp49=tmp0+0.78539816f;
tmp50=sin(tmp49);
tmp51=tmp3+(-0.78539816f);
tmp52=sin(tmp51);
tmp53=0.2443461f*tmp47*tmp48*tmp50*tmp52;
tmp54=tmp38+tmp45+tmp53;
dresidual0_d_shoulderud=tmp54;
tmp55=tmp0+0.78539816f;
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
tmp68=14.0f*tmp56*tmp67;
dresidual0_d_bicep=tmp68;
tmp69=tmp0+0.78539816f;
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
tmp82=(-0.2443461f)*tmp70*tmp81;
tmp83=tmp6+(-0.78539816f);
tmp84=cos(tmp83);
tmp85=tmp0+0.78539816f;
tmp86=sin(tmp85);
tmp87=tmp3+(-0.78539816f);
tmp88=sin(tmp87);
tmp89=(-0.2443461f)*tmp84*tmp86*tmp88;
tmp90=tmp82+tmp89;
dresidual0_d_elbow=tmp90;
dresidual0_d_forearm=0.0f;
tmp91=tmp0+0.78539816f;
tmp92=cos(tmp91);
tmp93=tmp6+(-0.78539816f);
tmp94=cos(tmp93);
tmp95=(-0.017453293f)*waist;
tmp96=cos(tmp95);
tmp97=tmp3+(-0.78539816f);
tmp98=sin(tmp97);
tmp99=(-0.2443461f)*tmp92*tmp94*tmp96*tmp98;
tmp100=tmp6+(-0.78539816f);
tmp101=cos(tmp100);
tmp102=cos(tmp95);
tmp103=tmp3+(-0.78539816f);
tmp104=sin(tmp103);
tmp105=(-0.14835299f)*tmp101*tmp102*tmp104;
tmp106=cos(tmp95);
tmp107=tmp3+(-0.78539816f);
tmp108=sin(tmp107);
tmp109=(-0.063268185f)*tmp106*tmp108;
tmp110=tmp0+0.78539816f;
tmp111=sin(tmp110);
tmp112=tmp3+(-0.78539816f);
tmp113=cos(tmp112);
tmp114=cos(tmp95);
tmp115=sin(tmp22);
tmp116=0.017453293f*tmp113*tmp114*tmp115;
tmp117=cos(tmp22);
tmp118=cos(tmp95);
tmp119=tmp3+(-0.78539816f);
tmp120=sin(tmp119);
tmp121=tmp6+(-0.78539816f);
tmp122=sin(tmp121);
tmp123=0.017453293f*tmp117*tmp118*tmp120*tmp122;
tmp124=tmp116+tmp123;
tmp125=14.0f*tmp111*tmp124;
tmp126=tmp99+tmp105+tmp109+tmp125;
dresidual1_d_shoulderio=tmp126;
tmp127=tmp3+(-0.78539816f);
tmp128=cos(tmp127);
tmp129=cos(tmp95);
tmp130=tmp6+(-0.78539816f);
tmp131=sin(tmp130);
tmp132=(-0.14835299f)*tmp128*tmp129*tmp131;
tmp133=tmp6+(-0.78539816f);
tmp134=cos(tmp133);
tmp135=sin(tmp95);
tmp136=(-0.14835299f)*tmp134*tmp135;
tmp137=tmp0+0.78539816f;
tmp138=cos(tmp137);
tmp139=tmp3+(-0.78539816f);
tmp140=cos(tmp139);
tmp141=cos(tmp95);
tmp142=tmp6+(-0.78539816f);
tmp143=sin(tmp142);
tmp144=(-0.017453293f)*tmp140*tmp141*tmp143;
tmp145=tmp6+(-0.78539816f);
tmp146=cos(tmp145);
tmp147=sin(tmp95);
tmp148=(-0.017453293f)*tmp146*tmp147;
tmp149=tmp144+tmp148;
tmp150=14.0f*tmp138*tmp149;
tmp151=cos(tmp22);
tmp152=tmp0+0.78539816f;
tmp153=sin(tmp152);
tmp154=tmp3+(-0.78539816f);
tmp155=cos(tmp154);
tmp156=tmp6+(-0.78539816f);
tmp157=cos(tmp156);
tmp158=cos(tmp95);
tmp159=(-0.017453293f)*tmp155*tmp157*tmp158;
tmp160=tmp6+(-0.78539816f);
tmp161=sin(tmp160);
tmp162=sin(tmp95);
tmp163=0.017453293f*tmp161*tmp162;
tmp164=tmp159+tmp163;
tmp165=14.0f*tmp151*tmp153*tmp164;
tmp166=tmp132+tmp136+tmp150+tmp165;
dresidual1_d_shoulderud=tmp166;
tmp167=tmp0+0.78539816f;
tmp168=sin(tmp167);
tmp169=cos(tmp22);
tmp170=cos(tmp95);
tmp171=tmp3+(-0.78539816f);
tmp172=sin(tmp171);
tmp173=(-0.017453293f)*tmp169*tmp170*tmp172;
tmp174=sin(tmp22);
tmp175=tmp3+(-0.78539816f);
tmp176=cos(tmp175);
tmp177=cos(tmp95);
tmp178=tmp6+(-0.78539816f);
tmp179=sin(tmp178);
tmp180=(-1.0f)*tmp176*tmp177*tmp179;
tmp181=tmp6+(-0.78539816f);
tmp182=cos(tmp181);
tmp183=sin(tmp95);
tmp184=(-1.0f)*tmp182*tmp183;
tmp185=tmp180+tmp184;
tmp186=0.017453293f*tmp174*tmp185;
tmp187=tmp173+tmp186;
tmp188=14.0f*tmp168*tmp187;
dresidual1_d_bicep=tmp188;
tmp189=tmp0+0.78539816f;
tmp190=cos(tmp189);
tmp191=cos(tmp22);
tmp192=tmp3+(-0.78539816f);
tmp193=cos(tmp192);
tmp194=cos(tmp95);
tmp195=tmp6+(-0.78539816f);
tmp196=sin(tmp195);
tmp197=(-1.0f)*tmp193*tmp194*tmp196;
tmp198=tmp6+(-0.78539816f);
tmp199=cos(tmp198);
tmp200=sin(tmp95);
tmp201=(-1.0f)*tmp199*tmp200;
tmp202=tmp197+tmp201;
tmp203=tmp191*tmp202;
tmp204=cos(tmp95);
tmp205=tmp3+(-0.78539816f);
tmp206=sin(tmp205);
tmp207=sin(tmp22);
tmp208=tmp204*tmp206*tmp207;
tmp209=tmp203+tmp208;
tmp210=(-0.2443461f)*tmp190*tmp209;
tmp211=tmp0+0.78539816f;
tmp212=sin(tmp211);
tmp213=tmp3+(-0.78539816f);
tmp214=cos(tmp213);
tmp215=tmp6+(-0.78539816f);
tmp216=cos(tmp215);
tmp217=cos(tmp95);
tmp218=tmp214*tmp216*tmp217;
tmp219=tmp6+(-0.78539816f);
tmp220=sin(tmp219);
tmp221=sin(tmp95);
tmp222=(-1.0f)*tmp220*tmp221;
tmp223=tmp218+tmp222;
tmp224=0.2443461f*tmp212*tmp223;
tmp225=tmp210+tmp224;
dresidual1_d_elbow=tmp225;
dresidual1_d_forearm=0.0f;
tmp226=tmp0+0.78539816f;
tmp227=cos(tmp226);
tmp228=tmp6+(-0.78539816f);
tmp229=cos(tmp228);
tmp230=tmp3+(-0.78539816f);
tmp231=sin(tmp230);
tmp232=sin(tmp95);
tmp233=(-0.2443461f)*tmp227*tmp229*tmp231*tmp232;
tmp234=tmp6+(-0.78539816f);
tmp235=cos(tmp234);
tmp236=tmp3+(-0.78539816f);
tmp237=sin(tmp236);
tmp238=sin(tmp95);
tmp239=(-0.14835299f)*tmp235*tmp237*tmp238;
tmp240=tmp3+(-0.78539816f);
tmp241=sin(tmp240);
tmp242=sin(tmp95);
tmp243=(-0.063268185f)*tmp241*tmp242;
tmp244=tmp0+0.78539816f;
tmp245=sin(tmp244);
tmp246=tmp3+(-0.78539816f);
tmp247=cos(tmp246);
tmp248=sin(tmp22);
tmp249=sin(tmp95);
tmp250=0.017453293f*tmp247*tmp248*tmp249;
tmp251=cos(tmp22);
tmp252=tmp3+(-0.78539816f);
tmp253=sin(tmp252);
tmp254=tmp6+(-0.78539816f);
tmp255=sin(tmp254);
tmp256=sin(tmp95);
tmp257=0.017453293f*tmp251*tmp253*tmp255*tmp256;
tmp258=tmp250+tmp257;
tmp259=14.0f*tmp245*tmp258;
tmp260=tmp233+tmp239+tmp243+tmp259;
dresidual2_d_shoulderio=tmp260;
tmp261=tmp3+(-0.78539816f);
tmp262=cos(tmp261);
tmp263=tmp6+(-0.78539816f);
tmp264=sin(tmp263);
tmp265=sin(tmp95);
tmp266=(-0.14835299f)*tmp262*tmp264*tmp265;
tmp267=tmp6+(-0.78539816f);
tmp268=cos(tmp267);
tmp269=cos(tmp95);
tmp270=0.14835299f*tmp268*tmp269;
tmp271=tmp0+0.78539816f;
tmp272=cos(tmp271);
tmp273=tmp3+(-0.78539816f);
tmp274=cos(tmp273);
tmp275=tmp6+(-0.78539816f);
tmp276=sin(tmp275);
tmp277=sin(tmp95);
tmp278=(-0.017453293f)*tmp274*tmp276*tmp277;
tmp279=tmp6+(-0.78539816f);
tmp280=cos(tmp279);
tmp281=cos(tmp95);
tmp282=0.017453293f*tmp280*tmp281;
tmp283=tmp278+tmp282;
tmp284=14.0f*tmp272*tmp283;
tmp285=cos(tmp22);
tmp286=tmp0+0.78539816f;
tmp287=sin(tmp286);
tmp288=tmp3+(-0.78539816f);
tmp289=cos(tmp288);
tmp290=tmp6+(-0.78539816f);
tmp291=cos(tmp290);
tmp292=sin(tmp95);
tmp293=(-0.017453293f)*tmp289*tmp291*tmp292;
tmp294=cos(tmp95);
tmp295=tmp6+(-0.78539816f);
tmp296=sin(tmp295);
tmp297=(-0.017453293f)*tmp294*tmp296;
tmp298=tmp293+tmp297;
tmp299=14.0f*tmp285*tmp287*tmp298;
tmp300=tmp266+tmp270+tmp284+tmp299;
dresidual2_d_shoulderud=tmp300;
tmp301=tmp0+0.78539816f;
tmp302=sin(tmp301);
tmp303=cos(tmp22);
tmp304=tmp3+(-0.78539816f);
tmp305=sin(tmp304);
tmp306=sin(tmp95);
tmp307=(-0.017453293f)*tmp303*tmp305*tmp306;
tmp308=sin(tmp22);
tmp309=tmp6+(-0.78539816f);
tmp310=cos(tmp309);
tmp311=cos(tmp95);
tmp312=tmp310*tmp311;
tmp313=tmp3+(-0.78539816f);
tmp314=cos(tmp313);
tmp315=tmp6+(-0.78539816f);
tmp316=sin(tmp315);
tmp317=sin(tmp95);
tmp318=(-1.0f)*tmp314*tmp316*tmp317;
tmp319=tmp312+tmp318;
tmp320=0.017453293f*tmp308*tmp319;
tmp321=tmp307+tmp320;
tmp322=14.0f*tmp302*tmp321;
dresidual2_d_bicep=tmp322;
tmp323=tmp0+0.78539816f;
tmp324=cos(tmp323);
tmp325=cos(tmp22);
tmp326=tmp6+(-0.78539816f);
tmp327=cos(tmp326);
tmp328=cos(tmp95);
tmp329=tmp327*tmp328;
tmp330=tmp3+(-0.78539816f);
tmp331=cos(tmp330);
tmp332=tmp6+(-0.78539816f);
tmp333=sin(tmp332);
tmp334=sin(tmp95);
tmp335=(-1.0f)*tmp331*tmp333*tmp334;
tmp336=tmp329+tmp335;
tmp337=tmp325*tmp336;
tmp338=tmp3+(-0.78539816f);
tmp339=sin(tmp338);
tmp340=sin(tmp22);
tmp341=sin(tmp95);
tmp342=tmp339*tmp340*tmp341;
tmp343=tmp337+tmp342;
tmp344=(-0.2443461f)*tmp324*tmp343;
tmp345=tmp0+0.78539816f;
tmp346=sin(tmp345);
tmp347=tmp3+(-0.78539816f);
tmp348=cos(tmp347);
tmp349=tmp6+(-0.78539816f);
tmp350=cos(tmp349);
tmp351=sin(tmp95);
tmp352=tmp348*tmp350*tmp351;
tmp353=cos(tmp95);
tmp354=tmp6+(-0.78539816f);
tmp355=sin(tmp354);
tmp356=tmp353*tmp355;
tmp357=tmp352+tmp356;
tmp358=0.2443461f*tmp346*tmp357;
tmp359=tmp344+tmp358;
dresidual2_d_elbow=tmp359;
dresidual2_d_forearm=0.0f;
tmp360=(-0.017453293f)*_forearm;
tmp361=tmp360+1.134464f;
tmp362=cos(tmp361);
tmp363=tmp0+0.78539816f;
tmp364=cos(tmp363);
tmp365=tmp3+(-0.78539816f);
tmp366=sin(tmp365);
tmp367=sin(tmp22);
tmp368=(-0.017453293f)*tmp366*tmp367;
tmp369=tmp3+(-0.78539816f);
tmp370=cos(tmp369);
tmp371=cos(tmp22);
tmp372=tmp6+(-0.78539816f);
tmp373=sin(tmp372);
tmp374=0.017453293f*tmp370*tmp371*tmp373;
tmp375=tmp368+tmp374;
tmp376=tmp364*tmp375;
tmp377=tmp3+(-0.78539816f);
tmp378=cos(tmp377);
tmp379=tmp6+(-0.78539816f);
tmp380=cos(tmp379);
tmp381=tmp0+0.78539816f;
tmp382=sin(tmp381);
tmp383=0.017453293f*tmp378*tmp380*tmp382;
tmp384=tmp376+tmp383;
tmp385=tmp362*tmp384;
tmp386=tmp360+1.134464f;
tmp387=sin(tmp386);
tmp388=tmp3+(-0.78539816f);
tmp389=cos(tmp388);
tmp390=tmp6+(-0.78539816f);
tmp391=sin(tmp390);
tmp392=sin(tmp22);
tmp393=(-0.017453293f)*tmp389*tmp391*tmp392;
tmp394=cos(tmp22);
tmp395=tmp3+(-0.78539816f);
tmp396=sin(tmp395);
tmp397=(-0.017453293f)*tmp394*tmp396;
tmp398=tmp393+tmp397;
tmp399=tmp387*tmp398;
tmp400=tmp385+tmp399;
dresidual3_d_shoulderio=tmp400;
tmp401=tmp360+1.134464f;
tmp402=cos(tmp401);
tmp403=tmp0+0.78539816f;
tmp404=sin(tmp403);
tmp405=tmp3+(-0.78539816f);
tmp406=sin(tmp405);
tmp407=tmp6+(-0.78539816f);
tmp408=sin(tmp407);
tmp409=(-0.017453293f)*tmp404*tmp406*tmp408;
tmp410=tmp0+0.78539816f;
tmp411=cos(tmp410);
tmp412=tmp6+(-0.78539816f);
tmp413=cos(tmp412);
tmp414=cos(tmp22);
tmp415=tmp3+(-0.78539816f);
tmp416=sin(tmp415);
tmp417=0.017453293f*tmp411*tmp413*tmp414*tmp416;
tmp418=tmp409+tmp417;
tmp419=tmp402*tmp418;
tmp420=tmp6+(-0.78539816f);
tmp421=cos(tmp420);
tmp422=tmp360+1.134464f;
tmp423=sin(tmp422);
tmp424=tmp3+(-0.78539816f);
tmp425=sin(tmp424);
tmp426=sin(tmp22);
tmp427=(-0.017453293f)*tmp421*tmp423*tmp425*tmp426;
tmp428=tmp419+tmp427;
dresidual3_d_shoulderud=tmp428;
tmp429=tmp0+0.78539816f;
tmp430=cos(tmp429);
tmp431=tmp360+1.134464f;
tmp432=cos(tmp431);
tmp433=tmp3+(-0.78539816f);
tmp434=cos(tmp433);
tmp435=cos(tmp22);
tmp436=(-0.017453293f)*tmp434*tmp435;
tmp437=tmp3+(-0.78539816f);
tmp438=sin(tmp437);
tmp439=tmp6+(-0.78539816f);
tmp440=sin(tmp439);
tmp441=sin(tmp22);
tmp442=0.017453293f*tmp438*tmp440*tmp441;
tmp443=tmp436+tmp442;
tmp444=tmp430*tmp432*tmp443;
tmp445=tmp360+1.134464f;
tmp446=sin(tmp445);
tmp447=tmp3+(-0.78539816f);
tmp448=cos(tmp447);
tmp449=sin(tmp22);
tmp450=0.017453293f*tmp448*tmp449;
tmp451=cos(tmp22);
tmp452=tmp3+(-0.78539816f);
tmp453=sin(tmp452);
tmp454=tmp6+(-0.78539816f);
tmp455=sin(tmp454);
tmp456=0.017453293f*tmp451*tmp453*tmp455;
tmp457=tmp450+tmp456;
tmp458=tmp446*tmp457;
tmp459=tmp444+tmp458;
dresidual3_d_bicep=tmp459;
tmp460=tmp360+1.134464f;
tmp461=cos(tmp460);
tmp462=tmp0+0.78539816f;
tmp463=cos(tmp462);
tmp464=tmp6+(-0.78539816f);
tmp465=cos(tmp464);
tmp466=tmp3+(-0.78539816f);
tmp467=sin(tmp466);
tmp468=(-0.017453293f)*tmp463*tmp465*tmp467;
tmp469=tmp0+0.78539816f;
tmp470=sin(tmp469);
tmp471=tmp3+(-0.78539816f);
tmp472=cos(tmp471);
tmp473=sin(tmp22);
tmp474=tmp472*tmp473;
tmp475=cos(tmp22);
tmp476=tmp3+(-0.78539816f);
tmp477=sin(tmp476);
tmp478=tmp6+(-0.78539816f);
tmp479=sin(tmp478);
tmp480=tmp475*tmp477*tmp479;
tmp481=tmp474+tmp480;
tmp482=0.017453293f*tmp470*tmp481;
tmp483=tmp468+tmp482;
tmp484=tmp461*tmp483;
dresidual3_d_elbow=tmp484;
tmp485=tmp360+1.134464f;
tmp486=cos(tmp485);
tmp487=tmp3+(-0.78539816f);
tmp488=cos(tmp487);
tmp489=cos(tmp22);
tmp490=tmp488*tmp489;
tmp491=tmp3+(-0.78539816f);
tmp492=sin(tmp491);
tmp493=tmp6+(-0.78539816f);
tmp494=sin(tmp493);
tmp495=sin(tmp22);
tmp496=(-1.0f)*tmp492*tmp494*tmp495;
tmp497=tmp490+tmp496;
tmp498=(-0.017453293f)*tmp486*tmp497;
tmp499=tmp360+1.134464f;
tmp500=sin(tmp499);
tmp501=tmp0+0.78539816f;
tmp502=cos(tmp501);
tmp503=tmp3+(-0.78539816f);
tmp504=cos(tmp503);
tmp505=sin(tmp22);
tmp506=tmp504*tmp505;
tmp507=cos(tmp22);
tmp508=tmp3+(-0.78539816f);
tmp509=sin(tmp508);
tmp510=tmp6+(-0.78539816f);
tmp511=sin(tmp510);
tmp512=tmp507*tmp509*tmp511;
tmp513=tmp506+tmp512;
tmp514=tmp502*tmp513;
tmp515=tmp6+(-0.78539816f);
tmp516=cos(tmp515);
tmp517=tmp0+0.78539816f;
tmp518=sin(tmp517);
tmp519=tmp3+(-0.78539816f);
tmp520=sin(tmp519);
tmp521=tmp516*tmp518*tmp520;
tmp522=tmp514+tmp521;
tmp523=0.017453293f*tmp500*tmp522;
tmp524=tmp498+tmp523;
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
} // ik_rightarmdf()
