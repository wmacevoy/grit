#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <cassert>
enum { ik_tipglobal_count=3 };
extern const char *ik_tipglobal_names[3];

enum { ik_tipparameter_count=14 };
extern const char *ik_tipparameter_names[14];

enum { ik_tipvariable_count=3 };
extern const char *ik_tipvariable_names[3];

enum { ik_tipequation_count=3 };
extern const char *ik_tipequation_names[3];
const char *ik_tipglobal_names[]={
  "t",
  "epsilon",
  "steps"
};

const char *ik_tipparameter_names[]={
  "aknee",
  "afemur",
  "ahip",
  "achasis",
  "dfemur2knee",
  "dhip2femur",
  "drchasis2hip",
  "dyknee2tip",
  "dzchasis2hip",
  "dzknee2tip",
  "px",
  "py",
  "pz",
  "residual"
};

const char *ik_tipvariable_names[]={
  "aknee_",
  "afemur_",
  "ahip_"
};

const char *ik_tipequation_names[]={
  "residual0",
  "residual1",
  "residual2"
};

class ik_tipStopwatch
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
 float ik_tipdist2(int dim, const float *x, const float *y)
{
  float sum2=0;
  for (int k=0; k<dim; ++k) {
    float dx=x[k]-y[k];
    sum2 += dx*dx;
  }
  return sum2;
} // dist2()
 float ik_tipdist(int dim, const float *x, const float *y)
{
  return sqrt(ik_tipdist2(dim,x,y));
} // dist()
 float ik_tipnorm2(int dim, const float *x)
{
  float sum2=0;
  for (int k=0; k<dim; ++k) {
    float dx=x[k];
    sum2 += dx*dx;
  }
  return sum2;
} // dist2()
 float ik_tipnorm(int dim, const float *x)
{
  return sqrt(ik_tipnorm2(dim,x));
} // dist()
 int ik_tipDoolittle_LU_Decomposition_with_Pivoting(float *A, int pivot[], int n)
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


 int ik_tipDoolittle_LU_with_Pivoting_Solve(float *A, float B[], int pivot[],
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

 void ik_tiplinear_solve(int dim, int *ipiv, float *A, float *b, float *x)
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
  
    ik_tipDoolittle_LU_Decomposition_with_Pivoting(A,ipiv,dim);
    ik_tipDoolittle_LU_with_Pivoting_Solve(A,b,ipiv,x,dim);
}
void ik_tipf(
  const float ik_tipglobals[3],
  const float ik_tipparameters[14],
  const float ik_tipx[3],
  float ik_tipy[3]
);
void ik_tipdf(
  const float ik_tipglobals[3],
  const float ik_tipparameters[14],
  const float ik_tipx[3],
  float ik_tipdy[9]
);
void ik_tipinitialize(
  const float ik_tipglobals[3],
  const float ik_tipparameters[14],
  float ik_tipx[3]
);
void ik_tipupdate(
  const float ik_tipglobals[3],
  float ik_tipparameters[14]
);
// damped newton solver

   void ik_tipsolve(
    
    const float globals[3],
    float *parameters,
    float *x
  )
  {
    ik_tipinitialize(globals,parameters,x);
    // newton iteration
    int steps=int(globals[2]);
    float epsilon=globals[1];
    float residual,new_residual;
  
    for (int step=0; step<steps; ++step) {
      float y[3];
      float dx[3];
      float dy[9];
      int iwork[3];
  
      ik_tipf(globals,parameters,x,y);
      residual = ik_tipnorm(3,y);
      ik_tipdf(globals,parameters,x,dy);

      ik_tiplinear_solve(3,iwork,dy,y,dx);
      for (int k=0; k<3; ++k) { x[k] -= dx[k]; }

      for (int damping=0; damping < steps; ++damping) {
	    ik_tipf(globals,parameters,x,y);
	    new_residual=ik_tipnorm(3,y);
	    if (new_residual < residual) break;
	    for (int k=0; k<3; ++k) { dx[k] /= 2; }
	    for (int k=0; k<3; ++k) { x[k] += dx[k] ; }
      }
      residual=new_residual;
      if (residual <= epsilon) break;
    }
    parameters[13]=residual;
  } //  ik_tipsolve()
typedef struct {
  const char *ik_tipname;
  float ik_tipglobals[3];
  float ik_tipparameters[14];
  float ik_tipx[3];
} ik_tiptest_data_t;

