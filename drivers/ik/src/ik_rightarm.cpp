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

enum { ik_rightarmequation_count=5 };
extern const char *ik_rightarmequation_names[5];
const char *ik_rightarmglobal_names[]={
  "epsilon",
  "steps"
};

const char *ik_rightarmparameter_names[]={
  "RIGHTARM_SHOULDER_IO",
  "RIGHTARM_SHOULDER_UD",
  "RIGHTARM_BICEP_ROTATE",
  "RIGHTARM_ELBOW",
  "RIGHTARM_FOREARM_ROTATE",
  "rightpx",
  "rightpy",
  "rightpz",
  "rightroll",
  "rightyaw",
  "residual"
};

const char *ik_rightarmvariable_names[]={
  "RIGHTARM_SHOULDER_IO_",
  "RIGHTARM_SHOULDER_UD_",
  "RIGHTARM_BICEP_ROTATE_",
  "RIGHTARM_ELBOW_",
  "RIGHTARM_FOREARM_ROTATE_"
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
  const float ik_rightarmparameters[11],
  const float ik_rightarmx[5],
  float ik_rightarmy[5]
);
void ik_rightarmdf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  const float ik_rightarmx[5],
  float ik_rightarmdy[25]
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
#define RIGHTARM_SHOULDER_IO ik_rightarmparameters[0]
#define RIGHTARM_SHOULDER_UD ik_rightarmparameters[1]
#define RIGHTARM_BICEP_ROTATE ik_rightarmparameters[2]
#define RIGHTARM_ELBOW ik_rightarmparameters[3]
#define RIGHTARM_FOREARM_ROTATE ik_rightarmparameters[4]
#define rightpx ik_rightarmparameters[5]
#define rightpy ik_rightarmparameters[6]
#define rightpz ik_rightarmparameters[7]
#define rightroll ik_rightarmparameters[8]
#define rightyaw ik_rightarmparameters[9]
#define residual ik_rightarmparameters[10]

// variable aliases
#define RIGHTARM_SHOULDER_IO_ ik_rightarmx[0]
#define RIGHTARM_SHOULDER_UD_ ik_rightarmx[1]
#define RIGHTARM_BICEP_ROTATE_ ik_rightarmx[2]
#define RIGHTARM_ELBOW_ ik_rightarmx[3]
#define RIGHTARM_FOREARM_ROTATE_ ik_rightarmx[4]

// residual aliases
#define residual0 ik_rightarmy[0]
#define residual1 ik_rightarmy[1]
#define residual2 ik_rightarmy[2]
#define residual3 ik_rightarmy[3]
#define residual4 ik_rightarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_dRIGHTARM_SHOULDER_IO_ ik_rightarmdy[0]
#define dresidual0_dRIGHTARM_SHOULDER_UD_ ik_rightarmdy[5]
#define dresidual0_dRIGHTARM_BICEP_ROTATE_ ik_rightarmdy[10]
#define dresidual0_dRIGHTARM_ELBOW_ ik_rightarmdy[15]
#define dresidual0_dRIGHTARM_FOREARM_ROTATE_ ik_rightarmdy[20]
#define dresidual1_dRIGHTARM_SHOULDER_IO_ ik_rightarmdy[1]
#define dresidual1_dRIGHTARM_SHOULDER_UD_ ik_rightarmdy[6]
#define dresidual1_dRIGHTARM_BICEP_ROTATE_ ik_rightarmdy[11]
#define dresidual1_dRIGHTARM_ELBOW_ ik_rightarmdy[16]
#define dresidual1_dRIGHTARM_FOREARM_ROTATE_ ik_rightarmdy[21]
#define dresidual2_dRIGHTARM_SHOULDER_IO_ ik_rightarmdy[2]
#define dresidual2_dRIGHTARM_SHOULDER_UD_ ik_rightarmdy[7]
#define dresidual2_dRIGHTARM_BICEP_ROTATE_ ik_rightarmdy[12]
#define dresidual2_dRIGHTARM_ELBOW_ ik_rightarmdy[17]
#define dresidual2_dRIGHTARM_FOREARM_ROTATE_ ik_rightarmdy[22]
#define dresidual3_dRIGHTARM_SHOULDER_IO_ ik_rightarmdy[3]
#define dresidual3_dRIGHTARM_SHOULDER_UD_ ik_rightarmdy[8]
#define dresidual3_dRIGHTARM_BICEP_ROTATE_ ik_rightarmdy[13]
#define dresidual3_dRIGHTARM_ELBOW_ ik_rightarmdy[18]
#define dresidual3_dRIGHTARM_FOREARM_ROTATE_ ik_rightarmdy[23]
#define dresidual4_dRIGHTARM_SHOULDER_IO_ ik_rightarmdy[4]
#define dresidual4_dRIGHTARM_SHOULDER_UD_ ik_rightarmdy[9]
#define dresidual4_dRIGHTARM_BICEP_ROTATE_ ik_rightarmdy[14]
#define dresidual4_dRIGHTARM_ELBOW_ ik_rightarmdy[19]
#define dresidual4_dRIGHTARM_FOREARM_ROTATE_ ik_rightarmdy[24]

