#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <cassert>
enum { ik_rightarmglobal_count=2 };
extern const char *ik_rightarmglobal_names[2];

enum { ik_rightarmparameter_count=16 };
extern const char *ik_rightarmparameter_names[16];

enum { ik_rightarmvariable_count=5 };
extern const char *ik_rightarmvariable_names[5];

enum { ik_rightarmequation_count=5 };
extern const char *ik_rightarmequation_names[5];
const char *ik_rightarmglobal_names[]={
  "epsilon",
  "steps"
};

const char *ik_rightarmparameter_names[]={
  "RIGHT_SHOULDER_IO",
  "RIGHT_SHOULDER_UD",
  "RIGHT_BICEP_ROTATE",
  "RIGHT_ELBOW",
  "RIGHT_FOREARM_ROTATE",
  "RIGHTARM_BICEP_ROTATE",
  "RIGHTARM_ELBOW",
  "RIGHTARM_FOREARM_ROTATE",
  "RIGHTARM_SHOULDER_IO",
  "RIGHTARM_SHOULDER_UD",
  "nx",
  "ny",
  "px",
  "py",
  "pz",
  "residual"
};

const char *ik_rightarmvariable_names[]={
  "RIGHT_SHOULDER_IO_",
  "RIGHT_SHOULDER_UD_",
  "RIGHT_BICEP_ROTATE_",
  "RIGHT_ELBOW_",
  "RIGHT_FOREARM_ROTATE_"
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
  const float ik_rightarmparameters[16],
  const float ik_rightarmx[5],
  float ik_rightarmy[5]
);
void ik_rightarmdf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[16],
  const float ik_rightarmx[5],
  float ik_rightarmdy[25]
);
void ik_rightarminitialize(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[16],
  float ik_rightarmx[5]
);
void ik_rightarmupdate(
  const float ik_rightarmglobals[2],
  float ik_rightarmparameters[16]
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
    parameters[15]=residual;
  } //  ik_rightarmsolve()
