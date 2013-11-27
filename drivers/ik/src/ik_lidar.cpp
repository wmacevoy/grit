#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <cassert>
enum { ik_lidarglobal_count=2 };
extern const char *ik_lidarglobal_names[2];

enum { ik_lidarparameter_count=7 };
extern const char *ik_lidarparameter_names[7];

enum { ik_lidarvariable_count=2 };
extern const char *ik_lidarvariable_names[2];

enum { ik_lidarequation_count=2 };
extern const char *ik_lidarequation_names[2];
const char *ik_lidarglobal_names[]={
  "epsilon",
  "steps"
};

const char *ik_lidarparameter_names[]={
  "necklr",
  "neckud",
  "px",
  "py",
  "pz",
  "waist",
  "residual"
};

const char *ik_lidarvariable_names[]={
  "_necklr",
  "_neckud"
};

const char *ik_lidarequation_names[]={
  "residual0",
  "residual1"
};

class ik_lidarStopwatch
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
 float ik_lidardist2(int dim, const float *x, const float *y)
{
  float sum2=0;
  for (int k=0; k<dim; ++k) {
    float dx=x[k]-y[k];
    sum2 += dx*dx;
  }
  return sum2;
} // dist2()
 float ik_lidardist(int dim, const float *x, const float *y)
{
  return sqrt(ik_lidardist2(dim,x,y));
} // dist()
 float ik_lidarnorm2(int dim, const float *x)
{
  float sum2=0;
  for (int k=0; k<dim; ++k) {
    float dx=x[k];
    sum2 += dx*dx;
  }
  return sum2;
} // dist2()
 float ik_lidarnorm(int dim, const float *x)
{
  return sqrt(ik_lidarnorm2(dim,x));
} // dist()
 int ik_lidarDoolittle_LU_Decomposition_with_Pivoting(float *A, int pivot[], int n)
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


 int ik_lidarDoolittle_LU_with_Pivoting_Solve(float *A, float B[], int pivot[],
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

 void ik_lidarlinear_solve(int dim, int *ipiv, float *A, float *b, float *x)
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
  
    ik_lidarDoolittle_LU_Decomposition_with_Pivoting(A,ipiv,dim);
    ik_lidarDoolittle_LU_with_Pivoting_Solve(A,b,ipiv,x,dim);
}
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
void ik_lidarinitialize(
  const float ik_lidarglobals[2],
  const float ik_lidarparameters[7],
  float ik_lidarx[2]
);
void ik_lidarupdate(
  const float ik_lidarglobals[2],
  float ik_lidarparameters[7]
);
// damped newton solver

   void ik_lidarsolve(
    
    const float globals[2],
    float *parameters,
    float *x
  )
  {
    ik_lidarinitialize(globals,parameters,x);
    // newton iteration
    int steps=int(globals[1]);
    float epsilon=globals[0];
    float residual,new_residual;
  
    for (int step=0; step<steps; ++step) {
      float y[2];
      float dx[2];
      float dy[4];
      int iwork[2];
  
      ik_lidarf(globals,parameters,x,y);
      residual = ik_lidarnorm(2,y);
      ik_lidardf(globals,parameters,x,dy);

      ik_lidarlinear_solve(2,iwork,dy,y,dx);
      for (int k=0; k<2; ++k) { x[k] -= dx[k]; }

      for (int damping=0; damping < steps; ++damping) {
	    ik_lidarf(globals,parameters,x,y);
	    new_residual=ik_lidarnorm(2,y);
	    if (new_residual < residual) break;
	    for (int k=0; k<2; ++k) { dx[k] /= 2; }
	    for (int k=0; k<2; ++k) { x[k] += dx[k] ; }
      }
      residual=new_residual;
      if (residual <= epsilon) break;
    }
    parameters[6]=residual;
  } //  ik_lidarsolve()
typedef struct {
  const char *ik_lidarname;
  float ik_lidarglobals[2];
  float ik_lidarparameters[7];
  float ik_lidarx[2];
} ik_lidartest_data_t;

