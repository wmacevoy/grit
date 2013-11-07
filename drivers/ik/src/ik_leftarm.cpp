#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <cassert>
enum { ik_leftarmglobal_count=2 };
extern const char *ik_leftarmglobal_names[2];

enum { ik_leftarmparameter_count=16 };
extern const char *ik_leftarmparameter_names[16];

enum { ik_leftarmvariable_count=5 };
extern const char *ik_leftarmvariable_names[5];

enum { ik_leftarmequation_count=5 };
extern const char *ik_leftarmequation_names[5];
const char *ik_leftarmglobal_names[]={
  "epsilon",
  "steps"
};

const char *ik_leftarmparameter_names[]={
  "LEFT_SHOULDER_IO",
  "LEFT_SHOULDER_UD",
  "LEFT_BICEP_ROTATE",
  "LEFT_ELBOW",
  "LEFT_FOREARM_ROTATE",
  "LEFTARM_BICEP_ROTATE",
  "LEFTARM_ELBOW",
  "LEFTARM_FOREARM_ROTATE",
  "LEFTARM_SHOULDER_IO",
  "LEFTARM_SHOULDER_UD",
  "nx",
  "ny",
  "px",
  "py",
  "pz",
  "residual"
};

const char *ik_leftarmvariable_names[]={
  "LEFT_SHOULDER_IO_",
  "LEFT_SHOULDER_UD_",
  "LEFT_BICEP_ROTATE_",
  "LEFT_ELBOW_",
  "LEFT_FOREARM_ROTATE_"
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
  const float ik_leftarmparameters[16],
  const float ik_leftarmx[5],
  float ik_leftarmy[5]
);
void ik_leftarmdf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[16],
  const float ik_leftarmx[5],
  float ik_leftarmdy[25]
);
void ik_leftarminitialize(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[16],
  float ik_leftarmx[5]
);
void ik_leftarmupdate(
  const float ik_leftarmglobals[2],
  float ik_leftarmparameters[16]
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
    parameters[15]=residual;
  } //  ik_leftarmsolve()
