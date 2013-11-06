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
  const float ik_leftarmparameters[11],
  const float ik_leftarmx[5],
  float ik_leftarmy[5]
);
void ik_leftarmdf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[11],
  const float ik_leftarmx[5],
  float ik_leftarmdy[25]
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
#define LEFT_SHOULDER_IO ik_leftarmparameters[0]
#define LEFT_SHOULDER_UD ik_leftarmparameters[1]
#define LEFT_BICEP_ROTATE ik_leftarmparameters[2]
#define LEFT_ELBOW ik_leftarmparameters[3]
#define LEFT_FOREARM_ROTATE ik_leftarmparameters[4]
#define nx ik_leftarmparameters[5]
#define ny ik_leftarmparameters[6]
#define px ik_leftarmparameters[7]
#define py ik_leftarmparameters[8]
#define pz ik_leftarmparameters[9]
#define residual ik_leftarmparameters[10]

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
  const float ik_leftarmparameters[11],
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
  float ik_leftarmparameters[11]
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
  const float ik_leftarmparameters[11],
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
tmp0=0.017453293f*LEFT_ELBOW_;
tmp1=sin(tmp0);
tmp2=0.017453293f*LEFT_BICEP_ROTATE_;
tmp3=cos(tmp2);
tmp4=0.017453293f*LEFT_SHOULDER_IO_;
tmp5=sin(tmp4);
tmp6=0.017453293f*LEFT_SHOULDER_UD_;
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
tmp57=0.017453293f*LEFT_FOREARM_ROTATE_;
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
tmp74=tmp58*tmp73;
tmp75=sin(tmp57);
tmp76=cos(tmp2);
tmp77=cos(tmp4);
tmp78=tmp76*tmp77;
tmp79=sin(tmp2);
tmp80=sin(tmp4);
tmp81=sin(tmp6);
tmp82=(-1.0f)*tmp79*tmp80*tmp81;
tmp83=tmp78+tmp82;
tmp84=tmp75*tmp83;
tmp85=sin(tmp0);
tmp86=cos(tmp2);
tmp87=sin(tmp4);
tmp88=sin(tmp6);
tmp89=tmp86*tmp87*tmp88;
tmp90=cos(tmp4);
tmp91=sin(tmp2);
tmp92=tmp90*tmp91;
tmp93=tmp89+tmp92;
tmp94=(-15.0f)*tmp85*tmp93;
tmp95=(-1.0f)*nx;
tmp96=sin(tmp4);
tmp97=3.625f*tmp96;
tmp98=cos(tmp6);
tmp99=sin(tmp4);
tmp100=9.0f*tmp98*tmp99;
tmp101=cos(tmp0);
tmp102=cos(tmp6);
tmp103=sin(tmp4);
tmp104=15.0f*tmp101*tmp102*tmp103;
tmp105=tmp74+tmp84+tmp94+tmp95+tmp97+tmp100+tmp104+(-6.0f);
residual3=tmp105;
tmp106=cos(tmp57);
tmp107=cos(tmp0);
tmp108=cos(tmp2);
tmp109=cos(tmp4);
tmp110=sin(tmp6);
tmp111=tmp108*tmp109*tmp110;
tmp112=sin(tmp2);
tmp113=sin(tmp4);
tmp114=(-1.0f)*tmp112*tmp113;
tmp115=tmp111+tmp114;
tmp116=tmp107*tmp115;
tmp117=cos(tmp4);
tmp118=cos(tmp6);
tmp119=sin(tmp0);
tmp120=tmp117*tmp118*tmp119;
tmp121=tmp116+tmp120;
tmp122=tmp106*tmp121;
tmp123=sin(tmp57);
tmp124=cos(tmp2);
tmp125=sin(tmp4);
tmp126=(-1.0f)*tmp124*tmp125;
tmp127=cos(tmp4);
tmp128=sin(tmp2);
tmp129=sin(tmp6);
tmp130=(-1.0f)*tmp127*tmp128*tmp129;
tmp131=tmp126+tmp130;
tmp132=tmp123*tmp131;
tmp133=sin(tmp0);
tmp134=cos(tmp2);
tmp135=cos(tmp4);
tmp136=sin(tmp6);
tmp137=tmp134*tmp135*tmp136;
tmp138=sin(tmp2);
tmp139=sin(tmp4);
tmp140=(-1.0f)*tmp138*tmp139;
tmp141=tmp137+tmp140;
tmp142=(-15.0f)*tmp133*tmp141;
tmp143=(-1.0f)*ny;
tmp144=cos(tmp4);
tmp145=3.625f*tmp144;
tmp146=cos(tmp4);
tmp147=cos(tmp6);
tmp148=9.0f*tmp146*tmp147;
tmp149=cos(tmp0);
tmp150=cos(tmp4);
tmp151=cos(tmp6);
tmp152=15.0f*tmp149*tmp150*tmp151;
tmp153=tmp122+tmp132+tmp142+tmp143+tmp145+tmp148+tmp152+2.375f;
residual4=tmp153;
} // ik_leftarmf()