ik_lidartest_data_t ik_lidartest_data[] = {
}; // ik_lidartest_data
void ik_lidartests(const std::string &ik_lidarname)
{
  std::string ik_lidarcase_name;
  std::string ik_lidarmax_x_error_name="none";
  int ik_lidarmax_x_error_count=0;
  float ik_lidarmax_x_error=0;
  float ik_lidarx_error=0;
  std::string ik_lidarmax_y_error_name="none";
  int ik_lidarmax_y_error_count=0;
  float ik_lidarmax_y_error=0;
  float ik_lidary_error=0;

  float ik_lidarx_test[2];
  float ik_lidary_test[2];

  for (int ik_lidarcase_count=0; ik_lidarcase_count < 0; ++ik_lidarcase_count) {
    if (ik_lidarname == "all" || ik_lidarname == ik_lidartest_data[ik_lidarcase_count].ik_lidarname) {
       const char *ik_lidarname=ik_lidartest_data[ik_lidarcase_count].ik_lidarname;
       float *ik_lidarglobals=ik_lidartest_data[ik_lidarcase_count].ik_lidarglobals;
       float *ik_lidarparameters=ik_lidartest_data[ik_lidarcase_count].ik_lidarparameters;
       float *ik_lidarx=ik_lidartest_data[ik_lidarcase_count].ik_lidarx;

       ik_lidarsolve(ik_lidarglobals,ik_lidarparameters,ik_lidarx_test);
       ik_lidarf(ik_lidarglobals,ik_lidarparameters,ik_lidarx_test,ik_lidary_test);
       ik_lidarx_error=ik_lidardist(2,ik_lidarx,ik_lidarx_test);
       ik_lidary_error=ik_lidarnorm(2,ik_lidary_test);

       if (ik_lidarx_error > ik_lidarmax_x_error) {
         ik_lidarmax_x_error=ik_lidarx_error;
         ik_lidarmax_x_error_name=ik_lidarname;
         ik_lidarmax_x_error_count=ik_lidarcase_count;
       }

       if (ik_lidary_error > ik_lidarmax_y_error) {
         ik_lidarmax_y_error=ik_lidary_error;
         ik_lidarmax_y_error_name=ik_lidarname;
         ik_lidarmax_y_error_count=ik_lidarcase_count;
       }

       std::cout << "test " << (ik_lidarcase_count+1) << "/" << "\"" << ik_lidarname << "\"" << ": " << "x_error=" << std::setprecision(15) << ik_lidarx_error << "," << "y_error=" << std::setprecision(15) << ik_lidary_error << std::endl;
     }
   }
   if (ik_lidarname == "all") {
     std::cout << "test summary: " 
       << "max_x_error=" << std::setprecision(15) << ik_lidarmax_x_error
          << " from " << ik_lidarmax_x_error_count << "/" 
          << "\"" << ik_lidarmax_x_error_name << "\""
       << ","
       << "max_y_error=" << std::setprecision(15) << ik_lidarmax_y_error
          << " from " << ik_lidarmax_y_error_count << "/" 
          << "\"" << ik_lidarmax_y_error_name << "\""
       << std::endl;
   }
}
typedef struct {
  const char *ik_lidarname;
  float ik_lidarglobals[2];
  float ik_lidarparameters[7];
} ik_lidarrun_data_t;

ik_lidarrun_data_t ik_lidarrun_data[] = {
}; // ik_lidarrun_data
void ik_lidarruns(const std::string &ik_lidarname)
{
  std::string ik_lidarcase_name;
  for (int ik_lidarcase_count=0; ik_lidarcase_count < 0; ++ik_lidarcase_count) {
    if (ik_lidarname == "all" || ik_lidarname == ik_lidarrun_data[ik_lidarcase_count].ik_lidarname) {
      const char *ik_lidarcase_name=ik_lidarrun_data[ik_lidarcase_count].ik_lidarname;
      float *ik_lidarglobals=ik_lidarrun_data[ik_lidarcase_count].ik_lidarglobals;
      float *ik_lidarparameters=ik_lidarrun_data[ik_lidarcase_count].ik_lidarparameters;
      float ik_lidarx[2];

      ik_lidarsolve(ik_lidarglobals,ik_lidarparameters,ik_lidarx);
      std::cout << "[case] # " << (ik_lidarcase_count+1) << std::endl;
      std::cout << "name=\"" << ik_lidarcase_name << "\"" << std::endl;

      for (int i=0; i<ik_lidarglobal_count; ++i) {
        std::cout << ik_lidarglobal_names[i] << "=" << std::setprecision(15) << ik_lidarglobals[i] << std::endl;
      }

      for (int i=0; i<ik_lidarparameter_count; ++i) {
        std::cout << ik_lidarparameter_names[i] << "=" << std::setprecision(15) << ik_lidarparameters[i] << std::endl;
      }

      for (int i=0; i<ik_lidarvariable_count; ++i) {
        std::cout << ik_lidarvariable_names[i] << "=" << std::setprecision(15) << ik_lidarx[i] << std::endl;
      }
    }
  }
} // ik_lidarruns()
// global aliases
#define epsilon ik_lidarglobals[0]
#define steps ik_lidarglobals[1]