typedef struct {
  const char *ik_leftarmname;
  float ik_leftarmglobals[2];
  float ik_leftarmparameters[16];
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
  float ik_leftarmparameters[16];
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
#define LEFT_SHOULDER_IO ik_leftarmparameters[0]
#define LEFT_SHOULDER_UD ik_leftarmparameters[1]
#define LEFT_BICEP_ROTATE ik_leftarmparameters[2]
#define LEFT_ELBOW ik_leftarmparameters[3]
#define LEFT_FOREARM_ROTATE ik_leftarmparameters[4]
#define LEFTARM_BICEP_ROTATE ik_leftarmparameters[5]
#define LEFTARM_ELBOW ik_leftarmparameters[6]
#define LEFTARM_FOREARM_ROTATE ik_leftarmparameters[7]
#define LEFTARM_SHOULDER_IO ik_leftarmparameters[8]
#define LEFTARM_SHOULDER_UD ik_leftarmparameters[9]
#define nx ik_leftarmparameters[10]
#define ny ik_leftarmparameters[11]
#define px ik_leftarmparameters[12]
#define py ik_leftarmparameters[13]
#define pz ik_leftarmparameters[14]
#define residual ik_leftarmparameters[15]

// variable aliases
#define LEFT_SHOULDER_IO_ ik_leftarmx[0]
#define LEFT_SHOULDER_UD_ ik_leftarmx[1]
#define LEFT_BICEP_ROTATE_ ik_leftarmx[2]
#define LEFT_ELBOW_ ik_leftarmx[3]
#define LEFT_FOREARM_ROTATE_ ik_leftarmx[4]

// residual aliases
#define residual0 ik_leftarmy[0]
#define residual1 ik_leftarmy[1]
#define residual2 ik_leftarmy[2]
#define residual3 ik_leftarmy[3]
#define residual4 ik_leftarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_dLEFT_SHOULDER_IO_ ik_leftarmdy[0]
#define dresidual0_dLEFT_SHOULDER_UD_ ik_leftarmdy[5]
#define dresidual0_dLEFT_BICEP_ROTATE_ ik_leftarmdy[10]
#define dresidual0_dLEFT_ELBOW_ ik_leftarmdy[15]
#define dresidual0_dLEFT_FOREARM_ROTATE_ ik_leftarmdy[20]
#define dresidual1_dLEFT_SHOULDER_IO_ ik_leftarmdy[1]
#define dresidual1_dLEFT_SHOULDER_UD_ ik_leftarmdy[6]
#define dresidual1_dLEFT_BICEP_ROTATE_ ik_leftarmdy[11]
#define dresidual1_dLEFT_ELBOW_ ik_leftarmdy[16]
#define dresidual1_dLEFT_FOREARM_ROTATE_ ik_leftarmdy[21]
#define dresidual2_dLEFT_SHOULDER_IO_ ik_leftarmdy[2]
#define dresidual2_dLEFT_SHOULDER_UD_ ik_leftarmdy[7]
#define dresidual2_dLEFT_BICEP_ROTATE_ ik_leftarmdy[12]
#define dresidual2_dLEFT_ELBOW_ ik_leftarmdy[17]
#define dresidual2_dLEFT_FOREARM_ROTATE_ ik_leftarmdy[22]
#define dresidual3_dLEFT_SHOULDER_IO_ ik_leftarmdy[3]
#define dresidual3_dLEFT_SHOULDER_UD_ ik_leftarmdy[8]
#define dresidual3_dLEFT_BICEP_ROTATE_ ik_leftarmdy[13]
#define dresidual3_dLEFT_ELBOW_ ik_leftarmdy[18]
#define dresidual3_dLEFT_FOREARM_ROTATE_ ik_leftarmdy[23]
#define dresidual4_dLEFT_SHOULDER_IO_ ik_leftarmdy[4]
#define dresidual4_dLEFT_SHOULDER_UD_ ik_leftarmdy[9]
#define dresidual4_dLEFT_BICEP_ROTATE_ ik_leftarmdy[14]
#define dresidual4_dLEFT_ELBOW_ ik_leftarmdy[19]
#define dresidual4_dLEFT_FOREARM_ROTATE_ ik_leftarmdy[24]

void ik_leftarminitialize(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[16],
  float ik_leftarmx[5]
)
{
  // initialize unknowns from parameters
  // LEFT_SHOULDER_IO_=LEFT_SHOULDER_IO;
  LEFT_SHOULDER_IO_=LEFT_SHOULDER_IO;
  // LEFT_SHOULDER_UD_=LEFT_SHOULDER_UD;
  LEFT_SHOULDER_UD_=LEFT_SHOULDER_UD;
  // LEFT_BICEP_ROTATE_=LEFT_BICEP_ROTATE;
  LEFT_BICEP_ROTATE_=LEFT_BICEP_ROTATE;
  // LEFT_ELBOW_=LEFT_ELBOW;
  LEFT_ELBOW_=LEFT_ELBOW;
  // LEFT_FOREARM_ROTATE_=LEFT_FOREARM_ROTATE;
  LEFT_FOREARM_ROTATE_=LEFT_FOREARM_ROTATE;
} // ik_leftarminitialize()
void ik_leftarmupdate(
  const float ik_leftarmglobals[2],
  float ik_leftarmparameters[16]
)
{
  float ik_leftarmx[5];
  ik_leftarmsolve(ik_leftarmglobals,ik_leftarmparameters,ik_leftarmx);

  // LEFT_SHOULDER_IO=LEFT_SHOULDER_IO_
  LEFT_SHOULDER_IO=LEFT_SHOULDER_IO_;
  // LEFT_SHOULDER_UD=LEFT_SHOULDER_UD_
  LEFT_SHOULDER_UD=LEFT_SHOULDER_UD_;
  // LEFT_BICEP_ROTATE=LEFT_BICEP_ROTATE_
  LEFT_BICEP_ROTATE=LEFT_BICEP_ROTATE_;
  // LEFT_ELBOW=LEFT_ELBOW_
  LEFT_ELBOW=LEFT_ELBOW_;
  // LEFT_FOREARM_ROTATE=LEFT_FOREARM_ROTATE_
  LEFT_FOREARM_ROTATE=LEFT_FOREARM_ROTATE_;
} // ik_leftarmupdate()

void ik_leftarmf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[16],
  const float ik_leftarmx[5],
  float ik_leftarmy[5]
)
{
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
tmp0=0.017453293f*LEFTARM_ELBOW;
tmp1=sin(tmp0);
tmp2=0.017453293f*LEFTARM_BICEP_ROTATE;
tmp3=cos(tmp2);
tmp4=0.017453293f*LEFTARM_SHOULDER_IO;
tmp5=sin(tmp4);
tmp6=0.017453293f*LEFTARM_SHOULDER_UD;
tmp7=sin(tmp6);
tmp8=tmp3*tmp5*tmp7;
tmp9=cos(tmp4);
tmp10=sin(tmp2);
tmp11=tmp9*tmp10;
tmp12=tmp8+tmp11;
tmp13=(-15.0f)*tmp1*tmp12;
tmp14=(-1.0f)*px;
tmp15=sin(tmp4);
tmp16=3.625f*tmp15;
tmp17=cos(tmp6);
tmp18=sin(tmp4);
tmp19=9.0f*tmp17*tmp18;
tmp20=cos(tmp0);
tmp21=cos(tmp6);
tmp22=sin(tmp4);
tmp23=15.0f*tmp20*tmp21*tmp22;
tmp24=tmp13+tmp14+tmp16+tmp19+tmp23+(-6.0f);
residual0=tmp24;
tmp25=sin(tmp0);
tmp26=cos(tmp2);
tmp27=cos(tmp4);
tmp28=sin(tmp6);
tmp29=tmp26*tmp27*tmp28;
tmp30=sin(tmp2);
tmp31=sin(tmp4);
tmp32=(-1.0f)*tmp30*tmp31;
tmp33=tmp29+tmp32;
tmp34=(-15.0f)*tmp25*tmp33;
tmp35=(-1.0f)*py;
tmp36=cos(tmp4);
tmp37=3.625f*tmp36;
tmp38=cos(tmp4);
tmp39=cos(tmp6);
tmp40=9.0f*tmp38*tmp39;
tmp41=cos(tmp0);
tmp42=cos(tmp4);
tmp43=cos(tmp6);
tmp44=15.0f*tmp41*tmp42*tmp43;
tmp45=tmp34+tmp35+tmp37+tmp40+tmp44+2.375f;
residual1=tmp45;
tmp46=cos(tmp2);
tmp47=cos(tmp6);
tmp48=sin(tmp0);
tmp49=(-15.0f)*tmp46*tmp47*tmp48;
tmp50=cos(tmp0);
tmp51=sin(tmp6);
tmp52=(-15.0f)*tmp50*tmp51;
tmp53=sin(tmp6);
tmp54=(-9.0f)*tmp53;
tmp55=(-1.0f)*pz;
tmp56=tmp49+tmp52+tmp54+tmp55;
residual2=tmp56;
tmp57=0.017453293f*LEFTARM_FOREARM_ROTATE;
tmp58=cos(tmp57);
tmp59=cos(tmp0);
tmp60=cos(tmp2);
tmp61=sin(tmp4);
tmp62=sin(tmp6);
tmp63=tmp60*tmp61*tmp62;
tmp64=cos(tmp4);
tmp65=sin(tmp2);
tmp66=tmp64*tmp65;
tmp67=tmp63+tmp66;
tmp68=tmp59*tmp67;
tmp69=cos(tmp6);
tmp70=sin(tmp0);
tmp71=sin(tmp4);
tmp72=tmp69*tmp70*tmp71;
tmp73=tmp68+tmp72;
tmp74=(-1.0f)*tmp58*tmp73;
tmp75=sin(tmp57);
tmp76=cos(tmp2);
tmp77=cos(tmp4);
tmp78=tmp76*tmp77;
tmp79=sin(tmp2);
tmp80=sin(tmp4);
tmp81=sin(tmp6);
tmp82=(-1.0f)*tmp79*tmp80*tmp81;
tmp83=tmp78+tmp82;
tmp84=(-1.0f)*tmp75*tmp83;
tmp85=(-1.0f)*nx;
tmp86=tmp74+tmp84+tmp85;
residual3=tmp86;
tmp87=cos(tmp57);
tmp88=cos(tmp0);
tmp89=cos(tmp2);
tmp90=cos(tmp4);
tmp91=sin(tmp6);
tmp92=tmp89*tmp90*tmp91;
tmp93=sin(tmp2);
tmp94=sin(tmp4);
tmp95=(-1.0f)*tmp93*tmp94;
tmp96=tmp92+tmp95;
tmp97=tmp88*tmp96;
tmp98=cos(tmp4);
tmp99=cos(tmp6);
tmp100=sin(tmp0);
tmp101=tmp98*tmp99*tmp100;
tmp102=tmp97+tmp101;
tmp103=(-1.0f)*tmp87*tmp102;
tmp104=sin(tmp57);
tmp105=cos(tmp2);
tmp106=sin(tmp4);
tmp107=(-1.0f)*tmp105*tmp106;
tmp108=cos(tmp4);
tmp109=sin(tmp2);
tmp110=sin(tmp6);
tmp111=(-1.0f)*tmp108*tmp109*tmp110;
tmp112=tmp107+tmp111;
tmp113=(-1.0f)*tmp104*tmp112;
tmp114=(-1.0f)*ny;
tmp115=tmp103+tmp113+tmp114;
residual4=tmp115;
} // ik_leftarmf()