void ik_rightarminitialize(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  float ik_rightarmx[5]
)
{
  // initialize unknowns from parameters
  // RIGHTARM_SHOULDER_IO_=RIGHTARM_SHOULDER_IO;
  RIGHTARM_SHOULDER_IO_=RIGHTARM_SHOULDER_IO;
  // RIGHTARM_SHOULDER_UD_=RIGHTARM_SHOULDER_UD;
  RIGHTARM_SHOULDER_UD_=RIGHTARM_SHOULDER_UD;
  // RIGHTARM_BICEP_ROTATE_=RIGHTARM_BICEP_ROTATE;
  RIGHTARM_BICEP_ROTATE_=RIGHTARM_BICEP_ROTATE;
  // RIGHTARM_ELBOW_=RIGHTARM_ELBOW;
  RIGHTARM_ELBOW_=RIGHTARM_ELBOW;
  // RIGHTARM_FOREARM_ROTATE_=RIGHTARM_FOREARM_ROTATE;
  RIGHTARM_FOREARM_ROTATE_=RIGHTARM_FOREARM_ROTATE;
} // ik_rightarminitialize()
void ik_rightarmupdate(
  const float ik_rightarmglobals[2],
  float ik_rightarmparameters[11]
)
{
  float ik_rightarmx[5];
  ik_rightarmsolve(ik_rightarmglobals,ik_rightarmparameters,ik_rightarmx);

  // RIGHTARM_SHOULDER_IO=RIGHTARM_SHOULDER_IO_
  RIGHTARM_SHOULDER_IO=RIGHTARM_SHOULDER_IO_;
  // RIGHTARM_SHOULDER_UD=RIGHTARM_SHOULDER_UD_
  RIGHTARM_SHOULDER_UD=RIGHTARM_SHOULDER_UD_;
  // RIGHTARM_BICEP_ROTATE=RIGHTARM_BICEP_ROTATE_
  RIGHTARM_BICEP_ROTATE=RIGHTARM_BICEP_ROTATE_;
  // RIGHTARM_ELBOW=RIGHTARM_ELBOW_
  RIGHTARM_ELBOW=RIGHTARM_ELBOW_;
  // RIGHTARM_FOREARM_ROTATE=RIGHTARM_FOREARM_ROTATE_
  RIGHTARM_FOREARM_ROTATE=RIGHTARM_FOREARM_ROTATE_;
} // ik_rightarmupdate()