void ik_leftarmdf(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[11],
  const float ik_leftarmx[5],
  float ik_leftarmdy[25]
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
tmp0=0.017453293f*LEFT_ELBOW_;
tmp1=sin(tmp0);
tmp2=0.017453293f*LEFT_BICEP_ROTATE_;
tmp3=sin(tmp2);
tmp4=0.017453293f*LEFT_SHOULDER_IO_;
tmp5=sin(tmp4);
tmp6=(-0.017453293f)*tmp3*tmp5;
tmp7=cos(tmp2);
tmp8=cos(tmp4);
tmp9=0.017453293f*LEFT_SHOULDER_UD_;
tmp10=sin(tmp9);
tmp11=0.017453293f*tmp7*tmp8*tmp10;
tmp12=tmp6+tmp11;
tmp13=(-15.0f)*tmp1*tmp12;
tmp14=cos(tmp4);
tmp15=0.063268185f*tmp14;
tmp16=cos(tmp4);
tmp17=cos(tmp9);
tmp18=0.15707963f*tmp16*tmp17;
tmp19=cos(tmp0);
tmp20=cos(tmp4);
tmp21=cos(tmp9);
tmp22=0.26179939f*tmp19*tmp20*tmp21;
tmp23=tmp13+tmp15+tmp18+tmp22;
dresidual0_dLEFT_SHOULDER_IO_=tmp23;
tmp24=cos(tmp2);
tmp25=cos(tmp9);
tmp26=sin(tmp0);
tmp27=sin(tmp4);
tmp28=(-0.26179939f)*tmp24*tmp25*tmp26*tmp27;
tmp29=cos(tmp0);
tmp30=sin(tmp4);
tmp31=sin(tmp9);
tmp32=(-0.26179939f)*tmp29*tmp30*tmp31;
tmp33=sin(tmp4);
tmp34=sin(tmp9);
tmp35=(-0.15707963f)*tmp33*tmp34;
tmp36=tmp28+tmp32+tmp35;
dresidual0_dLEFT_SHOULDER_UD_=tmp36;
tmp37=sin(tmp0);
tmp38=sin(tmp2);
tmp39=sin(tmp4);
tmp40=sin(tmp9);
tmp41=(-0.017453293f)*tmp38*tmp39*tmp40;
tmp42=cos(tmp2);
tmp43=cos(tmp4);
tmp44=0.017453293f*tmp42*tmp43;
tmp45=tmp41+tmp44;
tmp46=(-15.0f)*tmp37*tmp45;
dresidual0_dLEFT_BICEP_ROTATE_=tmp46;
tmp47=cos(tmp0);
tmp48=cos(tmp2);
tmp49=sin(tmp4);
tmp50=sin(tmp9);
tmp51=tmp48*tmp49*tmp50;
tmp52=cos(tmp4);
tmp53=sin(tmp2);
tmp54=tmp52*tmp53;
tmp55=tmp51+tmp54;
tmp56=(-0.26179939f)*tmp47*tmp55;
tmp57=cos(tmp9);
tmp58=sin(tmp0);
tmp59=sin(tmp4);
tmp60=(-0.26179939f)*tmp57*tmp58*tmp59;
tmp61=tmp56+tmp60;
dresidual0_dLEFT_ELBOW_=tmp61;
dresidual0_dLEFT_FOREARM_ROTATE_=0.0f;
tmp62=sin(tmp0);
tmp63=cos(tmp2);
tmp64=sin(tmp4);
tmp65=sin(tmp9);
tmp66=(-0.017453293f)*tmp63*tmp64*tmp65;
tmp67=cos(tmp4);
tmp68=sin(tmp2);
tmp69=(-0.017453293f)*tmp67*tmp68;
tmp70=tmp66+tmp69;
tmp71=(-15.0f)*tmp62*tmp70;
tmp72=cos(tmp0);
tmp73=cos(tmp9);
tmp74=sin(tmp4);
tmp75=(-0.26179939f)*tmp72*tmp73*tmp74;
tmp76=cos(tmp9);
tmp77=sin(tmp4);
tmp78=(-0.15707963f)*tmp76*tmp77;
tmp79=sin(tmp4);
tmp80=(-0.063268185f)*tmp79;
tmp81=tmp71+tmp75+tmp78+tmp80;
dresidual1_dLEFT_SHOULDER_IO_=tmp81;
tmp82=cos(tmp2);
tmp83=cos(tmp4);
tmp84=cos(tmp9);
tmp85=sin(tmp0);
tmp86=(-0.26179939f)*tmp82*tmp83*tmp84*tmp85;
tmp87=cos(tmp0);
tmp88=cos(tmp4);
tmp89=sin(tmp9);
tmp90=(-0.26179939f)*tmp87*tmp88*tmp89;
tmp91=cos(tmp4);
tmp92=sin(tmp9);
tmp93=(-0.15707963f)*tmp91*tmp92;
tmp94=tmp86+tmp90+tmp93;
dresidual1_dLEFT_SHOULDER_UD_=tmp94;
tmp95=sin(tmp0);
tmp96=cos(tmp2);
tmp97=sin(tmp4);
tmp98=(-0.017453293f)*tmp96*tmp97;
tmp99=cos(tmp4);
tmp100=sin(tmp2);
tmp101=sin(tmp9);
tmp102=(-0.017453293f)*tmp99*tmp100*tmp101;
tmp103=tmp98+tmp102;
tmp104=(-15.0f)*tmp95*tmp103;
dresidual1_dLEFT_BICEP_ROTATE_=tmp104;
tmp105=cos(tmp0);
tmp106=cos(tmp2);
tmp107=cos(tmp4);
tmp108=sin(tmp9);
tmp109=tmp106*tmp107*tmp108;
tmp110=sin(tmp2);
tmp111=sin(tmp4);
tmp112=(-1.0f)*tmp110*tmp111;
tmp113=tmp109+tmp112;
tmp114=(-0.26179939f)*tmp105*tmp113;
tmp115=cos(tmp4);
tmp116=cos(tmp9);
tmp117=sin(tmp0);
tmp118=(-0.26179939f)*tmp115*tmp116*tmp117;
tmp119=tmp114+tmp118;
dresidual1_dLEFT_ELBOW_=tmp119;
dresidual1_dLEFT_FOREARM_ROTATE_=0.0f;
dresidual2_dLEFT_SHOULDER_IO_=0.0f;
tmp120=cos(tmp0);
tmp121=cos(tmp9);
tmp122=(-0.26179939f)*tmp120*tmp121;
tmp123=cos(tmp9);
tmp124=(-0.15707963f)*tmp123;
tmp125=cos(tmp2);
tmp126=sin(tmp0);
tmp127=sin(tmp9);
tmp128=0.26179939f*tmp125*tmp126*tmp127;
tmp129=tmp122+tmp124+tmp128;
dresidual2_dLEFT_SHOULDER_UD_=tmp129;
tmp130=cos(tmp9);
tmp131=sin(tmp2);
tmp132=sin(tmp0);
tmp133=0.26179939f*tmp130*tmp131*tmp132;
dresidual2_dLEFT_BICEP_ROTATE_=tmp133;
tmp134=cos(tmp2);
tmp135=cos(tmp0);
tmp136=cos(tmp9);
tmp137=(-0.26179939f)*tmp134*tmp135*tmp136;
tmp138=sin(tmp0);
tmp139=sin(tmp9);
tmp140=0.26179939f*tmp138*tmp139;
tmp141=tmp137+tmp140;
dresidual2_dLEFT_ELBOW_=tmp141;
dresidual2_dLEFT_FOREARM_ROTATE_=0.0f;
tmp142=0.017453293f*LEFT_FOREARM_ROTATE_;
tmp143=cos(tmp142);
tmp144=cos(tmp0);
tmp145=sin(tmp2);
tmp146=sin(tmp4);
tmp147=(-0.017453293f)*tmp145*tmp146;
tmp148=cos(tmp2);
tmp149=cos(tmp4);
tmp150=sin(tmp9);
tmp151=0.017453293f*tmp148*tmp149*tmp150;
tmp152=tmp147+tmp151;
tmp153=tmp144*tmp152;
tmp154=cos(tmp4);
tmp155=cos(tmp9);
tmp156=sin(tmp0);
tmp157=0.017453293f*tmp154*tmp155*tmp156;
tmp158=tmp153+tmp157;
tmp159=tmp143*tmp158;
tmp160=sin(tmp142);
tmp161=cos(tmp2);
tmp162=sin(tmp4);
tmp163=(-0.017453293f)*tmp161*tmp162;
tmp164=cos(tmp4);
tmp165=sin(tmp2);
tmp166=sin(tmp9);
tmp167=(-0.017453293f)*tmp164*tmp165*tmp166;
tmp168=tmp163+tmp167;
tmp169=tmp160*tmp168;
tmp170=sin(tmp0);
tmp171=sin(tmp2);
tmp172=sin(tmp4);
tmp173=(-0.017453293f)*tmp171*tmp172;
tmp174=cos(tmp2);
tmp175=cos(tmp4);
tmp176=sin(tmp9);
tmp177=0.017453293f*tmp174*tmp175*tmp176;
tmp178=tmp173+tmp177;
tmp179=(-15.0f)*tmp170*tmp178;
tmp180=cos(tmp4);
tmp181=0.063268185f*tmp180;
tmp182=cos(tmp4);
tmp183=cos(tmp9);
tmp184=0.15707963f*tmp182*tmp183;
tmp185=cos(tmp0);
tmp186=cos(tmp4);
tmp187=cos(tmp9);
tmp188=0.26179939f*tmp185*tmp186*tmp187;
tmp189=tmp159+tmp169+tmp179+tmp181+tmp184+tmp188;
dresidual3_dLEFT_SHOULDER_IO_=tmp189;
tmp190=cos(tmp142);
tmp191=sin(tmp0);
tmp192=sin(tmp4);
tmp193=sin(tmp9);
tmp194=(-0.017453293f)*tmp191*tmp192*tmp193;
tmp195=cos(tmp2);
tmp196=cos(tmp0);
tmp197=cos(tmp9);
tmp198=sin(tmp4);
tmp199=0.017453293f*tmp195*tmp196*tmp197*tmp198;
tmp200=tmp194+tmp199;
tmp201=tmp190*tmp200;
tmp202=cos(tmp2);
tmp203=cos(tmp9);
tmp204=sin(tmp0);
tmp205=sin(tmp4);
tmp206=(-0.26179939f)*tmp202*tmp203*tmp204*tmp205;
tmp207=cos(tmp0);
tmp208=sin(tmp4);
tmp209=sin(tmp9);
tmp210=(-0.26179939f)*tmp207*tmp208*tmp209;
tmp211=sin(tmp4);
tmp212=sin(tmp9);
tmp213=(-0.15707963f)*tmp211*tmp212;
tmp214=cos(tmp9);
tmp215=sin(tmp2);
tmp216=sin(tmp142);
tmp217=sin(tmp4);
tmp218=(-0.017453293f)*tmp214*tmp215*tmp216*tmp217;
tmp219=tmp201+tmp206+tmp210+tmp213+tmp218;
dresidual3_dLEFT_SHOULDER_UD_=tmp219;
tmp220=cos(tmp0);
tmp221=cos(tmp142);
tmp222=sin(tmp2);
tmp223=sin(tmp4);
tmp224=sin(tmp9);
tmp225=(-0.017453293f)*tmp222*tmp223*tmp224;
tmp226=cos(tmp2);
tmp227=cos(tmp4);
tmp228=0.017453293f*tmp226*tmp227;
tmp229=tmp225+tmp228;
tmp230=tmp220*tmp221*tmp229;
tmp231=sin(tmp142);
tmp232=cos(tmp2);
tmp233=sin(tmp4);
tmp234=sin(tmp9);
tmp235=(-0.017453293f)*tmp232*tmp233*tmp234;
tmp236=cos(tmp4);
tmp237=sin(tmp2);
tmp238=(-0.017453293f)*tmp236*tmp237;
tmp239=tmp235+tmp238;
tmp240=tmp231*tmp239;
tmp241=tmp230+tmp240+dresidual0_dLEFT_BICEP_ROTATE_;
dresidual3_dLEFT_BICEP_ROTATE_=tmp241;
tmp242=cos(tmp142);
tmp243=sin(tmp0);
tmp244=cos(tmp2);
tmp245=sin(tmp4);
tmp246=sin(tmp9);
tmp247=tmp244*tmp245*tmp246;
tmp248=cos(tmp4);
tmp249=sin(tmp2);
tmp250=tmp248*tmp249;
tmp251=tmp247+tmp250;
tmp252=(-0.017453293f)*tmp243*tmp251;
tmp253=cos(tmp0);
tmp254=cos(tmp9);
tmp255=sin(tmp4);
tmp256=0.017453293f*tmp253*tmp254*tmp255;
tmp257=tmp252+tmp256;
tmp258=tmp242*tmp257;
tmp259=cos(tmp0);
tmp260=cos(tmp2);
tmp261=sin(tmp4);
tmp262=sin(tmp9);
tmp263=tmp260*tmp261*tmp262;
tmp264=cos(tmp4);
tmp265=sin(tmp2);
tmp266=tmp264*tmp265;
tmp267=tmp263+tmp266;
tmp268=(-0.26179939f)*tmp259*tmp267;
tmp269=cos(tmp9);
tmp270=sin(tmp0);
tmp271=sin(tmp4);
tmp272=(-0.26179939f)*tmp269*tmp270*tmp271;
tmp273=tmp258+tmp268+tmp272;
dresidual3_dLEFT_ELBOW_=tmp273;
tmp274=sin(tmp142);
tmp275=cos(tmp0);
tmp276=cos(tmp2);
tmp277=sin(tmp4);
tmp278=sin(tmp9);
tmp279=tmp276*tmp277*tmp278;
tmp280=cos(tmp4);
tmp281=sin(tmp2);
tmp282=tmp280*tmp281;
tmp283=tmp279+tmp282;
tmp284=tmp275*tmp283;
tmp285=cos(tmp9);
tmp286=sin(tmp0);
tmp287=sin(tmp4);
tmp288=tmp285*tmp286*tmp287;
tmp289=tmp284+tmp288;
tmp290=(-0.017453293f)*tmp274*tmp289;
tmp291=cos(tmp142);
tmp292=cos(tmp2);
tmp293=cos(tmp4);
tmp294=tmp292*tmp293;
tmp295=sin(tmp2);
tmp296=sin(tmp4);
tmp297=sin(tmp9);
tmp298=(-1.0f)*tmp295*tmp296*tmp297;
tmp299=tmp294+tmp298;
tmp300=0.017453293f*tmp291*tmp299;
tmp301=tmp290+tmp300;
dresidual3_dLEFT_FOREARM_ROTATE_=tmp301;
tmp302=cos(tmp142);
tmp303=cos(tmp0);
tmp304=cos(tmp2);
tmp305=sin(tmp4);
tmp306=sin(tmp9);
tmp307=(-0.017453293f)*tmp304*tmp305*tmp306;
tmp308=cos(tmp4);
tmp309=sin(tmp2);
tmp310=(-0.017453293f)*tmp308*tmp309;
tmp311=tmp307+tmp310;
tmp312=tmp303*tmp311;
tmp313=cos(tmp9);
tmp314=sin(tmp0);
tmp315=sin(tmp4);
tmp316=(-0.017453293f)*tmp313*tmp314*tmp315;
tmp317=tmp312+tmp316;
tmp318=tmp302*tmp317;
tmp319=sin(tmp142);
tmp320=cos(tmp2);
tmp321=cos(tmp4);
tmp322=(-0.017453293f)*tmp320*tmp321;
tmp323=sin(tmp2);
tmp324=sin(tmp4);
tmp325=sin(tmp9);
tmp326=0.017453293f*tmp323*tmp324*tmp325;
tmp327=tmp322+tmp326;
tmp328=tmp319*tmp327;
tmp329=sin(tmp0);
tmp330=cos(tmp2);
tmp331=sin(tmp4);
tmp332=sin(tmp9);
tmp333=(-0.017453293f)*tmp330*tmp331*tmp332;
tmp334=cos(tmp4);
tmp335=sin(tmp2);
tmp336=(-0.017453293f)*tmp334*tmp335;
tmp337=tmp333+tmp336;
tmp338=(-15.0f)*tmp329*tmp337;
tmp339=cos(tmp0);
tmp340=cos(tmp9);
tmp341=sin(tmp4);
tmp342=(-0.26179939f)*tmp339*tmp340*tmp341;
tmp343=cos(tmp9);
tmp344=sin(tmp4);
tmp345=(-0.15707963f)*tmp343*tmp344;
tmp346=sin(tmp4);
tmp347=(-0.063268185f)*tmp346;
tmp348=tmp318+tmp328+tmp338+tmp342+tmp345+tmp347;
dresidual4_dLEFT_SHOULDER_IO_=tmp348;
tmp349=cos(tmp142);
tmp350=cos(tmp4);
tmp351=sin(tmp0);
tmp352=sin(tmp9);
tmp353=(-0.017453293f)*tmp350*tmp351*tmp352;
tmp354=cos(tmp2);
tmp355=cos(tmp0);
tmp356=cos(tmp4);
tmp357=cos(tmp9);
tmp358=0.017453293f*tmp354*tmp355*tmp356*tmp357;
tmp359=tmp353+tmp358;
tmp360=tmp349*tmp359;
tmp361=cos(tmp2);
tmp362=cos(tmp4);
tmp363=cos(tmp9);
tmp364=sin(tmp0);
tmp365=(-0.26179939f)*tmp361*tmp362*tmp363*tmp364;
tmp366=cos(tmp0);
tmp367=cos(tmp4);
tmp368=sin(tmp9);
tmp369=(-0.26179939f)*tmp366*tmp367*tmp368;
tmp370=cos(tmp4);
tmp371=sin(tmp9);
tmp372=(-0.15707963f)*tmp370*tmp371;
tmp373=cos(tmp4);
tmp374=cos(tmp9);
tmp375=sin(tmp2);
tmp376=sin(tmp142);
tmp377=(-0.017453293f)*tmp373*tmp374*tmp375*tmp376;
tmp378=tmp360+tmp365+tmp369+tmp372+tmp377;
dresidual4_dLEFT_SHOULDER_UD_=tmp378;
tmp379=cos(tmp0);
tmp380=cos(tmp142);
tmp381=cos(tmp2);
tmp382=sin(tmp4);
tmp383=(-0.017453293f)*tmp381*tmp382;
tmp384=cos(tmp4);
tmp385=sin(tmp2);
tmp386=sin(tmp9);
tmp387=(-0.017453293f)*tmp384*tmp385*tmp386;
tmp388=tmp383+tmp387;
tmp389=tmp379*tmp380*tmp388;
tmp390=sin(tmp142);
tmp391=cos(tmp2);
tmp392=cos(tmp4);
tmp393=sin(tmp9);
tmp394=(-0.017453293f)*tmp391*tmp392*tmp393;
tmp395=sin(tmp2);
tmp396=sin(tmp4);
tmp397=0.017453293f*tmp395*tmp396;
tmp398=tmp394+tmp397;
tmp399=tmp390*tmp398;
tmp400=tmp389+tmp399+dresidual1_dLEFT_BICEP_ROTATE_;
dresidual4_dLEFT_BICEP_ROTATE_=tmp400;
tmp401=cos(tmp142);
tmp402=sin(tmp0);
tmp403=cos(tmp2);
tmp404=cos(tmp4);
tmp405=sin(tmp9);
tmp406=tmp403*tmp404*tmp405;
tmp407=sin(tmp2);
tmp408=sin(tmp4);
tmp409=(-1.0f)*tmp407*tmp408;
tmp410=tmp406+tmp409;
tmp411=(-0.017453293f)*tmp402*tmp410;
tmp412=cos(tmp0);
tmp413=cos(tmp4);
tmp414=cos(tmp9);
tmp415=0.017453293f*tmp412*tmp413*tmp414;
tmp416=tmp411+tmp415;
tmp417=tmp401*tmp416;
tmp418=cos(tmp0);
tmp419=cos(tmp2);
tmp420=cos(tmp4);
tmp421=sin(tmp9);
tmp422=tmp419*tmp420*tmp421;
tmp423=sin(tmp2);
tmp424=sin(tmp4);
tmp425=(-1.0f)*tmp423*tmp424;
tmp426=tmp422+tmp425;
tmp427=(-0.26179939f)*tmp418*tmp426;
tmp428=cos(tmp4);
tmp429=cos(tmp9);
tmp430=sin(tmp0);
tmp431=(-0.26179939f)*tmp428*tmp429*tmp430;
tmp432=tmp417+tmp427+tmp431;
dresidual4_dLEFT_ELBOW_=tmp432;
tmp433=sin(tmp142);
tmp434=cos(tmp0);
tmp435=cos(tmp2);
tmp436=cos(tmp4);
tmp437=sin(tmp9);
tmp438=tmp435*tmp436*tmp437;
tmp439=sin(tmp2);
tmp440=sin(tmp4);
tmp441=(-1.0f)*tmp439*tmp440;
tmp442=tmp438+tmp441;
tmp443=tmp434*tmp442;
tmp444=cos(tmp4);
tmp445=cos(tmp9);
tmp446=sin(tmp0);
tmp447=tmp444*tmp445*tmp446;
tmp448=tmp443+tmp447;
tmp449=(-0.017453293f)*tmp433*tmp448;
tmp450=cos(tmp142);
tmp451=cos(tmp2);
tmp452=sin(tmp4);
tmp453=(-1.0f)*tmp451*tmp452;
tmp454=cos(tmp4);
tmp455=sin(tmp2);
tmp456=sin(tmp9);
tmp457=(-1.0f)*tmp454*tmp455*tmp456;
tmp458=tmp453+tmp457;
tmp459=0.017453293f*tmp450*tmp458;
tmp460=tmp449+tmp459;
dresidual4_dLEFT_FOREARM_ROTATE_=tmp460;
} // ik_leftarmdf()