void ik_leftarmdf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[16],
  const float ik_leftarmx[5],
  float ik_leftarmdy[25]
)
{
dresidual0_dLEFT_SHOULDER_IO_=0.0f;
dresidual0_dLEFT_SHOULDER_UD_=0.0f;
dresidual0_dLEFT_BICEP_ROTATE_=0.0f;
dresidual0_dLEFT_ELBOW_=0.0f;
dresidual0_dLEFT_FOREARM_ROTATE_=0.0f;
dresidual1_dLEFT_SHOULDER_IO_=0.0f;
dresidual1_dLEFT_SHOULDER_UD_=0.0f;
dresidual1_dLEFT_BICEP_ROTATE_=0.0f;
dresidual1_dLEFT_ELBOW_=0.0f;
dresidual1_dLEFT_FOREARM_ROTATE_=0.0f;
dresidual2_dLEFT_SHOULDER_IO_=0.0f;
dresidual2_dLEFT_SHOULDER_UD_=0.0f;
dresidual2_dLEFT_BICEP_ROTATE_=0.0f;
dresidual2_dLEFT_ELBOW_=0.0f;
dresidual2_dLEFT_FOREARM_ROTATE_=0.0f;
dresidual3_dLEFT_SHOULDER_IO_=0.0f;
dresidual3_dLEFT_SHOULDER_UD_=0.0f;
dresidual3_dLEFT_BICEP_ROTATE_=0.0f;
dresidual3_dLEFT_ELBOW_=0.0f;
dresidual3_dLEFT_FOREARM_ROTATE_=0.0f;
dresidual4_dLEFT_SHOULDER_IO_=0.0f;
dresidual4_dLEFT_SHOULDER_UD_=0.0f;
dresidual4_dLEFT_BICEP_ROTATE_=0.0f;
dresidual4_dLEFT_ELBOW_=0.0f;
dresidual4_dLEFT_FOREARM_ROTATE_=0.0f;
} // ik_leftarmdf()
