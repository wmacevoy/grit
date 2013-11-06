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
  "RIGHT_SHOULDER_IO",
  "RIGHT_SHOULDER_UD",
  "RIGHT_BICEP_ROTATE",
  "RIGHT_ELBOW",
  "RIGHT_FOREARM_ROTATE",
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
#define RIGHT_SHOULDER_IO ik_rightarmparameters[0]
#define RIGHT_SHOULDER_UD ik_rightarmparameters[1]
#define RIGHT_BICEP_ROTATE ik_rightarmparameters[2]
#define RIGHT_ELBOW ik_rightarmparameters[3]
#define RIGHT_FOREARM_ROTATE ik_rightarmparameters[4]
#define nx ik_rightarmparameters[5]
#define ny ik_rightarmparameters[6]
#define px ik_rightarmparameters[7]
#define py ik_rightarmparameters[8]
#define pz ik_rightarmparameters[9]
#define residual ik_rightarmparameters[10]

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
  const float ik_rightarmparameters[11],
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
  float ik_rightarmparameters[11]
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
float tmp149;
float tmp150;
float tmp151;
float tmp152;
float tmp153;
tmp0=(-1.0f)*px;
tmp1=0.017453293f*RIGHT_SHOULDER_IO_;
tmp2=sin(tmp1);
tmp3=3.625f*tmp2;
tmp4=0.017453293f*RIGHT_SHOULDER_UD_;
tmp5=cos(tmp4);
tmp6=sin(tmp1);
tmp7=9.0f*tmp5*tmp6;
tmp8=0.017453293f*RIGHT_ELBOW_;
tmp9=cos(tmp8);
tmp10=cos(tmp4);
tmp11=sin(tmp1);
tmp12=15.0f*tmp9*tmp10*tmp11;
tmp13=sin(tmp8);
tmp14=cos(tmp1);
tmp15=0.017453293f*RIGHT_BICEP_ROTATE_;
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
tmp57=0.017453293f*RIGHT_FOREARM_ROTATE_;
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
tmp74=tmp58*tmp73;
tmp75=sin(tmp57);
tmp76=cos(tmp15);
tmp77=cos(tmp1);
tmp78=tmp76*tmp77;
tmp79=sin(tmp15);
tmp80=sin(tmp1);
tmp81=sin(tmp4);
tmp82=tmp79*tmp80*tmp81;
tmp83=tmp78+tmp82;
tmp84=tmp75*tmp83;
tmp85=(-1.0f)*nx;
tmp86=sin(tmp1);
tmp87=3.625f*tmp86;
tmp88=cos(tmp4);
tmp89=sin(tmp1);
tmp90=9.0f*tmp88*tmp89;
tmp91=cos(tmp8);
tmp92=cos(tmp4);
tmp93=sin(tmp1);
tmp94=15.0f*tmp91*tmp92*tmp93;
tmp95=sin(tmp8);
tmp96=cos(tmp1);
tmp97=sin(tmp15);
tmp98=tmp96*tmp97;
tmp99=cos(tmp15);
tmp100=sin(tmp1);
tmp101=sin(tmp4);
tmp102=(-1.0f)*tmp99*tmp100*tmp101;
tmp103=tmp98+tmp102;
tmp104=15.0f*tmp95*tmp103;
tmp105=tmp74+tmp84+tmp85+tmp87+tmp90+tmp94+tmp104+6.0f;
residual3=tmp105;
tmp106=cos(tmp57);
tmp107=cos(tmp8);
tmp108=cos(tmp15);
tmp109=cos(tmp1);
tmp110=sin(tmp4);
tmp111=(-1.0f)*tmp108*tmp109*tmp110;
tmp112=sin(tmp15);
tmp113=sin(tmp1);
tmp114=(-1.0f)*tmp112*tmp113;
tmp115=tmp111+tmp114;
tmp116=tmp107*tmp115;
tmp117=cos(tmp1);
tmp118=cos(tmp4);
tmp119=sin(tmp8);
tmp120=(-1.0f)*tmp117*tmp118*tmp119;
tmp121=tmp116+tmp120;
tmp122=tmp106*tmp121;
tmp123=sin(tmp57);
tmp124=cos(tmp1);
tmp125=sin(tmp15);
tmp126=sin(tmp4);
tmp127=tmp124*tmp125*tmp126;
tmp128=cos(tmp15);
tmp129=sin(tmp1);
tmp130=(-1.0f)*tmp128*tmp129;
tmp131=tmp127+tmp130;
tmp132=tmp123*tmp131;
tmp133=(-1.0f)*ny;
tmp134=cos(tmp1);
tmp135=3.625f*tmp134;
tmp136=cos(tmp1);
tmp137=cos(tmp4);
tmp138=9.0f*tmp136*tmp137;
tmp139=cos(tmp8);
tmp140=cos(tmp1);
tmp141=cos(tmp4);
tmp142=15.0f*tmp139*tmp140*tmp141;
tmp143=sin(tmp8);
tmp144=cos(tmp15);
tmp145=cos(tmp1);
tmp146=sin(tmp4);
tmp147=(-1.0f)*tmp144*tmp145*tmp146;
tmp148=sin(tmp15);
tmp149=sin(tmp1);
tmp150=(-1.0f)*tmp148*tmp149;
tmp151=tmp147+tmp150;
tmp152=15.0f*tmp143*tmp151;
tmp153=tmp122+tmp132+tmp133+tmp135+tmp138+tmp142+tmp152+2.375f;
residual4=tmp153;
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
tmp0=0.017453293f*RIGHT_SHOULDER_IO_;
tmp1=cos(tmp0);
tmp2=0.063268185f*tmp1;
tmp3=cos(tmp0);
tmp4=0.017453293f*RIGHT_SHOULDER_UD_;
tmp5=cos(tmp4);
tmp6=0.15707963f*tmp3*tmp5;
tmp7=0.017453293f*RIGHT_ELBOW_;
tmp8=cos(tmp7);
tmp9=cos(tmp0);
tmp10=cos(tmp4);
tmp11=0.26179939f*tmp8*tmp9*tmp10;
tmp12=sin(tmp7);
tmp13=0.017453293f*RIGHT_BICEP_ROTATE_;
tmp14=cos(tmp13);
tmp15=cos(tmp0);
tmp16=sin(tmp4);
tmp17=(-0.017453293f)*tmp14*tmp15*tmp16;
tmp18=sin(tmp13);
tmp19=sin(tmp0);
tmp20=(-0.017453293f)*tmp18*tmp19;
tmp21=tmp17+tmp20;
tmp22=15.0f*tmp12*tmp21;
tmp23=tmp2+tmp6+tmp11+tmp22;
dresidual0_dRIGHT_SHOULDER_IO_=tmp23;
tmp24=cos(tmp13);
tmp25=cos(tmp4);
tmp26=sin(tmp7);
tmp27=sin(tmp0);
tmp28=(-0.26179939f)*tmp24*tmp25*tmp26*tmp27;
tmp29=cos(tmp7);
tmp30=sin(tmp0);
tmp31=sin(tmp4);
tmp32=(-0.26179939f)*tmp29*tmp30*tmp31;
tmp33=sin(tmp0);
tmp34=sin(tmp4);
tmp35=(-0.15707963f)*tmp33*tmp34;
tmp36=tmp28+tmp32+tmp35;
dresidual0_dRIGHT_SHOULDER_UD_=tmp36;
tmp37=sin(tmp7);
tmp38=cos(tmp13);
tmp39=cos(tmp0);
tmp40=0.017453293f*tmp38*tmp39;
tmp41=sin(tmp13);
tmp42=sin(tmp0);
tmp43=sin(tmp4);
tmp44=0.017453293f*tmp41*tmp42*tmp43;
tmp45=tmp40+tmp44;
tmp46=15.0f*tmp37*tmp45;
dresidual0_dRIGHT_BICEP_ROTATE_=tmp46;
tmp47=cos(tmp4);
tmp48=sin(tmp7);
tmp49=sin(tmp0);
tmp50=(-0.26179939f)*tmp47*tmp48*tmp49;
tmp51=cos(tmp7);
tmp52=cos(tmp0);
tmp53=sin(tmp13);
tmp54=tmp52*tmp53;
tmp55=cos(tmp13);
tmp56=sin(tmp0);
tmp57=sin(tmp4);
tmp58=(-1.0f)*tmp55*tmp56*tmp57;
tmp59=tmp54+tmp58;
tmp60=0.26179939f*tmp51*tmp59;
tmp61=tmp50+tmp60;
dresidual0_dRIGHT_ELBOW_=tmp61;
dresidual0_dRIGHT_FOREARM_ROTATE_=0.0f;
tmp62=cos(tmp7);
tmp63=cos(tmp4);
tmp64=sin(tmp0);
tmp65=(-0.26179939f)*tmp62*tmp63*tmp64;
tmp66=cos(tmp4);
tmp67=sin(tmp0);
tmp68=(-0.15707963f)*tmp66*tmp67;
tmp69=sin(tmp0);
tmp70=(-0.063268185f)*tmp69;
tmp71=sin(tmp7);
tmp72=cos(tmp0);
tmp73=sin(tmp13);
tmp74=(-0.017453293f)*tmp72*tmp73;
tmp75=cos(tmp13);
tmp76=sin(tmp0);
tmp77=sin(tmp4);
tmp78=0.017453293f*tmp75*tmp76*tmp77;
tmp79=tmp74+tmp78;
tmp80=15.0f*tmp71*tmp79;
tmp81=tmp65+tmp68+tmp70+tmp80;
dresidual1_dRIGHT_SHOULDER_IO_=tmp81;
tmp82=cos(tmp13);
tmp83=cos(tmp0);
tmp84=cos(tmp4);
tmp85=sin(tmp7);
tmp86=(-0.26179939f)*tmp82*tmp83*tmp84*tmp85;
tmp87=cos(tmp7);
tmp88=cos(tmp0);
tmp89=sin(tmp4);
tmp90=(-0.26179939f)*tmp87*tmp88*tmp89;
tmp91=cos(tmp0);
tmp92=sin(tmp4);
tmp93=(-0.15707963f)*tmp91*tmp92;
tmp94=tmp86+tmp90+tmp93;
dresidual1_dRIGHT_SHOULDER_UD_=tmp94;
tmp95=sin(tmp7);
tmp96=cos(tmp13);
tmp97=sin(tmp0);
tmp98=(-0.017453293f)*tmp96*tmp97;
tmp99=cos(tmp0);
tmp100=sin(tmp13);
tmp101=sin(tmp4);
tmp102=0.017453293f*tmp99*tmp100*tmp101;
tmp103=tmp98+tmp102;
tmp104=15.0f*tmp95*tmp103;
dresidual1_dRIGHT_BICEP_ROTATE_=tmp104;
tmp105=cos(tmp0);
tmp106=cos(tmp4);
tmp107=sin(tmp7);
tmp108=(-0.26179939f)*tmp105*tmp106*tmp107;
tmp109=cos(tmp7);
tmp110=cos(tmp13);
tmp111=cos(tmp0);
tmp112=sin(tmp4);
tmp113=(-1.0f)*tmp110*tmp111*tmp112;
tmp114=sin(tmp13);
tmp115=sin(tmp0);
tmp116=(-1.0f)*tmp114*tmp115;
tmp117=tmp113+tmp116;
tmp118=0.26179939f*tmp109*tmp117;
tmp119=tmp108+tmp118;
dresidual1_dRIGHT_ELBOW_=tmp119;
dresidual1_dRIGHT_FOREARM_ROTATE_=0.0f;
dresidual2_dRIGHT_SHOULDER_IO_=0.0f;
tmp120=cos(tmp13);
tmp121=sin(tmp7);
tmp122=sin(tmp4);
tmp123=(-0.26179939f)*tmp120*tmp121*tmp122;
tmp124=cos(tmp4);
tmp125=0.15707963f*tmp124;
tmp126=cos(tmp7);
tmp127=cos(tmp4);
tmp128=0.26179939f*tmp126*tmp127;
tmp129=tmp123+tmp125+tmp128;
dresidual2_dRIGHT_SHOULDER_UD_=tmp129;
tmp130=cos(tmp4);
tmp131=sin(tmp13);
tmp132=sin(tmp7);
tmp133=(-0.26179939f)*tmp130*tmp131*tmp132;
dresidual2_dRIGHT_BICEP_ROTATE_=tmp133;
tmp134=sin(tmp7);
tmp135=sin(tmp4);
tmp136=(-0.26179939f)*tmp134*tmp135;
tmp137=cos(tmp13);
tmp138=cos(tmp7);
tmp139=cos(tmp4);
tmp140=0.26179939f*tmp137*tmp138*tmp139;
tmp141=tmp136+tmp140;
dresidual2_dRIGHT_ELBOW_=tmp141;
dresidual2_dRIGHT_FOREARM_ROTATE_=0.0f;
tmp142=0.017453293f*RIGHT_FOREARM_ROTATE_;
tmp143=cos(tmp142);
tmp144=cos(tmp7);
tmp145=cos(tmp13);
tmp146=cos(tmp0);
tmp147=sin(tmp4);
tmp148=(-0.017453293f)*tmp145*tmp146*tmp147;
tmp149=sin(tmp13);
tmp150=sin(tmp0);
tmp151=(-0.017453293f)*tmp149*tmp150;
tmp152=tmp148+tmp151;
tmp153=tmp144*tmp152;
tmp154=cos(tmp0);
tmp155=cos(tmp4);
tmp156=sin(tmp7);
tmp157=(-0.017453293f)*tmp154*tmp155*tmp156;
tmp158=tmp153+tmp157;
tmp159=tmp143*tmp158;
tmp160=sin(tmp142);
tmp161=cos(tmp13);
tmp162=sin(tmp0);
tmp163=(-0.017453293f)*tmp161*tmp162;
tmp164=cos(tmp0);
tmp165=sin(tmp13);
tmp166=sin(tmp4);
tmp167=0.017453293f*tmp164*tmp165*tmp166;
tmp168=tmp163+tmp167;
tmp169=tmp160*tmp168;
tmp170=cos(tmp0);
tmp171=0.063268185f*tmp170;
tmp172=cos(tmp0);
tmp173=cos(tmp4);
tmp174=0.15707963f*tmp172*tmp173;
tmp175=cos(tmp7);
tmp176=cos(tmp0);
tmp177=cos(tmp4);
tmp178=0.26179939f*tmp175*tmp176*tmp177;
tmp179=sin(tmp7);
tmp180=cos(tmp13);
tmp181=cos(tmp0);
tmp182=sin(tmp4);
tmp183=(-0.017453293f)*tmp180*tmp181*tmp182;
tmp184=sin(tmp13);
tmp185=sin(tmp0);
tmp186=(-0.017453293f)*tmp184*tmp185;
tmp187=tmp183+tmp186;
tmp188=15.0f*tmp179*tmp187;
tmp189=tmp159+tmp169+tmp171+tmp174+tmp178+tmp188;
dresidual3_dRIGHT_SHOULDER_IO_=tmp189;
tmp190=cos(tmp142);
tmp191=cos(tmp13);
tmp192=cos(tmp7);
tmp193=cos(tmp4);
tmp194=sin(tmp0);
tmp195=(-0.017453293f)*tmp191*tmp192*tmp193*tmp194;
tmp196=sin(tmp7);
tmp197=sin(tmp0);
tmp198=sin(tmp4);
tmp199=0.017453293f*tmp196*tmp197*tmp198;
tmp200=tmp195+tmp199;
tmp201=tmp190*tmp200;
tmp202=cos(tmp13);
tmp203=cos(tmp4);
tmp204=sin(tmp7);
tmp205=sin(tmp0);
tmp206=(-0.26179939f)*tmp202*tmp203*tmp204*tmp205;
tmp207=cos(tmp7);
tmp208=sin(tmp0);
tmp209=sin(tmp4);
tmp210=(-0.26179939f)*tmp207*tmp208*tmp209;
tmp211=sin(tmp0);
tmp212=sin(tmp4);
tmp213=(-0.15707963f)*tmp211*tmp212;
tmp214=cos(tmp4);
tmp215=sin(tmp13);
tmp216=sin(tmp142);
tmp217=sin(tmp0);
tmp218=0.017453293f*tmp214*tmp215*tmp216*tmp217;
tmp219=tmp201+tmp206+tmp210+tmp213+tmp218;
dresidual3_dRIGHT_SHOULDER_UD_=tmp219;
tmp220=cos(tmp7);
tmp221=cos(tmp142);
tmp222=cos(tmp13);
tmp223=cos(tmp0);
tmp224=0.017453293f*tmp222*tmp223;
tmp225=sin(tmp13);
tmp226=sin(tmp0);
tmp227=sin(tmp4);
tmp228=0.017453293f*tmp225*tmp226*tmp227;
tmp229=tmp224+tmp228;
tmp230=tmp220*tmp221*tmp229;
tmp231=sin(tmp142);
tmp232=cos(tmp0);
tmp233=sin(tmp13);
tmp234=(-0.017453293f)*tmp232*tmp233;
tmp235=cos(tmp13);
tmp236=sin(tmp0);
tmp237=sin(tmp4);
tmp238=0.017453293f*tmp235*tmp236*tmp237;
tmp239=tmp234+tmp238;
tmp240=tmp231*tmp239;
tmp241=tmp230+tmp240+dresidual0_dRIGHT_BICEP_ROTATE_;
dresidual3_dRIGHT_BICEP_ROTATE_=tmp241;
tmp242=cos(tmp142);
tmp243=cos(tmp7);
tmp244=cos(tmp4);
tmp245=sin(tmp0);
tmp246=(-0.017453293f)*tmp243*tmp244*tmp245;
tmp247=sin(tmp7);
tmp248=cos(tmp0);
tmp249=sin(tmp13);
tmp250=tmp248*tmp249;
tmp251=cos(tmp13);
tmp252=sin(tmp0);
tmp253=sin(tmp4);
tmp254=(-1.0f)*tmp251*tmp252*tmp253;
tmp255=tmp250+tmp254;
tmp256=(-0.017453293f)*tmp247*tmp255;
tmp257=tmp246+tmp256;
tmp258=tmp242*tmp257;
tmp259=cos(tmp4);
tmp260=sin(tmp7);
tmp261=sin(tmp0);
tmp262=(-0.26179939f)*tmp259*tmp260*tmp261;
tmp263=cos(tmp7);
tmp264=cos(tmp0);
tmp265=sin(tmp13);
tmp266=tmp264*tmp265;
tmp267=cos(tmp13);
tmp268=sin(tmp0);
tmp269=sin(tmp4);
tmp270=(-1.0f)*tmp267*tmp268*tmp269;
tmp271=tmp266+tmp270;
tmp272=0.26179939f*tmp263*tmp271;
tmp273=tmp258+tmp262+tmp272;
dresidual3_dRIGHT_ELBOW_=tmp273;
tmp274=sin(tmp142);
tmp275=cos(tmp7);
tmp276=cos(tmp0);
tmp277=sin(tmp13);
tmp278=tmp276*tmp277;
tmp279=cos(tmp13);
tmp280=sin(tmp0);
tmp281=sin(tmp4);
tmp282=(-1.0f)*tmp279*tmp280*tmp281;
tmp283=tmp278+tmp282;
tmp284=tmp275*tmp283;
tmp285=cos(tmp4);
tmp286=sin(tmp7);
tmp287=sin(tmp0);
tmp288=(-1.0f)*tmp285*tmp286*tmp287;
tmp289=tmp284+tmp288;
tmp290=(-0.017453293f)*tmp274*tmp289;
tmp291=cos(tmp142);
tmp292=cos(tmp13);
tmp293=cos(tmp0);
tmp294=tmp292*tmp293;
tmp295=sin(tmp13);
tmp296=sin(tmp0);
tmp297=sin(tmp4);
tmp298=tmp295*tmp296*tmp297;
tmp299=tmp294+tmp298;
tmp300=0.017453293f*tmp291*tmp299;
tmp301=tmp290+tmp300;
dresidual3_dRIGHT_FOREARM_ROTATE_=tmp301;
tmp302=cos(tmp142);
tmp303=cos(tmp7);
tmp304=cos(tmp0);
tmp305=sin(tmp13);
tmp306=(-0.017453293f)*tmp304*tmp305;
tmp307=cos(tmp13);
tmp308=sin(tmp0);
tmp309=sin(tmp4);
tmp310=0.017453293f*tmp307*tmp308*tmp309;
tmp311=tmp306+tmp310;
tmp312=tmp303*tmp311;
tmp313=cos(tmp4);
tmp314=sin(tmp7);
tmp315=sin(tmp0);
tmp316=0.017453293f*tmp313*tmp314*tmp315;
tmp317=tmp312+tmp316;
tmp318=tmp302*tmp317;
tmp319=sin(tmp142);
tmp320=cos(tmp13);
tmp321=cos(tmp0);
tmp322=(-0.017453293f)*tmp320*tmp321;
tmp323=sin(tmp13);
tmp324=sin(tmp0);
tmp325=sin(tmp4);
tmp326=(-0.017453293f)*tmp323*tmp324*tmp325;
tmp327=tmp322+tmp326;
tmp328=tmp319*tmp327;
tmp329=cos(tmp7);
tmp330=cos(tmp4);
tmp331=sin(tmp0);
tmp332=(-0.26179939f)*tmp329*tmp330*tmp331;
tmp333=cos(tmp4);
tmp334=sin(tmp0);
tmp335=(-0.15707963f)*tmp333*tmp334;
tmp336=sin(tmp0);
tmp337=(-0.063268185f)*tmp336;
tmp338=sin(tmp7);
tmp339=cos(tmp0);
tmp340=sin(tmp13);
tmp341=(-0.017453293f)*tmp339*tmp340;
tmp342=cos(tmp13);
tmp343=sin(tmp0);
tmp344=sin(tmp4);
tmp345=0.017453293f*tmp342*tmp343*tmp344;
tmp346=tmp341+tmp345;
tmp347=15.0f*tmp338*tmp346;
tmp348=tmp318+tmp328+tmp332+tmp335+tmp337+tmp347;
dresidual4_dRIGHT_SHOULDER_IO_=tmp348;
tmp349=cos(tmp142);
tmp350=cos(tmp13);
tmp351=cos(tmp7);
tmp352=cos(tmp0);
tmp353=cos(tmp4);
tmp354=(-0.017453293f)*tmp350*tmp351*tmp352*tmp353;
tmp355=cos(tmp0);
tmp356=sin(tmp7);
tmp357=sin(tmp4);
tmp358=0.017453293f*tmp355*tmp356*tmp357;
tmp359=tmp354+tmp358;
tmp360=tmp349*tmp359;
tmp361=cos(tmp13);
tmp362=cos(tmp0);
tmp363=cos(tmp4);
tmp364=sin(tmp7);
tmp365=(-0.26179939f)*tmp361*tmp362*tmp363*tmp364;
tmp366=cos(tmp7);
tmp367=cos(tmp0);
tmp368=sin(tmp4);
tmp369=(-0.26179939f)*tmp366*tmp367*tmp368;
tmp370=cos(tmp0);
tmp371=sin(tmp4);
tmp372=(-0.15707963f)*tmp370*tmp371;
tmp373=cos(tmp0);
tmp374=cos(tmp4);
tmp375=sin(tmp13);
tmp376=sin(tmp142);
tmp377=0.017453293f*tmp373*tmp374*tmp375*tmp376;
tmp378=tmp360+tmp365+tmp369+tmp372+tmp377;
dresidual4_dRIGHT_SHOULDER_UD_=tmp378;
tmp379=cos(tmp7);
tmp380=cos(tmp142);
tmp381=cos(tmp13);
tmp382=sin(tmp0);
tmp383=(-0.017453293f)*tmp381*tmp382;
tmp384=cos(tmp0);
tmp385=sin(tmp13);
tmp386=sin(tmp4);
tmp387=0.017453293f*tmp384*tmp385*tmp386;
tmp388=tmp383+tmp387;
tmp389=tmp379*tmp380*tmp388;
tmp390=sin(tmp142);
tmp391=cos(tmp13);
tmp392=cos(tmp0);
tmp393=sin(tmp4);
tmp394=0.017453293f*tmp391*tmp392*tmp393;
tmp395=sin(tmp13);
tmp396=sin(tmp0);
tmp397=0.017453293f*tmp395*tmp396;
tmp398=tmp394+tmp397;
tmp399=tmp390*tmp398;
tmp400=tmp389+tmp399+dresidual1_dRIGHT_BICEP_ROTATE_;
dresidual4_dRIGHT_BICEP_ROTATE_=tmp400;
tmp401=cos(tmp142);
tmp402=cos(tmp7);
tmp403=cos(tmp0);
tmp404=cos(tmp4);
tmp405=(-0.017453293f)*tmp402*tmp403*tmp404;
tmp406=sin(tmp7);
tmp407=cos(tmp13);
tmp408=cos(tmp0);
tmp409=sin(tmp4);
tmp410=(-1.0f)*tmp407*tmp408*tmp409;
tmp411=sin(tmp13);
tmp412=sin(tmp0);
tmp413=(-1.0f)*tmp411*tmp412;
tmp414=tmp410+tmp413;
tmp415=(-0.017453293f)*tmp406*tmp414;
tmp416=tmp405+tmp415;
tmp417=tmp401*tmp416;
tmp418=cos(tmp0);
tmp419=cos(tmp4);
tmp420=sin(tmp7);
tmp421=(-0.26179939f)*tmp418*tmp419*tmp420;
tmp422=cos(tmp7);
tmp423=cos(tmp13);
tmp424=cos(tmp0);
tmp425=sin(tmp4);
tmp426=(-1.0f)*tmp423*tmp424*tmp425;
tmp427=sin(tmp13);
tmp428=sin(tmp0);
tmp429=(-1.0f)*tmp427*tmp428;
tmp430=tmp426+tmp429;
tmp431=0.26179939f*tmp422*tmp430;
tmp432=tmp417+tmp421+tmp431;
dresidual4_dRIGHT_ELBOW_=tmp432;
tmp433=sin(tmp142);
tmp434=cos(tmp7);
tmp435=cos(tmp13);
tmp436=cos(tmp0);
tmp437=sin(tmp4);
tmp438=(-1.0f)*tmp435*tmp436*tmp437;
tmp439=sin(tmp13);
tmp440=sin(tmp0);
tmp441=(-1.0f)*tmp439*tmp440;
tmp442=tmp438+tmp441;
tmp443=tmp434*tmp442;
tmp444=cos(tmp0);
tmp445=cos(tmp4);
tmp446=sin(tmp7);
tmp447=(-1.0f)*tmp444*tmp445*tmp446;
tmp448=tmp443+tmp447;
tmp449=(-0.017453293f)*tmp433*tmp448;
tmp450=cos(tmp142);
tmp451=cos(tmp0);
tmp452=sin(tmp13);
tmp453=sin(tmp4);
tmp454=tmp451*tmp452*tmp453;
tmp455=cos(tmp13);
tmp456=sin(tmp0);
tmp457=(-1.0f)*tmp455*tmp456;
tmp458=tmp454+tmp457;
tmp459=0.017453293f*tmp450*tmp458;
tmp460=tmp449+tmp459;
dresidual4_dRIGHT_FOREARM_ROTATE_=tmp460;
} // ik_rightarmdf()