// parameter aliases
#define necklr ik_lidarparameters[0]
#define neckud ik_lidarparameters[1]
#define px ik_lidarparameters[2]
#define py ik_lidarparameters[3]
#define pz ik_lidarparameters[4]
#define waist ik_lidarparameters[5]
#define residual ik_lidarparameters[6]

// variable aliases
#define _necklr ik_lidarx[0]
#define _neckud ik_lidarx[1]

// residual aliases
#define residual0 ik_lidary[0]
#define residual1 ik_lidary[1]

// jacobian aliases (fortran order)
#define dresidual0_d_necklr ik_lidardy[0]
#define dresidual0_d_neckud ik_lidardy[2]
#define dresidual1_d_necklr ik_lidardy[1]
#define dresidual1_d_neckud ik_lidardy[3]

void ik_lidarinitialize(
  const float ik_lidarglobals[2],
  const float ik_lidarparameters[7],
  float ik_lidarx[2]
)
{
// global aliases
#define epsilon ik_lidarglobals[0]
#define steps ik_lidarglobals[1]

// parameter aliases
#define necklr ik_lidarparameters[0]
#define neckud ik_lidarparameters[1]
#define px ik_lidarparameters[2]
#define py ik_lidarparameters[3]
#define pz ik_lidarparameters[4]
#define waist ik_lidarparameters[5]
#define residual ik_lidarparameters[6]

// variable aliases
#define _necklr ik_lidarx[0]
#define _neckud ik_lidarx[1]

// residual aliases
#define residual0 ik_lidary[0]
#define residual1 ik_lidary[1]

// jacobian aliases (fortran order)
#define dresidual0_d_necklr ik_lidardy[0]
#define dresidual0_d_neckud ik_lidardy[2]
#define dresidual1_d_necklr ik_lidardy[1]
#define dresidual1_d_neckud ik_lidardy[3]

  // initialize unknowns from parameters
  // _necklr=necklr;
  _necklr=necklr;
  // _neckud=neckud;
  _neckud=neckud;
// undefine global aliases
#undef epsilon
#undef steps

// undefine parameter aliases
#undef necklr
#undef neckud
#undef px
#undef py
#undef pz
#undef waist
#undef residual

// undefine variable aliases
#undef _necklr
#undef _neckud

// undefine residual aliases
#undef residual0
#undef residual1

// undefine jacobian aliases (fortran order)
#undef dresidual0_d_necklr
#undef dresidual0_d_neckud
#undef dresidual1_d_necklr
#undef dresidual1_d_neckud
} // ik_lidarinitialize()
void ik_lidarupdate(
  const float ik_lidarglobals[2],
  float ik_lidarparameters[7]
)
{
  float ik_lidarx[2];
  ik_lidarsolve(ik_lidarglobals,ik_lidarparameters,ik_lidarx);
// global aliases
#define epsilon ik_lidarglobals[0]
#define steps ik_lidarglobals[1]

// parameter aliases
#define necklr ik_lidarparameters[0]
#define neckud ik_lidarparameters[1]
#define px ik_lidarparameters[2]
#define py ik_lidarparameters[3]
#define pz ik_lidarparameters[4]
#define waist ik_lidarparameters[5]
#define residual ik_lidarparameters[6]

// variable aliases
#define _necklr ik_lidarx[0]
#define _neckud ik_lidarx[1]

// residual aliases
#define residual0 ik_lidary[0]
#define residual1 ik_lidary[1]

// jacobian aliases (fortran order)
#define dresidual0_d_necklr ik_lidardy[0]
#define dresidual0_d_neckud ik_lidardy[2]
#define dresidual1_d_necklr ik_lidardy[1]
#define dresidual1_d_neckud ik_lidardy[3]

  // necklr=_necklr
  necklr=_necklr;
  // neckud=_neckud
  neckud=_neckud;
// undefine global aliases
#undef epsilon
#undef steps

// undefine parameter aliases
#undef necklr
#undef neckud
#undef px
#undef py
#undef pz
#undef waist
#undef residual

// undefine variable aliases
#undef _necklr
#undef _neckud

// undefine residual aliases
#undef residual0
#undef residual1

// undefine jacobian aliases (fortran order)
#undef dresidual0_d_necklr
#undef dresidual0_d_neckud
#undef dresidual1_d_necklr
#undef dresidual1_d_neckud
} // ik_lidarupdate()