void ik_rightarmf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  const float ik_rightarmx[5],
  float ik_rightarmy[5]
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
tmp0=(-1.0f)*rightpx;
tmp1=0.017453293f*RIGHTARM_SHOULDER_IO_;
tmp2=tmp1+(-0.78539816f);
tmp3=sin(tmp2);
tmp4=3.625f*tmp3;
tmp5=0.017453293f*RIGHTARM_SHOULDER_UD_;
tmp6=tmp5+(-0.78539816f);
tmp7=cos(tmp6);
tmp8=tmp1+(-0.78539816f);
tmp9=sin(tmp8);
tmp10=9.0f*tmp7*tmp9;
tmp11=(-0.017453293f)*RIGHTARM_ELBOW_;
tmp12=tmp11+0.78539816f;
tmp13=cos(tmp12);
tmp14=tmp5+(-0.78539816f);
tmp15=cos(tmp14);
tmp16=tmp1+(-0.78539816f);
tmp17=sin(tmp16);
tmp18=15.0f*tmp13*tmp15*tmp17;
tmp19=tmp11+0.78539816f;
tmp20=sin(tmp19);
tmp21=tmp1+(-0.78539816f);
tmp22=cos(tmp21);
tmp23=0.017453293f*RIGHTARM_BICEP_ROTATE_;
tmp24=sin(tmp23);
tmp25=tmp22*tmp24;
tmp26=cos(tmp23);
tmp27=tmp1+(-0.78539816f);
tmp28=sin(tmp27);
tmp29=tmp5+(-0.78539816f);
tmp30=sin(tmp29);
tmp31=(-1.0f)*tmp26*tmp28*tmp30;
tmp32=tmp25+tmp31;
tmp33=15.0f*tmp20*tmp32;
tmp34=tmp0+tmp4+tmp10+tmp18+tmp33+6.0f;
residual0=tmp34;
tmp35=(-1.0f)*rightpy;
tmp36=tmp1+(-0.78539816f);
tmp37=cos(tmp36);
tmp38=3.625f*tmp37;
tmp39=tmp1+(-0.78539816f);
tmp40=cos(tmp39);
tmp41=tmp5+(-0.78539816f);
tmp42=cos(tmp41);
tmp43=9.0f*tmp40*tmp42;
tmp44=tmp11+0.78539816f;
tmp45=cos(tmp44);
tmp46=tmp1+(-0.78539816f);
tmp47=cos(tmp46);
tmp48=tmp5+(-0.78539816f);
tmp49=cos(tmp48);
tmp50=15.0f*tmp45*tmp47*tmp49;
tmp51=tmp11+0.78539816f;
tmp52=sin(tmp51);
tmp53=tmp1+(-0.78539816f);
tmp54=cos(tmp53);
tmp55=cos(tmp23);
tmp56=tmp5+(-0.78539816f);
tmp57=sin(tmp56);
tmp58=(-1.0f)*tmp54*tmp55*tmp57;
tmp59=tmp1+(-0.78539816f);
tmp60=sin(tmp59);
tmp61=sin(tmp23);
tmp62=(-1.0f)*tmp60*tmp61;
tmp63=tmp58+tmp62;
tmp64=15.0f*tmp52*tmp63;
tmp65=tmp35+tmp38+tmp43+tmp50+tmp64+2.375f;
residual1=tmp65;
tmp66=(-1.0f)*rightpz;
tmp67=tmp5+(-0.78539816f);
tmp68=sin(tmp67);
tmp69=9.0f*tmp68;
tmp70=tmp11+0.78539816f;
tmp71=cos(tmp70);
tmp72=tmp5+(-0.78539816f);
tmp73=sin(tmp72);
tmp74=15.0f*tmp71*tmp73;
tmp75=tmp5+(-0.78539816f);
tmp76=cos(tmp75);
tmp77=cos(tmp23);
tmp78=tmp11+0.78539816f;
tmp79=sin(tmp78);
tmp80=15.0f*tmp76*tmp77*tmp79;
tmp81=tmp66+tmp69+tmp74+tmp80;
residual2=tmp81;
tmp82=0.017453293f*RIGHTARM_FOREARM_ROTATE_;
tmp83=tmp82+0.52359878f;
tmp84=cos(tmp83);
tmp85=tmp11+0.78539816f;
tmp86=cos(tmp85);
tmp87=tmp1+(-0.78539816f);
tmp88=cos(tmp87);
tmp89=sin(tmp23);
tmp90=tmp88*tmp89;
tmp91=cos(tmp23);
tmp92=tmp1+(-0.78539816f);
tmp93=sin(tmp92);
tmp94=tmp5+(-0.78539816f);
tmp95=sin(tmp94);
tmp96=(-1.0f)*tmp91*tmp93*tmp95;
tmp97=tmp90+tmp96;
tmp98=tmp86*tmp97;
tmp99=tmp5+(-0.78539816f);
tmp100=cos(tmp99);
tmp101=tmp11+0.78539816f;
tmp102=sin(tmp101);
tmp103=tmp1+(-0.78539816f);
tmp104=sin(tmp103);
tmp105=(-1.0f)*tmp100*tmp102*tmp104;
tmp106=tmp98+tmp105;
tmp107=tmp84*tmp106;
tmp108=tmp82+0.52359878f;
tmp109=sin(tmp108);
tmp110=tmp1+(-0.78539816f);
tmp111=cos(tmp110);
tmp112=cos(tmp23);
tmp113=tmp111*tmp112;
tmp114=tmp1+(-0.78539816f);
tmp115=sin(tmp114);
tmp116=tmp5+(-0.78539816f);
tmp117=sin(tmp116);
tmp118=sin(tmp23);
tmp119=tmp115*tmp117*tmp118;
tmp120=tmp113+tmp119;
tmp121=tmp109*tmp120;
tmp122=sin(rightroll);
tmp123=(-1.0f)*tmp122;
tmp124=tmp107+tmp121+tmp123;
residual3=tmp124;
tmp125=tmp11+0.78539816f;
tmp126=cos(tmp125);
tmp127=tmp5+(-0.78539816f);
tmp128=cos(tmp127);
tmp129=tmp1+(-0.78539816f);
tmp130=sin(tmp129);
tmp131=tmp126*tmp128*tmp130;
tmp132=tmp11+0.78539816f;
tmp133=sin(tmp132);
tmp134=tmp1+(-0.78539816f);
tmp135=cos(tmp134);
tmp136=sin(tmp23);
tmp137=tmp135*tmp136;
tmp138=cos(tmp23);
tmp139=tmp1+(-0.78539816f);
tmp140=sin(tmp139);
tmp141=tmp5+(-0.78539816f);
tmp142=sin(tmp141);
tmp143=(-1.0f)*tmp138*tmp140*tmp142;
tmp144=tmp137+tmp143;
tmp145=tmp133*tmp144;
tmp146=sin(rightyaw);
tmp147=(-1.0f)*tmp146;
tmp148=tmp131+tmp145+tmp147;
residual4=tmp148;
} // ik_rightarmf()