ik_tiptest_data_t ik_tiptest_data[] = {
}; // ik_tiptest_data
void ik_tiptests(const std::string &ik_tipname)
{
  std::string ik_tipcase_name;
  std::string ik_tipmax_x_error_name="none";
  int ik_tipmax_x_error_count=0;
  float ik_tipmax_x_error=0;
  float ik_tipx_error=0;
  std::string ik_tipmax_y_error_name="none";
  int ik_tipmax_y_error_count=0;
  float ik_tipmax_y_error=0;
  float ik_tipy_error=0;

  float ik_tipx_test[3];
  float ik_tipy_test[3];

  for (int ik_tipcase_count=0; ik_tipcase_count < 0; ++ik_tipcase_count) {
    if (ik_tipname == "all" || ik_tipname == ik_tiptest_data[ik_tipcase_count].ik_tipname) {
       const char *ik_tipname=ik_tiptest_data[ik_tipcase_count].ik_tipname;
       float *ik_tipglobals=ik_tiptest_data[ik_tipcase_count].ik_tipglobals;
       float *ik_tipparameters=ik_tiptest_data[ik_tipcase_count].ik_tipparameters;
       float *ik_tipx=ik_tiptest_data[ik_tipcase_count].ik_tipx;

       ik_tipsolve(ik_tipglobals,ik_tipparameters,ik_tipx_test);
       ik_tipf(ik_tipglobals,ik_tipparameters,ik_tipx_test,ik_tipy_test);
       ik_tipx_error=ik_tipdist(3,ik_tipx,ik_tipx_test);
       ik_tipy_error=ik_tipnorm(3,ik_tipy_test);

       if (ik_tipx_error > ik_tipmax_x_error) {
         ik_tipmax_x_error=ik_tipx_error;
         ik_tipmax_x_error_name=ik_tipname;
         ik_tipmax_x_error_count=ik_tipcase_count;
       }

       if (ik_tipy_error > ik_tipmax_y_error) {
         ik_tipmax_y_error=ik_tipy_error;
         ik_tipmax_y_error_name=ik_tipname;
         ik_tipmax_y_error_count=ik_tipcase_count;
       }

       std::cout << "test " << (ik_tipcase_count+1) << "/" << "\"" << ik_tipname << "\"" << ": " << "x_error=" << std::setprecision(15) << ik_tipx_error << "," << "y_error=" << std::setprecision(15) << ik_tipy_error << std::endl;
     }
   }
   if (ik_tipname == "all") {
     std::cout << "test summary: " 
       << "max_x_error=" << std::setprecision(15) << ik_tipmax_x_error
          << " from " << ik_tipmax_x_error_count << "/" 
          << "\"" << ik_tipmax_x_error_name << "\""
       << ","
       << "max_y_error=" << std::setprecision(15) << ik_tipmax_y_error
          << " from " << ik_tipmax_y_error_count << "/" 
          << "\"" << ik_tipmax_y_error_name << "\""
       << std::endl;
   }
}
typedef struct {
  const char *ik_tipname;
  float ik_tipglobals[3];
  float ik_tipparameters[14];
} ik_tiprun_data_t;

ik_tiprun_data_t ik_tiprun_data[] = {
}; // ik_tiprun_data
void ik_tipruns(const std::string &ik_tipname)
{
  std::string ik_tipcase_name;
  for (int ik_tipcase_count=0; ik_tipcase_count < 0; ++ik_tipcase_count) {
    if (ik_tipname == "all" || ik_tipname == ik_tiprun_data[ik_tipcase_count].ik_tipname) {
      const char *ik_tipcase_name=ik_tiprun_data[ik_tipcase_count].ik_tipname;
      float *ik_tipglobals=ik_tiprun_data[ik_tipcase_count].ik_tipglobals;
      float *ik_tipparameters=ik_tiprun_data[ik_tipcase_count].ik_tipparameters;
      float ik_tipx[3];

      ik_tipsolve(ik_tipglobals,ik_tipparameters,ik_tipx);
      std::cout << "[case] # " << (ik_tipcase_count+1) << std::endl;
      std::cout << "name=\"" << ik_tipcase_name << "\"" << std::endl;

      for (int i=0; i<ik_tipglobal_count; ++i) {
        std::cout << ik_tipglobal_names[i] << "=" << std::setprecision(15) << ik_tipglobals[i] << std::endl;
      }

      for (int i=0; i<ik_tipparameter_count; ++i) {
        std::cout << ik_tipparameter_names[i] << "=" << std::setprecision(15) << ik_tipparameters[i] << std::endl;
      }

      for (int i=0; i<ik_tipvariable_count; ++i) {
        std::cout << ik_tipvariable_names[i] << "=" << std::setprecision(15) << ik_tipx[i] << std::endl;
      }
    }
  }
} // ik_tipruns()
// global aliases
#define t ik_tipglobals[0]
#define epsilon ik_tipglobals[1]
#define steps ik_tipglobals[2]