void ik_lidarf(
  const float ik_lidarglobals[2],
  const float ik_lidarparameters[7],
  const float ik_lidarx[2],
  float ik_lidary[2]
)
{
// global aliases
#define epsilon ik_lidarglobals[0]
#define steps ik_lidarglobals[1]

// parameter aliases
#define necklr ik_lidarparameters[0]
#define neckud ik_lidarparameters[1]
#define px ik_lidarparameters[2]
#define py ik_lidarparameters[3]
#define pz ik_lidarparameters[4]
#define waist ik_lidarparameters[5]
#define residual ik_lidarparameters[6]

// variable aliases
#define _necklr ik_lidarx[0]
#define _neckud ik_lidarx[1]

// residual aliases
#define residual0 ik_lidary[0]
#define residual1 ik_lidary[1]

// jacobian aliases (fortran order)
#define dresidual0_d_necklr ik_lidardy[0]
#define dresidual0_d_neckud ik_lidardy[2]
#define dresidual1_d_necklr ik_lidardy[1]
#define dresidual1_d_neckud ik_lidardy[3]

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
tmp0=0.017453293f*_necklr;
tmp1=cos(tmp0);
tmp2=tmp1*px;
tmp3=(-0.017453293f)*_neckud;
tmp4=sin(tmp3);
tmp5=sin(tmp0);
tmp6=tmp4*tmp5*pz;
tmp7=cos(tmp3);
tmp8=sin(tmp0);
tmp9=(-1.0f)*tmp7*tmp8*py;
tmp10=tmp2+tmp6+tmp9;
residual0=tmp10;
tmp11=(-0.017453293f)*waist;
tmp12=sin(tmp11);
tmp13=sin(tmp0);
tmp14=tmp12*tmp13*px;
tmp15=cos(tmp3);
tmp16=cos(tmp11);
tmp17=tmp15*tmp16;
tmp18=cos(tmp0);
tmp19=sin(tmp3);
tmp20=sin(tmp11);
tmp21=(-1.0f)*tmp18*tmp19*tmp20;
tmp22=tmp17+tmp21;
tmp23=tmp22*pz;
tmp24=cos(tmp3);
tmp25=cos(tmp0);
tmp26=sin(tmp11);
tmp27=tmp24*tmp25*tmp26;
tmp28=cos(tmp11);
tmp29=sin(tmp3);
tmp30=tmp28*tmp29;
tmp31=tmp27+tmp30;
tmp32=tmp31*py;
tmp33=tmp14+tmp23+tmp32;
residual1=tmp33;
// undefine global aliases
#undef epsilon
#undef steps

// undefine parameter aliases
#undef necklr
#undef neckud
#undef px
#undef py
#undef pz
#undef waist
#undef residual

// undefine variable aliases
#undef _necklr
#undef _neckud

// undefine residual aliases
#undef residual0
#undef residual1

// undefine jacobian aliases (fortran order)
#undef dresidual0_d_necklr
#undef dresidual0_d_neckud
#undef dresidual1_d_necklr
#undef dresidual1_d_neckud
} // ik_lidarf()