typedef struct {
  const char *ik_rightarmname;
  float ik_rightarmglobals[2];
  float ik_rightarmparameters[16];
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
  float ik_rightarmparameters[16];
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
#define RIGHT_SHOULDER_IO ik_rightarmparameters[0]
#define RIGHT_SHOULDER_UD ik_rightarmparameters[1]
#define RIGHT_BICEP_ROTATE ik_rightarmparameters[2]
#define RIGHT_ELBOW ik_rightarmparameters[3]
#define RIGHT_FOREARM_ROTATE ik_rightarmparameters[4]
#define RIGHTARM_BICEP_ROTATE ik_rightarmparameters[5]
#define RIGHTARM_ELBOW ik_rightarmparameters[6]
#define RIGHTARM_FOREARM_ROTATE ik_rightarmparameters[7]
#define RIGHTARM_SHOULDER_IO ik_rightarmparameters[8]
#define RIGHTARM_SHOULDER_UD ik_rightarmparameters[9]
#define nx ik_rightarmparameters[10]
#define ny ik_rightarmparameters[11]
#define px ik_rightarmparameters[12]
#define py ik_rightarmparameters[13]
#define pz ik_rightarmparameters[14]
#define residual ik_rightarmparameters[15]

// variable aliases
#define RIGHT_SHOULDER_IO_ ik_rightarmx[0]
#define RIGHT_SHOULDER_UD_ ik_rightarmx[1]
#define RIGHT_BICEP_ROTATE_ ik_rightarmx[2]
#define RIGHT_ELBOW_ ik_rightarmx[3]
#define RIGHT_FOREARM_ROTATE_ ik_rightarmx[4]

// residual aliases
#define residual0 ik_rightarmy[0]
#define residual1 ik_rightarmy[1]
#define residual2 ik_rightarmy[2]
#define residual3 ik_rightarmy[3]
#define residual4 ik_rightarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_dRIGHT_SHOULDER_IO_ ik_rightarmdy[0]
#define dresidual0_dRIGHT_SHOULDER_UD_ ik_rightarmdy[5]
#define dresidual0_dRIGHT_BICEP_ROTATE_ ik_rightarmdy[10]
#define dresidual0_dRIGHT_ELBOW_ ik_rightarmdy[15]
#define dresidual0_dRIGHT_FOREARM_ROTATE_ ik_rightarmdy[20]
#define dresidual1_dRIGHT_SHOULDER_IO_ ik_rightarmdy[1]
#define dresidual1_dRIGHT_SHOULDER_UD_ ik_rightarmdy[6]
#define dresidual1_dRIGHT_BICEP_ROTATE_ ik_rightarmdy[11]
#define dresidual1_dRIGHT_ELBOW_ ik_rightarmdy[16]
#define dresidual1_dRIGHT_FOREARM_ROTATE_ ik_rightarmdy[21]
#define dresidual2_dRIGHT_SHOULDER_IO_ ik_rightarmdy[2]
#define dresidual2_dRIGHT_SHOULDER_UD_ ik_rightarmdy[7]
#define dresidual2_dRIGHT_BICEP_ROTATE_ ik_rightarmdy[12]
#define dresidual2_dRIGHT_ELBOW_ ik_rightarmdy[17]
#define dresidual2_dRIGHT_FOREARM_ROTATE_ ik_rightarmdy[22]
#define dresidual3_dRIGHT_SHOULDER_IO_ ik_rightarmdy[3]
#define dresidual3_dRIGHT_SHOULDER_UD_ ik_rightarmdy[8]
#define dresidual3_dRIGHT_BICEP_ROTATE_ ik_rightarmdy[13]
#define dresidual3_dRIGHT_ELBOW_ ik_rightarmdy[18]
#define dresidual3_dRIGHT_FOREARM_ROTATE_ ik_rightarmdy[23]
#define dresidual4_dRIGHT_SHOULDER_IO_ ik_rightarmdy[4]
#define dresidual4_dRIGHT_SHOULDER_UD_ ik_rightarmdy[9]
#define dresidual4_dRIGHT_BICEP_ROTATE_ ik_rightarmdy[14]
#define dresidual4_dRIGHT_ELBOW_ ik_rightarmdy[19]
#define dresidual4_dRIGHT_FOREARM_ROTATE_ ik_rightarmdy[24]

void ik_rightarminitialize(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[16],
  float ik_rightarmx[5]
)
{
  // initialize unknowns from parameters
  // RIGHT_SHOULDER_IO_=RIGHT_SHOULDER_IO;
  RIGHT_SHOULDER_IO_=RIGHT_SHOULDER_IO;
  // RIGHT_SHOULDER_UD_=RIGHT_SHOULDER_UD;
  RIGHT_SHOULDER_UD_=RIGHT_SHOULDER_UD;
  // RIGHT_BICEP_ROTATE_=RIGHT_BICEP_ROTATE;
  RIGHT_BICEP_ROTATE_=RIGHT_BICEP_ROTATE;
  // RIGHT_ELBOW_=RIGHT_ELBOW;
  RIGHT_ELBOW_=RIGHT_ELBOW;
  // RIGHT_FOREARM_ROTATE_=RIGHT_FOREARM_ROTATE;
  RIGHT_FOREARM_ROTATE_=RIGHT_FOREARM_ROTATE;
} // ik_rightarminitialize()
void ik_rightarmupdate(
  const float ik_rightarmglobals[2],
  float ik_rightarmparameters[16]
)
{
  float ik_rightarmx[5];
  ik_rightarmsolve(ik_rightarmglobals,ik_rightarmparameters,ik_rightarmx);

  // RIGHT_SHOULDER_IO=RIGHT_SHOULDER_IO_
  RIGHT_SHOULDER_IO=RIGHT_SHOULDER_IO_;
  // RIGHT_SHOULDER_UD=RIGHT_SHOULDER_UD_
  RIGHT_SHOULDER_UD=RIGHT_SHOULDER_UD_;
  // RIGHT_BICEP_ROTATE=RIGHT_BICEP_ROTATE_
  RIGHT_BICEP_ROTATE=RIGHT_BICEP_ROTATE_;
  // RIGHT_ELBOW=RIGHT_ELBOW_
  RIGHT_ELBOW=RIGHT_ELBOW_;
  // RIGHT_FOREARM_ROTATE=RIGHT_FOREARM_ROTATE_
  RIGHT_FOREARM_ROTATE=RIGHT_FOREARM_ROTATE_;
} // ik_rightarmupdate()

void ik_rightarmf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[16],
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
tmp0=(-1.0f)*px;
tmp1=0.017453293f*RIGHTARM_SHOULDER_IO;
tmp2=sin(tmp1);
tmp3=3.625f*tmp2;
tmp4=0.017453293f*RIGHTARM_SHOULDER_UD;
tmp5=cos(tmp4);
tmp6=sin(tmp1);
tmp7=9.0f*tmp5*tmp6;
tmp8=0.017453293f*RIGHTARM_ELBOW;
tmp9=cos(tmp8);
tmp10=cos(tmp4);
tmp11=sin(tmp1);
tmp12=15.0f*tmp9*tmp10*tmp11;
tmp13=sin(tmp8);
tmp14=cos(tmp1);
tmp15=0.017453293f*RIGHTARM_BICEP_ROTATE;
tmp16=sin(tmp15);
tmp17=tmp14*tmp16;
tmp18=cos(tmp15);
tmp19=sin(tmp1);
tmp20=sin(tmp4);
tmp21=(-1.0f)*tmp18*tmp19*tmp20;
tmp22=tmp17+tmp21;
tmp23=15.0f*tmp13*tmp22;
tmp24=tmp0+tmp3+tmp7+tmp12+tmp23+6.0f;
residual0=tmp24;
tmp25=(-1.0f)*py;
tmp26=cos(tmp1);
tmp27=3.625f*tmp26;
tmp28=cos(tmp1);
tmp29=cos(tmp4);
tmp30=9.0f*tmp28*tmp29;
tmp31=cos(tmp8);
tmp32=cos(tmp1);
tmp33=cos(tmp4);
tmp34=15.0f*tmp31*tmp32*tmp33;
tmp35=sin(tmp8);
tmp36=cos(tmp15);
tmp37=cos(tmp1);
tmp38=sin(tmp4);
tmp39=(-1.0f)*tmp36*tmp37*tmp38;
tmp40=sin(tmp15);
tmp41=sin(tmp1);
tmp42=(-1.0f)*tmp40*tmp41;
tmp43=tmp39+tmp42;
tmp44=15.0f*tmp35*tmp43;
tmp45=tmp25+tmp27+tmp30+tmp34+tmp44+2.375f;
residual1=tmp45;
tmp46=(-1.0f)*pz;
tmp47=sin(tmp4);
tmp48=9.0f*tmp47;
tmp49=cos(tmp15);
tmp50=cos(tmp4);
tmp51=sin(tmp8);
tmp52=15.0f*tmp49*tmp50*tmp51;
tmp53=cos(tmp8);
tmp54=sin(tmp4);
tmp55=15.0f*tmp53*tmp54;
tmp56=tmp46+tmp48+tmp52+tmp55;
residual2=tmp56;
tmp57=0.017453293f*RIGHTARM_FOREARM_ROTATE;
tmp58=cos(tmp57);
tmp59=cos(tmp8);
tmp60=cos(tmp1);
tmp61=sin(tmp15);
tmp62=tmp60*tmp61;
tmp63=cos(tmp15);
tmp64=sin(tmp1);
tmp65=sin(tmp4);
tmp66=(-1.0f)*tmp63*tmp64*tmp65;
tmp67=tmp62+tmp66;
tmp68=tmp59*tmp67;
tmp69=cos(tmp4);
tmp70=sin(tmp8);
tmp71=sin(tmp1);
tmp72=(-1.0f)*tmp69*tmp70*tmp71;
tmp73=tmp68+tmp72;
tmp74=(-1.0f)*tmp58*tmp73;
tmp75=sin(tmp57);
tmp76=cos(tmp15);
tmp77=cos(tmp1);
tmp78=tmp76*tmp77;
tmp79=sin(tmp15);
tmp80=sin(tmp1);
tmp81=sin(tmp4);
tmp82=tmp79*tmp80*tmp81;
tmp83=tmp78+tmp82;
tmp84=(-1.0f)*tmp75*tmp83;
tmp85=(-1.0f)*nx;
tmp86=tmp74+tmp84+tmp85;
residual3=tmp86;
tmp87=cos(tmp57);
tmp88=cos(tmp8);
tmp89=cos(tmp15);
tmp90=cos(tmp1);
tmp91=sin(tmp4);
tmp92=(-1.0f)*tmp89*tmp90*tmp91;
tmp93=sin(tmp15);
tmp94=sin(tmp1);
tmp95=(-1.0f)*tmp93*tmp94;
tmp96=tmp92+tmp95;
tmp97=tmp88*tmp96;
tmp98=cos(tmp1);
tmp99=cos(tmp4);
tmp100=sin(tmp8);
tmp101=(-1.0f)*tmp98*tmp99*tmp100;
tmp102=tmp97+tmp101;
tmp103=(-1.0f)*tmp87*tmp102;
tmp104=sin(tmp57);
tmp105=cos(tmp1);
tmp106=sin(tmp15);
tmp107=sin(tmp4);
tmp108=tmp105*tmp106*tmp107;
tmp109=cos(tmp15);
tmp110=sin(tmp1);
tmp111=(-1.0f)*tmp109*tmp110;
tmp112=tmp108+tmp111;
tmp113=(-1.0f)*tmp104*tmp112;
tmp114=(-1.0f)*ny;
tmp115=tmp103+tmp113+tmp114;
residual4=tmp115;
} // ik_rightarmf()