// parameter aliases
#define aknee ik_tipparameters[0]
#define afemur ik_tipparameters[1]
#define ahip ik_tipparameters[2]
#define achasis ik_tipparameters[3]
#define dfemur2knee ik_tipparameters[4]
#define dhip2femur ik_tipparameters[5]
#define drchasis2hip ik_tipparameters[6]
#define dyknee2tip ik_tipparameters[7]
#define dzchasis2hip ik_tipparameters[8]
#define dzknee2tip ik_tipparameters[9]
#define px ik_tipparameters[10]
#define py ik_tipparameters[11]
#define pz ik_tipparameters[12]
#define residual ik_tipparameters[13]

// variable aliases
#define aknee_ ik_tipx[0]
#define afemur_ ik_tipx[1]
#define ahip_ ik_tipx[2]

// residual aliases
#define residual0 ik_tipy[0]
#define residual1 ik_tipy[1]
#define residual2 ik_tipy[2]

// jacobian aliases (fortran order)
#define dresidual0_daknee_ ik_tipdy[0]
#define dresidual0_dafemur_ ik_tipdy[3]
#define dresidual0_dahip_ ik_tipdy[6]
#define dresidual1_daknee_ ik_tipdy[1]
#define dresidual1_dafemur_ ik_tipdy[4]
#define dresidual1_dahip_ ik_tipdy[7]
#define dresidual2_daknee_ ik_tipdy[2]
#define dresidual2_dafemur_ ik_tipdy[5]
#define dresidual2_dahip_ ik_tipdy[8]

void ik_tipinitialize(
  const float ik_tipglobals[3],
  const float ik_tipparameters[14],
  float ik_tipx[3]
)
{
  // initialize unknowns from parameters
  // aknee_=aknee;
  aknee_=aknee;
  // afemur_=afemur;
  afemur_=afemur;
  // ahip_=ahip;
  ahip_=ahip;
} // ik_tipinitialize()
void ik_tipupdate(
  const float ik_tipglobals[3],
  float ik_tipparameters[14]
)
{
  float ik_tipx[3];
  ik_tipsolve(ik_tipglobals,ik_tipparameters,ik_tipx);

  // aknee=aknee_
  aknee=aknee_;
  // afemur=afemur_
  afemur=afemur_;
  // ahip=ahip_
  ahip=ahip_;
} // ik_tipupdate()

void ik_tipf(
  const float ik_tipglobals[3],
  const float ik_tipparameters[14],
  const float ik_tipx[3],
  float ik_tipy[3]
)
{
  // residual0=cos(afemur_)*((-1.0)*cos(achasis)*sin(ahip_)+(-1.0)*cos(ahip_)*sin(achasis))*dfemur2knee+(cos(afemur_)*cos(aknee_)*((-1.0)*cos(achasis)*sin(ahip_)+(-1.0)*cos(ahip_)*sin(achasis))+(-1.0)*sin(afemur_)*sin(aknee_)*((-1.0)*cos(achasis)*sin(ahip_)+(-1.0)*cos(ahip_)*sin(achasis)))*dyknee2tip+((-1.0)*cos(achasis)*sin(ahip_)+(-1.0)*cos(ahip_)*sin(achasis))*dhip2femur+((-1.0)*cos(afemur_)*sin(aknee_)*((-1.0)*cos(achasis)*sin(ahip_)+(-1.0)*cos(ahip_)*sin(achasis))+(-1.0)*cos(aknee_)*sin(afemur_)*((-1.0)*cos(achasis)*sin(ahip_)+(-1.0)*cos(ahip_)*sin(achasis)))*dzknee2tip+(-1.0)*sin(achasis)*drchasis2hip+(-1.0)*px;
  residual0=cos(afemur_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))*dfemur2knee+(cos(afemur_)*cos(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))+(-1.0f)*sin(afemur_)*sin(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis)))*dyknee2tip+((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))*dhip2femur+((-1.0f)*cos(afemur_)*sin(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))+(-1.0f)*cos(aknee_)*sin(afemur_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis)))*dzknee2tip+(-1.0f)*sin(achasis)*drchasis2hip+(-1.0f)*px;
  // residual1=cos(achasis)*drchasis2hip+cos(afemur_)*(cos(achasis)*cos(ahip_)+(-1.0)*sin(achasis)*sin(ahip_))*dfemur2knee+(cos(achasis)*cos(ahip_)+(-1.0)*sin(achasis)*sin(ahip_))*dhip2femur+(cos(afemur_)*cos(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0)*sin(achasis)*sin(ahip_))+(-1.0)*sin(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0)*sin(achasis)*sin(ahip_)))*dyknee2tip+((-1.0)*cos(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0)*sin(achasis)*sin(ahip_))+(-1.0)*cos(aknee_)*sin(afemur_)*(cos(achasis)*cos(ahip_)+(-1.0)*sin(achasis)*sin(ahip_)))*dzknee2tip+(-1.0)*py;
  residual1=cos(achasis)*drchasis2hip+cos(afemur_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_))*dfemur2knee+(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_))*dhip2femur+(cos(afemur_)*cos(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_))+(-1.0f)*sin(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_)))*dyknee2tip+((-1.0f)*cos(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_))+(-1.0f)*cos(aknee_)*sin(afemur_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_)))*dzknee2tip+(-1.0f)*py;
  // residual2=sin(afemur_)*dfemur2knee+(cos(afemur_)*cos(aknee_)+(-1.0)*sin(afemur_)*sin(aknee_))*dzknee2tip+(cos(afemur_)*sin(aknee_)+cos(aknee_)*sin(afemur_))*dyknee2tip+(-1.0)*pz+dzchasis2hip;
  residual2=sin(afemur_)*dfemur2knee+(cos(afemur_)*cos(aknee_)+(-1.0f)*sin(afemur_)*sin(aknee_))*dzknee2tip+(cos(afemur_)*sin(aknee_)+cos(aknee_)*sin(afemur_))*dyknee2tip+(-1.0f)*pz+dzchasis2hip;
} // ik_tipf()