void ik_rightarmdf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[11],
  const float ik_rightarmx[5],
  float ik_rightarmdy[25]
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
tmp0=0.017453293f*RIGHTARM_SHOULDER_IO_;
tmp1=tmp0+(-0.78539816f);
tmp2=cos(tmp1);
tmp3=0.063268185f*tmp2;
tmp4=tmp0+(-0.78539816f);
tmp5=cos(tmp4);
tmp6=0.017453293f*RIGHTARM_SHOULDER_UD_;
tmp7=tmp6+(-0.78539816f);
tmp8=cos(tmp7);
tmp9=0.15707963f*tmp5*tmp8;
tmp10=(-0.017453293f)*RIGHTARM_ELBOW_;
tmp11=tmp10+0.78539816f;
tmp12=cos(tmp11);
tmp13=tmp0+(-0.78539816f);
tmp14=cos(tmp13);
tmp15=tmp6+(-0.78539816f);
tmp16=cos(tmp15);
tmp17=0.26179939f*tmp12*tmp14*tmp16;
tmp18=tmp10+0.78539816f;
tmp19=sin(tmp18);
tmp20=tmp0+(-0.78539816f);
tmp21=cos(tmp20);
tmp22=0.017453293f*RIGHTARM_BICEP_ROTATE_;
tmp23=cos(tmp22);
tmp24=tmp6+(-0.78539816f);
tmp25=sin(tmp24);
tmp26=(-0.017453293f)*tmp21*tmp23*tmp25;
tmp27=tmp0+(-0.78539816f);
tmp28=sin(tmp27);
tmp29=sin(tmp22);
tmp30=(-0.017453293f)*tmp28*tmp29;
tmp31=tmp26+tmp30;
tmp32=15.0f*tmp19*tmp31;
tmp33=tmp3+tmp9+tmp17+tmp32;
dresidual0_dRIGHTARM_SHOULDER_IO_=tmp33;
tmp34=tmp10+0.78539816f;
tmp35=cos(tmp34);
tmp36=tmp0+(-0.78539816f);
tmp37=sin(tmp36);
tmp38=tmp6+(-0.78539816f);
tmp39=sin(tmp38);
tmp40=(-0.26179939f)*tmp35*tmp37*tmp39;
tmp41=tmp6+(-0.78539816f);
tmp42=cos(tmp41);
tmp43=cos(tmp22);
tmp44=tmp10+0.78539816f;
tmp45=sin(tmp44);
tmp46=tmp0+(-0.78539816f);
tmp47=sin(tmp46);
tmp48=(-0.26179939f)*tmp42*tmp43*tmp45*tmp47;
tmp49=tmp0+(-0.78539816f);
tmp50=sin(tmp49);
tmp51=tmp6+(-0.78539816f);
tmp52=sin(tmp51);
tmp53=(-0.15707963f)*tmp50*tmp52;
tmp54=tmp40+tmp48+tmp53;
dresidual0_dRIGHTARM_SHOULDER_UD_=tmp54;
tmp55=tmp10+0.78539816f;
tmp56=sin(tmp55);
tmp57=tmp0+(-0.78539816f);
tmp58=cos(tmp57);
tmp59=cos(tmp22);
tmp60=0.017453293f*tmp58*tmp59;
tmp61=tmp0+(-0.78539816f);
tmp62=sin(tmp61);
tmp63=tmp6+(-0.78539816f);
tmp64=sin(tmp63);
tmp65=sin(tmp22);
tmp66=0.017453293f*tmp62*tmp64*tmp65;
tmp67=tmp60+tmp66;
tmp68=15.0f*tmp56*tmp67;
dresidual0_dRIGHTARM_BICEP_ROTATE_=tmp68;
tmp69=tmp10+0.78539816f;
tmp70=cos(tmp69);
tmp71=tmp0+(-0.78539816f);
tmp72=cos(tmp71);
tmp73=sin(tmp22);
tmp74=tmp72*tmp73;
tmp75=cos(tmp22);
tmp76=tmp0+(-0.78539816f);
tmp77=sin(tmp76);
tmp78=tmp6+(-0.78539816f);
tmp79=sin(tmp78);
tmp80=(-1.0f)*tmp75*tmp77*tmp79;
tmp81=tmp74+tmp80;
tmp82=(-0.26179939f)*tmp70*tmp81;
tmp83=tmp6+(-0.78539816f);
tmp84=cos(tmp83);
tmp85=tmp10+0.78539816f;
tmp86=sin(tmp85);
tmp87=tmp0+(-0.78539816f);
tmp88=sin(tmp87);
tmp89=0.26179939f*tmp84*tmp86*tmp88;
tmp90=tmp82+tmp89;
dresidual0_dRIGHTARM_ELBOW_=tmp90;
dresidual0_dRIGHTARM_FOREARM_ROTATE_=0.0f;
tmp91=tmp10+0.78539816f;
tmp92=cos(tmp91);
tmp93=tmp6+(-0.78539816f);
tmp94=cos(tmp93);
tmp95=tmp0+(-0.78539816f);
tmp96=sin(tmp95);
tmp97=(-0.26179939f)*tmp92*tmp94*tmp96;
tmp98=tmp6+(-0.78539816f);
tmp99=cos(tmp98);
tmp100=tmp0+(-0.78539816f);
tmp101=sin(tmp100);
tmp102=(-0.15707963f)*tmp99*tmp101;
tmp103=tmp0+(-0.78539816f);
tmp104=sin(tmp103);
tmp105=(-0.063268185f)*tmp104;
tmp106=tmp10+0.78539816f;
tmp107=sin(tmp106);
tmp108=tmp0+(-0.78539816f);
tmp109=cos(tmp108);
tmp110=sin(tmp22);
tmp111=(-0.017453293f)*tmp109*tmp110;
tmp112=cos(tmp22);
tmp113=tmp0+(-0.78539816f);
tmp114=sin(tmp113);
tmp115=tmp6+(-0.78539816f);
tmp116=sin(tmp115);
tmp117=0.017453293f*tmp112*tmp114*tmp116;
tmp118=tmp111+tmp117;
tmp119=15.0f*tmp107*tmp118;
tmp120=tmp97+tmp102+tmp105+tmp119;
dresidual1_dRIGHTARM_SHOULDER_IO_=tmp120;
tmp121=tmp10+0.78539816f;
tmp122=cos(tmp121);
tmp123=tmp0+(-0.78539816f);
tmp124=cos(tmp123);
tmp125=tmp6+(-0.78539816f);
tmp126=sin(tmp125);
tmp127=(-0.26179939f)*tmp122*tmp124*tmp126;
tmp128=tmp0+(-0.78539816f);
tmp129=cos(tmp128);
tmp130=tmp6+(-0.78539816f);
tmp131=cos(tmp130);
tmp132=cos(tmp22);
tmp133=tmp10+0.78539816f;
tmp134=sin(tmp133);
tmp135=(-0.26179939f)*tmp129*tmp131*tmp132*tmp134;
tmp136=tmp0+(-0.78539816f);
tmp137=cos(tmp136);
tmp138=tmp6+(-0.78539816f);
tmp139=sin(tmp138);
tmp140=(-0.15707963f)*tmp137*tmp139;
tmp141=tmp127+tmp135+tmp140;
dresidual1_dRIGHTARM_SHOULDER_UD_=tmp141;
tmp142=tmp10+0.78539816f;
tmp143=sin(tmp142);
tmp144=cos(tmp22);
tmp145=tmp0+(-0.78539816f);
tmp146=sin(tmp145);
tmp147=(-0.017453293f)*tmp144*tmp146;
tmp148=tmp0+(-0.78539816f);
tmp149=cos(tmp148);
tmp150=tmp6+(-0.78539816f);
tmp151=sin(tmp150);
tmp152=sin(tmp22);
tmp153=0.017453293f*tmp149*tmp151*tmp152;
tmp154=tmp147+tmp153;
tmp155=15.0f*tmp143*tmp154;
dresidual1_dRIGHTARM_BICEP_ROTATE_=tmp155;
tmp156=tmp10+0.78539816f;
tmp157=cos(tmp156);
tmp158=tmp0+(-0.78539816f);
tmp159=cos(tmp158);
tmp160=cos(tmp22);
tmp161=tmp6+(-0.78539816f);
tmp162=sin(tmp161);
tmp163=(-1.0f)*tmp159*tmp160*tmp162;
tmp164=tmp0+(-0.78539816f);
tmp165=sin(tmp164);
tmp166=sin(tmp22);
tmp167=(-1.0f)*tmp165*tmp166;
tmp168=tmp163+tmp167;
tmp169=(-0.26179939f)*tmp157*tmp168;
tmp170=tmp0+(-0.78539816f);
tmp171=cos(tmp170);
tmp172=tmp6+(-0.78539816f);
tmp173=cos(tmp172);
tmp174=tmp10+0.78539816f;
tmp175=sin(tmp174);
tmp176=0.26179939f*tmp171*tmp173*tmp175;
tmp177=tmp169+tmp176;
dresidual1_dRIGHTARM_ELBOW_=tmp177;
dresidual1_dRIGHTARM_FOREARM_ROTATE_=0.0f;
dresidual2_dRIGHTARM_SHOULDER_IO_=0.0f;
tmp178=cos(tmp22);
tmp179=tmp10+0.78539816f;
tmp180=sin(tmp179);
tmp181=tmp6+(-0.78539816f);
tmp182=sin(tmp181);
tmp183=(-0.26179939f)*tmp178*tmp180*tmp182;
tmp184=tmp6+(-0.78539816f);
tmp185=cos(tmp184);
tmp186=0.15707963f*tmp185;
tmp187=tmp10+0.78539816f;
tmp188=cos(tmp187);
tmp189=tmp6+(-0.78539816f);
tmp190=cos(tmp189);
tmp191=0.26179939f*tmp188*tmp190;
tmp192=tmp183+tmp186+tmp191;
dresidual2_dRIGHTARM_SHOULDER_UD_=tmp192;
tmp193=tmp6+(-0.78539816f);
tmp194=cos(tmp193);
tmp195=tmp10+0.78539816f;
tmp196=sin(tmp195);
tmp197=sin(tmp22);
tmp198=(-0.26179939f)*tmp194*tmp196*tmp197;
dresidual2_dRIGHTARM_BICEP_ROTATE_=tmp198;
tmp199=tmp10+0.78539816f;
tmp200=cos(tmp199);
tmp201=tmp6+(-0.78539816f);
tmp202=cos(tmp201);
tmp203=cos(tmp22);
tmp204=(-0.26179939f)*tmp200*tmp202*tmp203;
tmp205=tmp10+0.78539816f;
tmp206=sin(tmp205);
tmp207=tmp6+(-0.78539816f);
tmp208=sin(tmp207);
tmp209=0.26179939f*tmp206*tmp208;
tmp210=tmp204+tmp209;
dresidual2_dRIGHTARM_ELBOW_=tmp210;
dresidual2_dRIGHTARM_FOREARM_ROTATE_=0.0f;
tmp211=0.017453293f*RIGHTARM_FOREARM_ROTATE_;
tmp212=tmp211+0.52359878f;
tmp213=cos(tmp212);
tmp214=tmp10+0.78539816f;
tmp215=cos(tmp214);
tmp216=tmp0+(-0.78539816f);
tmp217=cos(tmp216);
tmp218=cos(tmp22);
tmp219=tmp6+(-0.78539816f);
tmp220=sin(tmp219);
tmp221=(-0.017453293f)*tmp217*tmp218*tmp220;
tmp222=tmp0+(-0.78539816f);
tmp223=sin(tmp222);
tmp224=sin(tmp22);
tmp225=(-0.017453293f)*tmp223*tmp224;
tmp226=tmp221+tmp225;
tmp227=tmp215*tmp226;
tmp228=tmp0+(-0.78539816f);
tmp229=cos(tmp228);
tmp230=tmp6+(-0.78539816f);
tmp231=cos(tmp230);
tmp232=tmp10+0.78539816f;
tmp233=sin(tmp232);
tmp234=(-0.017453293f)*tmp229*tmp231*tmp233;
tmp235=tmp227+tmp234;
tmp236=tmp213*tmp235;
tmp237=tmp211+0.52359878f;
tmp238=sin(tmp237);
tmp239=cos(tmp22);
tmp240=tmp0+(-0.78539816f);
tmp241=sin(tmp240);
tmp242=(-0.017453293f)*tmp239*tmp241;
tmp243=tmp0+(-0.78539816f);
tmp244=cos(tmp243);
tmp245=tmp6+(-0.78539816f);
tmp246=sin(tmp245);
tmp247=sin(tmp22);
tmp248=0.017453293f*tmp244*tmp246*tmp247;
tmp249=tmp242+tmp248;
tmp250=tmp238*tmp249;
tmp251=tmp236+tmp250;
dresidual3_dRIGHTARM_SHOULDER_IO_=tmp251;
tmp252=tmp211+0.52359878f;
tmp253=cos(tmp252);
tmp254=tmp10+0.78539816f;
tmp255=cos(tmp254);
tmp256=tmp6+(-0.78539816f);
tmp257=cos(tmp256);
tmp258=cos(tmp22);
tmp259=tmp0+(-0.78539816f);
tmp260=sin(tmp259);
tmp261=(-0.017453293f)*tmp255*tmp257*tmp258*tmp260;
tmp262=tmp10+0.78539816f;
tmp263=sin(tmp262);
tmp264=tmp0+(-0.78539816f);
tmp265=sin(tmp264);
tmp266=tmp6+(-0.78539816f);
tmp267=sin(tmp266);
tmp268=0.017453293f*tmp263*tmp265*tmp267;
tmp269=tmp261+tmp268;
tmp270=tmp253*tmp269;
tmp271=tmp6+(-0.78539816f);
tmp272=cos(tmp271);
tmp273=tmp211+0.52359878f;
tmp274=sin(tmp273);
tmp275=tmp0+(-0.78539816f);
tmp276=sin(tmp275);
tmp277=sin(tmp22);
tmp278=0.017453293f*tmp272*tmp274*tmp276*tmp277;
tmp279=tmp270+tmp278;
dresidual3_dRIGHTARM_SHOULDER_UD_=tmp279;
tmp280=tmp10+0.78539816f;
tmp281=cos(tmp280);
tmp282=tmp211+0.52359878f;
tmp283=cos(tmp282);
tmp284=tmp0+(-0.78539816f);
tmp285=cos(tmp284);
tmp286=cos(tmp22);
tmp287=0.017453293f*tmp285*tmp286;
tmp288=tmp0+(-0.78539816f);
tmp289=sin(tmp288);
tmp290=tmp6+(-0.78539816f);
tmp291=sin(tmp290);
tmp292=sin(tmp22);
tmp293=0.017453293f*tmp289*tmp291*tmp292;
tmp294=tmp287+tmp293;
tmp295=tmp281*tmp283*tmp294;
tmp296=tmp211+0.52359878f;
tmp297=sin(tmp296);
tmp298=tmp0+(-0.78539816f);
tmp299=cos(tmp298);
tmp300=sin(tmp22);
tmp301=(-0.017453293f)*tmp299*tmp300;
tmp302=cos(tmp22);
tmp303=tmp0+(-0.78539816f);
tmp304=sin(tmp303);
tmp305=tmp6+(-0.78539816f);
tmp306=sin(tmp305);
tmp307=0.017453293f*tmp302*tmp304*tmp306;
tmp308=tmp301+tmp307;
tmp309=tmp297*tmp308;
tmp310=tmp295+tmp309;
dresidual3_dRIGHTARM_BICEP_ROTATE_=tmp310;
tmp311=tmp211+0.52359878f;
tmp312=cos(tmp311);
tmp313=tmp10+0.78539816f;
tmp314=cos(tmp313);
tmp315=tmp6+(-0.78539816f);
tmp316=cos(tmp315);
tmp317=tmp0+(-0.78539816f);
tmp318=sin(tmp317);
tmp319=0.017453293f*tmp314*tmp316*tmp318;
tmp320=tmp10+0.78539816f;
tmp321=sin(tmp320);
tmp322=tmp0+(-0.78539816f);
tmp323=cos(tmp322);
tmp324=sin(tmp22);
tmp325=tmp323*tmp324;
tmp326=cos(tmp22);
tmp327=tmp0+(-0.78539816f);
tmp328=sin(tmp327);
tmp329=tmp6+(-0.78539816f);
tmp330=sin(tmp329);
tmp331=(-1.0f)*tmp326*tmp328*tmp330;
tmp332=tmp325+tmp331;
tmp333=0.017453293f*tmp321*tmp332;
tmp334=tmp319+tmp333;
tmp335=tmp312*tmp334;
dresidual3_dRIGHTARM_ELBOW_=tmp335;
tmp336=tmp211+0.52359878f;
tmp337=sin(tmp336);
tmp338=tmp10+0.78539816f;
tmp339=cos(tmp338);
tmp340=tmp0+(-0.78539816f);
tmp341=cos(tmp340);
tmp342=sin(tmp22);
tmp343=tmp341*tmp342;
tmp344=cos(tmp22);
tmp345=tmp0+(-0.78539816f);
tmp346=sin(tmp345);
tmp347=tmp6+(-0.78539816f);
tmp348=sin(tmp347);
tmp349=(-1.0f)*tmp344*tmp346*tmp348;
tmp350=tmp343+tmp349;
tmp351=tmp339*tmp350;
tmp352=tmp6+(-0.78539816f);
tmp353=cos(tmp352);
tmp354=tmp10+0.78539816f;
tmp355=sin(tmp354);
tmp356=tmp0+(-0.78539816f);
tmp357=sin(tmp356);
tmp358=(-1.0f)*tmp353*tmp355*tmp357;
tmp359=tmp351+tmp358;
tmp360=(-0.017453293f)*tmp337*tmp359;
tmp361=tmp211+0.52359878f;
tmp362=cos(tmp361);
tmp363=tmp0+(-0.78539816f);
tmp364=cos(tmp363);
tmp365=cos(tmp22);
tmp366=tmp364*tmp365;
tmp367=tmp0+(-0.78539816f);
tmp368=sin(tmp367);
tmp369=tmp6+(-0.78539816f);
tmp370=sin(tmp369);
tmp371=sin(tmp22);
tmp372=tmp368*tmp370*tmp371;
tmp373=tmp366+tmp372;
tmp374=0.017453293f*tmp362*tmp373;
tmp375=tmp360+tmp374;
dresidual3_dRIGHTARM_FOREARM_ROTATE_=tmp375;
tmp376=tmp10+0.78539816f;
tmp377=sin(tmp376);
tmp378=tmp0+(-0.78539816f);
tmp379=cos(tmp378);
tmp380=cos(tmp22);
tmp381=tmp6+(-0.78539816f);
tmp382=sin(tmp381);
tmp383=(-0.017453293f)*tmp379*tmp380*tmp382;
tmp384=tmp0+(-0.78539816f);
tmp385=sin(tmp384);
tmp386=sin(tmp22);
tmp387=(-0.017453293f)*tmp385*tmp386;
tmp388=tmp383+tmp387;
tmp389=tmp377*tmp388;
tmp390=tmp10+0.78539816f;
tmp391=cos(tmp390);
tmp392=tmp0+(-0.78539816f);
tmp393=cos(tmp392);
tmp394=tmp6+(-0.78539816f);
tmp395=cos(tmp394);
tmp396=0.017453293f*tmp391*tmp393*tmp395;
tmp397=tmp389+tmp396;
dresidual4_dRIGHTARM_SHOULDER_IO_=tmp397;
tmp398=tmp10+0.78539816f;
tmp399=cos(tmp398);
tmp400=tmp0+(-0.78539816f);
tmp401=sin(tmp400);
tmp402=tmp6+(-0.78539816f);
tmp403=sin(tmp402);
tmp404=(-0.017453293f)*tmp399*tmp401*tmp403;
tmp405=tmp6+(-0.78539816f);
tmp406=cos(tmp405);
tmp407=cos(tmp22);
tmp408=tmp10+0.78539816f;
tmp409=sin(tmp408);
tmp410=tmp0+(-0.78539816f);
tmp411=sin(tmp410);
tmp412=(-0.017453293f)*tmp406*tmp407*tmp409*tmp411;
tmp413=tmp404+tmp412;
dresidual4_dRIGHTARM_SHOULDER_UD_=tmp413;
tmp414=tmp10+0.78539816f;
tmp415=sin(tmp414);
tmp416=tmp0+(-0.78539816f);
tmp417=cos(tmp416);
tmp418=cos(tmp22);
tmp419=0.017453293f*tmp417*tmp418;
tmp420=tmp0+(-0.78539816f);
tmp421=sin(tmp420);
tmp422=tmp6+(-0.78539816f);
tmp423=sin(tmp422);
tmp424=sin(tmp22);
tmp425=0.017453293f*tmp421*tmp423*tmp424;
tmp426=tmp419+tmp425;
tmp427=tmp415*tmp426;
dresidual4_dRIGHTARM_BICEP_ROTATE_=tmp427;
tmp428=tmp10+0.78539816f;
tmp429=cos(tmp428);
tmp430=tmp0+(-0.78539816f);
tmp431=cos(tmp430);
tmp432=sin(tmp22);
tmp433=tmp431*tmp432;
tmp434=cos(tmp22);
tmp435=tmp0+(-0.78539816f);
tmp436=sin(tmp435);
tmp437=tmp6+(-0.78539816f);
tmp438=sin(tmp437);
tmp439=(-1.0f)*tmp434*tmp436*tmp438;
tmp440=tmp433+tmp439;
tmp441=(-0.017453293f)*tmp429*tmp440;
tmp442=tmp6+(-0.78539816f);
tmp443=cos(tmp442);
tmp444=tmp10+0.78539816f;
tmp445=sin(tmp444);
tmp446=tmp0+(-0.78539816f);
tmp447=sin(tmp446);
tmp448=0.017453293f*tmp443*tmp445*tmp447;
tmp449=tmp441+tmp448;
dresidual4_dRIGHTARM_ELBOW_=tmp449;
dresidual4_dRIGHTARM_FOREARM_ROTATE_=0.0f;
} // ik_rightarmdf()