void ik_rightarmdf(
  const float ik_rightarmglobals[2],
  const float ik_rightarmparameters[16],
  const float ik_rightarmx[5],
  float ik_rightarmdy[25]
)
{
dresidual0_dRIGHT_SHOULDER_IO_=0.0f;
dresidual0_dRIGHT_SHOULDER_UD_=0.0f;
dresidual0_dRIGHT_BICEP_ROTATE_=0.0f;
dresidual0_dRIGHT_ELBOW_=0.0f;
dresidual0_dRIGHT_FOREARM_ROTATE_=0.0f;
dresidual1_dRIGHT_SHOULDER_IO_=0.0f;
dresidual1_dRIGHT_SHOULDER_UD_=0.0f;
dresidual1_dRIGHT_BICEP_ROTATE_=0.0f;
dresidual1_dRIGHT_ELBOW_=0.0f;
dresidual1_dRIGHT_FOREARM_ROTATE_=0.0f;
dresidual2_dRIGHT_SHOULDER_IO_=0.0f;
dresidual2_dRIGHT_SHOULDER_UD_=0.0f;
dresidual2_dRIGHT_BICEP_ROTATE_=0.0f;
dresidual2_dRIGHT_ELBOW_=0.0f;
dresidual2_dRIGHT_FOREARM_ROTATE_=0.0f;
dresidual3_dRIGHT_SHOULDER_IO_=0.0f;
dresidual3_dRIGHT_SHOULDER_UD_=0.0f;
dresidual3_dRIGHT_BICEP_ROTATE_=0.0f;
dresidual3_dRIGHT_ELBOW_=0.0f;
dresidual3_dRIGHT_FOREARM_ROTATE_=0.0f;
dresidual4_dRIGHT_SHOULDER_IO_=0.0f;
dresidual4_dRIGHT_SHOULDER_UD_=0.0f;
dresidual4_dRIGHT_BICEP_ROTATE_=0.0f;
dresidual4_dRIGHT_ELBOW_=0.0f;
dresidual4_dRIGHT_FOREARM_ROTATE_=0.0f;
} // ik_rightarmdf()