void ik_tipdf(
  const float ik_tipglobals[3],
  const float ik_tipparameters[14],
  const float ik_tipx[3],
  float ik_tipdy[9]
)
{
  dresidual0_daknee_=(sin(afemur_)*sin(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))+(-1.0f)*cos(afemur_)*cos(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis)))*dzknee2tip+((-1.0f)*cos(afemur_)*sin(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))+(-1.0f)*cos(aknee_)*sin(afemur_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis)))*dyknee2tip;
  dresidual0_dafemur_=(sin(afemur_)*sin(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))+(-1.0f)*cos(afemur_)*cos(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis)))*dzknee2tip+((-1.0f)*cos(afemur_)*sin(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))+(-1.0f)*cos(aknee_)*sin(afemur_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis)))*dyknee2tip+(-1.0f)*sin(afemur_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))*dfemur2knee;
  dresidual0_dahip_=cos(afemur_)*(sin(achasis)*sin(ahip_)+(-1.0f)*cos(achasis)*cos(ahip_))*dfemur2knee+(cos(afemur_)*cos(aknee_)*(sin(achasis)*sin(ahip_)+(-1.0f)*cos(achasis)*cos(ahip_))+(-1.0f)*sin(afemur_)*sin(aknee_)*(sin(achasis)*sin(ahip_)+(-1.0f)*cos(achasis)*cos(ahip_)))*dyknee2tip+(sin(achasis)*sin(ahip_)+(-1.0f)*cos(achasis)*cos(ahip_))*dhip2femur+((-1.0f)*cos(afemur_)*sin(aknee_)*(sin(achasis)*sin(ahip_)+(-1.0f)*cos(achasis)*cos(ahip_))+(-1.0f)*cos(aknee_)*sin(afemur_)*(sin(achasis)*sin(ahip_)+(-1.0f)*cos(achasis)*cos(ahip_)))*dzknee2tip;
  dresidual1_daknee_=(sin(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_))+(-1.0f)*cos(afemur_)*cos(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_)))*dzknee2tip+((-1.0f)*cos(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_))+(-1.0f)*cos(aknee_)*sin(afemur_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_)))*dyknee2tip;
  dresidual1_dafemur_=(sin(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_))+(-1.0f)*cos(afemur_)*cos(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_)))*dzknee2tip+((-1.0f)*cos(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_))+(-1.0f)*cos(aknee_)*sin(afemur_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_)))*dyknee2tip+(-1.0f)*sin(afemur_)*(cos(achasis)*cos(ahip_)+(-1.0f)*sin(achasis)*sin(ahip_))*dfemur2knee;
  dresidual1_dahip_=cos(afemur_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))*dfemur2knee+(cos(afemur_)*cos(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))+(-1.0f)*sin(afemur_)*sin(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis)))*dyknee2tip+((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))*dhip2femur+((-1.0f)*cos(afemur_)*sin(aknee_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis))+(-1.0f)*cos(aknee_)*sin(afemur_)*((-1.0f)*cos(achasis)*sin(ahip_)+(-1.0f)*cos(ahip_)*sin(achasis)))*dzknee2tip;
  dresidual2_daknee_=(cos(afemur_)*cos(aknee_)+(-1.0f)*sin(afemur_)*sin(aknee_))*dyknee2tip+((-1.0f)*cos(afemur_)*sin(aknee_)+(-1.0f)*cos(aknee_)*sin(afemur_))*dzknee2tip;
  dresidual2_dafemur_=cos(afemur_)*dfemur2knee+(cos(afemur_)*cos(aknee_)+(-1.0f)*sin(afemur_)*sin(aknee_))*dyknee2tip+((-1.0f)*cos(afemur_)*sin(aknee_)+(-1.0f)*cos(aknee_)*sin(afemur_))*dzknee2tip;
  dresidual2_dahip_=0.0f;
} // ik_tipdf()