void ik_lidardf(
  const float ik_lidarglobals[2],
  const float ik_lidarparameters[7],
  const float ik_lidarx[2],
  float ik_lidardy[4]
)
{
// global aliases
#define epsilon ik_lidarglobals[0]
#define steps ik_lidarglobals[1]

// parameter aliases
#define necklr ik_lidarparameters[0]
#define neckud ik_lidarparameters[1]
#define px ik_lidarparameters[2]
#define py ik_lidarparameters[3]
#define pz ik_lidarparameters[4]
#define waist ik_lidarparameters[5]
#define residual ik_lidarparameters[6]

// variable aliases
#define _necklr ik_lidarx[0]
#define _neckud ik_lidarx[1]

// residual aliases
#define residual0 ik_lidary[0]
#define residual1 ik_lidary[1]

// jacobian aliases (fortran order)
#define dresidual0_d_necklr ik_lidardy[0]
#define dresidual0_d_neckud ik_lidardy[2]
#define dresidual1_d_necklr ik_lidardy[1]
#define dresidual1_d_neckud ik_lidardy[3]

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
tmp0=(-0.017453293f)*_neckud;
tmp1=cos(tmp0);
tmp2=0.017453293f*_necklr;
tmp3=cos(tmp2);
tmp4=(-0.017453293f)*tmp1*tmp3*py;
tmp5=sin(tmp2);
tmp6=(-0.017453293f)*tmp5*px;
tmp7=cos(tmp2);
tmp8=sin(tmp0);
tmp9=0.017453293f*tmp7*tmp8*pz;
tmp10=tmp4+tmp6+tmp9;
dresidual0_d_necklr=tmp10;
tmp11=cos(tmp0);
tmp12=sin(tmp2);
tmp13=(-0.017453293f)*tmp11*tmp12*pz;
tmp14=sin(tmp0);
tmp15=sin(tmp2);
tmp16=(-0.017453293f)*tmp14*tmp15*py;
tmp17=tmp13+tmp16;
dresidual0_d_neckud=tmp17;
tmp18=cos(tmp0);
tmp19=(-0.017453293f)*waist;
tmp20=sin(tmp19);
tmp21=sin(tmp2);
tmp22=(-0.017453293f)*tmp18*tmp20*tmp21*py;
tmp23=cos(tmp2);
tmp24=sin(tmp19);
tmp25=0.017453293f*tmp23*tmp24*px;
tmp26=sin(tmp0);
tmp27=sin(tmp19);
tmp28=sin(tmp2);
tmp29=0.017453293f*tmp26*tmp27*tmp28*pz;
tmp30=tmp22+tmp25+tmp29;
dresidual1_d_necklr=tmp30;
tmp31=cos(tmp0);
tmp32=cos(tmp19);
tmp33=(-0.017453293f)*tmp31*tmp32;
tmp34=cos(tmp2);
tmp35=sin(tmp0);
tmp36=sin(tmp19);
tmp37=0.017453293f*tmp34*tmp35*tmp36;
tmp38=tmp33+tmp37;
tmp39=tmp38*py;
tmp40=cos(tmp0);
tmp41=cos(tmp2);
tmp42=sin(tmp19);
tmp43=0.017453293f*tmp40*tmp41*tmp42;
tmp44=cos(tmp19);
tmp45=sin(tmp0);
tmp46=0.017453293f*tmp44*tmp45;
tmp47=tmp43+tmp46;
tmp48=tmp47*pz;
tmp49=tmp39+tmp48;
dresidual1_d_neckud=tmp49;
// undefine global aliases
#undef epsilon
#undef steps

// undefine parameter aliases
#undef necklr
#undef neckud
#undef px
#undef py
#undef pz
#undef waist
#undef residual

// undefine variable aliases
#undef _necklr
#undef _neckud

// undefine residual aliases
#undef residual0
#undef residual1

// undefine jacobian aliases (fortran order)
#undef dresidual0_d_necklr
#undef dresidual0_d_neckud
#undef dresidual1_d_necklr
#undef dresidual1_d_neckud
} // ik_lidardf()
