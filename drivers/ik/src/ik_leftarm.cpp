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
  "shoulderio",
  "shoulderud",
  "bicep",
  "elbow",
  "forearm",
  "leftpx",
  "leftpy",
  "leftpz",
  "leftroll",
  "leftyaw",
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
#define shoulderio ik_leftarmparameters[0]
#define shoulderud ik_leftarmparameters[1]
#define bicep ik_leftarmparameters[2]
#define elbow ik_leftarmparameters[3]
#define forearm ik_leftarmparameters[4]
#define leftpx ik_leftarmparameters[5]
#define leftpy ik_leftarmparameters[6]
#define leftpz ik_leftarmparameters[7]
#define leftroll ik_leftarmparameters[8]
#define leftyaw ik_leftarmparameters[9]
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
#define residual4 ik_leftarmy[4]

// jacobian aliases (fortran order)
#define dresidual0_d_shoulderio ik_leftarmdy[0]
#define dresidual0_d_shoulderud ik_leftarmdy[5]
#define dresidual0_d_bicep ik_leftarmdy[10]
#define dresidual0_d_elbow ik_leftarmdy[15]
#define dresidual0_d_forearm ik_leftarmdy[20]
#define dresidual1_d_shoulderio ik_leftarmdy[1]
#define dresidual1_d_shoulderud ik_leftarmdy[6]
#define dresidual1_d_bicep ik_leftarmdy[11]
#define dresidual1_d_elbow ik_leftarmdy[16]
#define dresidual1_d_forearm ik_leftarmdy[21]
#define dresidual2_d_shoulderio ik_leftarmdy[2]
#define dresidual2_d_shoulderud ik_leftarmdy[7]
#define dresidual2_d_bicep ik_leftarmdy[12]
#define dresidual2_d_elbow ik_leftarmdy[17]
#define dresidual2_d_forearm ik_leftarmdy[22]
#define dresidual3_d_shoulderio ik_leftarmdy[3]
#define dresidual3_d_shoulderud ik_leftarmdy[8]
#define dresidual3_d_bicep ik_leftarmdy[13]
#define dresidual3_d_elbow ik_leftarmdy[18]
#define dresidual3_d_forearm ik_leftarmdy[23]
#define dresidual4_d_shoulderio ik_leftarmdy[4]
#define dresidual4_d_shoulderud ik_leftarmdy[9]
#define dresidual4_d_bicep ik_leftarmdy[14]
#define dresidual4_d_elbow ik_leftarmdy[19]
#define dresidual4_d_forearm ik_leftarmdy[24]

void ik_leftarminitialize(
  const float ik_leftarmglobals[2],
  const float ik_leftarmparameters[11],
  float ik_leftarmx[5]
)
{
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
} // ik_leftarminitialize()
void ik_leftarmupdate(
  const float ik_leftarmglobals[2],
  float ik_leftarmparameters[11]
)
{
  float ik_leftarmx[5];
  ik_leftarmsolve(ik_leftarmglobals,ik_leftarmparameters,ik_leftarmx);

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
tmp0=0.017453293f*_elbow;
tmp1=tmp0+(-0.78539816f);
tmp2=sin(tmp1);
tmp3=0.017453293f*_shoulderio;
tmp4=tmp3+(-0.78539816f);
tmp5=cos(tmp4);
tmp6=0.017453293f*_bicep;
tmp7=sin(tmp6);
tmp8=tmp5*tmp7;
tmp9=cos(tmp6);
tmp10=(-0.017453293f)*_shoulderud;
tmp11=tmp10+0.78539816f;
tmp12=sin(tmp11);
tmp13=tmp3+(-0.78539816f);
tmp14=sin(tmp13);
tmp15=tmp9*tmp12*tmp14;
tmp16=tmp8+tmp15;
tmp17=(-15.0f)*tmp2*tmp16;
tmp18=(-1.0f)*leftpx;
tmp19=tmp3+(-0.78539816f);
tmp20=sin(tmp19);
tmp21=3.625f*tmp20;
tmp22=tmp10+0.78539816f;
tmp23=cos(tmp22);
tmp24=tmp3+(-0.78539816f);
tmp25=sin(tmp24);
tmp26=9.0f*tmp23*tmp25;
tmp27=tmp10+0.78539816f;
tmp28=cos(tmp27);
tmp29=tmp0+(-0.78539816f);
tmp30=cos(tmp29);
tmp31=tmp3+(-0.78539816f);
tmp32=sin(tmp31);
tmp33=15.0f*tmp28*tmp30*tmp32;
tmp34=tmp17+tmp18+tmp21+tmp26+tmp33+(-6.0f);
residual0=tmp34;
tmp35=tmp0+(-0.78539816f);
tmp36=sin(tmp35);
tmp37=tmp3+(-0.78539816f);
tmp38=cos(tmp37);
tmp39=cos(tmp6);
tmp40=tmp10+0.78539816f;
tmp41=sin(tmp40);
tmp42=tmp38*tmp39*tmp41;
tmp43=tmp3+(-0.78539816f);
tmp44=sin(tmp43);
tmp45=sin(tmp6);
tmp46=(-1.0f)*tmp44*tmp45;
tmp47=tmp42+tmp46;
tmp48=(-15.0f)*tmp36*tmp47;
tmp49=(-1.0f)*leftpy;
tmp50=tmp3+(-0.78539816f);
tmp51=cos(tmp50);
tmp52=3.625f*tmp51;
tmp53=tmp10+0.78539816f;
tmp54=cos(tmp53);
tmp55=tmp3+(-0.78539816f);
tmp56=cos(tmp55);
tmp57=9.0f*tmp54*tmp56;
tmp58=tmp10+0.78539816f;
tmp59=cos(tmp58);
tmp60=tmp0+(-0.78539816f);
tmp61=cos(tmp60);
tmp62=tmp3+(-0.78539816f);
tmp63=cos(tmp62);
tmp64=15.0f*tmp59*tmp61*tmp63;
tmp65=tmp48+tmp49+tmp52+tmp57+tmp64+2.375f;
residual1=tmp65;
tmp66=tmp10+0.78539816f;
tmp67=cos(tmp66);
tmp68=cos(tmp6);
tmp69=tmp0+(-0.78539816f);
tmp70=sin(tmp69);
tmp71=(-15.0f)*tmp67*tmp68*tmp70;
tmp72=tmp0+(-0.78539816f);
tmp73=cos(tmp72);
tmp74=tmp10+0.78539816f;
tmp75=sin(tmp74);
tmp76=(-15.0f)*tmp73*tmp75;
tmp77=tmp10+0.78539816f;
tmp78=sin(tmp77);
tmp79=(-9.0f)*tmp78;
tmp80=(-1.0f)*leftpz;
tmp81=tmp71+tmp76+tmp79+tmp80;
residual2=tmp81;
tmp82=0.017453293f*_forearm;
tmp83=tmp82+0.52359878f;
tmp84=cos(tmp83);
tmp85=tmp10+0.78539816f;
tmp86=cos(tmp85);
tmp87=tmp0+(-0.78539816f);
tmp88=sin(tmp87);
tmp89=tmp3+(-0.78539816f);
tmp90=sin(tmp89);
tmp91=tmp86*tmp88*tmp90;
tmp92=tmp0+(-0.78539816f);
tmp93=cos(tmp92);
tmp94=tmp3+(-0.78539816f);
tmp95=cos(tmp94);
tmp96=sin(tmp6);
tmp97=tmp95*tmp96;
tmp98=cos(tmp6);
tmp99=tmp10+0.78539816f;
tmp100=sin(tmp99);
tmp101=tmp3+(-0.78539816f);
tmp102=sin(tmp101);
tmp103=tmp98*tmp100*tmp102;
tmp104=tmp97+tmp103;
tmp105=tmp93*tmp104;
tmp106=tmp91+tmp105;
tmp107=tmp84*tmp106;
tmp108=tmp82+0.52359878f;
tmp109=sin(tmp108);
tmp110=tmp3+(-0.78539816f);
tmp111=cos(tmp110);
tmp112=cos(tmp6);
tmp113=tmp111*tmp112;
tmp114=tmp10+0.78539816f;
tmp115=sin(tmp114);
tmp116=tmp3+(-0.78539816f);
tmp117=sin(tmp116);
tmp118=sin(tmp6);
tmp119=(-1.0f)*tmp115*tmp117*tmp118;
tmp120=tmp113+tmp119;
tmp121=tmp109*tmp120;
tmp122=0.017453293f*leftroll;
tmp123=sin(tmp122);
tmp124=(-1.0f)*tmp123;
tmp125=tmp107+tmp121+tmp124;
residual3=tmp125;
tmp126=tmp10+0.78539816f;
tmp127=cos(tmp126);
tmp128=tmp0+(-0.78539816f);
tmp129=cos(tmp128);
tmp130=tmp3+(-0.78539816f);
tmp131=sin(tmp130);
tmp132=tmp127*tmp129*tmp131;
tmp133=tmp0+(-0.78539816f);
tmp134=sin(tmp133);
tmp135=tmp3+(-0.78539816f);
tmp136=cos(tmp135);
tmp137=sin(tmp6);
tmp138=tmp136*tmp137;
tmp139=cos(tmp6);
tmp140=tmp10+0.78539816f;
tmp141=sin(tmp140);
tmp142=tmp3+(-0.78539816f);
tmp143=sin(tmp142);
tmp144=tmp139*tmp141*tmp143;
tmp145=tmp138+tmp144;
tmp146=(-1.0f)*tmp134*tmp145;
tmp147=0.017453293f*leftyaw;
tmp148=sin(tmp147);
tmp149=(-1.0f)*tmp148;
tmp150=tmp132+tmp146+tmp149;
residual4=tmp150;
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
tmp0=0.017453293f*_elbow;
tmp1=tmp0+(-0.78539816f);
tmp2=sin(tmp1);
tmp3=0.017453293f*_shoulderio;
tmp4=tmp3+(-0.78539816f);
tmp5=sin(tmp4);
tmp6=0.017453293f*_bicep;
tmp7=sin(tmp6);
tmp8=(-0.017453293f)*tmp5*tmp7;
tmp9=tmp3+(-0.78539816f);
tmp10=cos(tmp9);
tmp11=cos(tmp6);
tmp12=(-0.017453293f)*_shoulderud;
tmp13=tmp12+0.78539816f;
tmp14=sin(tmp13);
tmp15=0.017453293f*tmp10*tmp11*tmp14;
tmp16=tmp8+tmp15;
tmp17=(-15.0f)*tmp2*tmp16;
tmp18=tmp3+(-0.78539816f);
tmp19=cos(tmp18);
tmp20=0.063268185f*tmp19;
tmp21=tmp12+0.78539816f;
tmp22=cos(tmp21);
tmp23=tmp3+(-0.78539816f);
tmp24=cos(tmp23);
tmp25=0.15707963f*tmp22*tmp24;
tmp26=tmp12+0.78539816f;
tmp27=cos(tmp26);
tmp28=tmp0+(-0.78539816f);
tmp29=cos(tmp28);
tmp30=tmp3+(-0.78539816f);
tmp31=cos(tmp30);
tmp32=0.26179939f*tmp27*tmp29*tmp31;
tmp33=tmp17+tmp20+tmp25+tmp32;
dresidual0_d_shoulderio=tmp33;
tmp34=tmp12+0.78539816f;
tmp35=sin(tmp34);
tmp36=tmp3+(-0.78539816f);
tmp37=sin(tmp36);
tmp38=0.15707963f*tmp35*tmp37;
tmp39=tmp12+0.78539816f;
tmp40=cos(tmp39);
tmp41=cos(tmp6);
tmp42=tmp0+(-0.78539816f);
tmp43=sin(tmp42);
tmp44=tmp3+(-0.78539816f);
tmp45=sin(tmp44);
tmp46=0.26179939f*tmp40*tmp41*tmp43*tmp45;
tmp47=tmp0+(-0.78539816f);
tmp48=cos(tmp47);
tmp49=tmp12+0.78539816f;
tmp50=sin(tmp49);
tmp51=tmp3+(-0.78539816f);
tmp52=sin(tmp51);
tmp53=0.26179939f*tmp48*tmp50*tmp52;
tmp54=tmp38+tmp46+tmp53;
dresidual0_d_shoulderud=tmp54;
tmp55=tmp0+(-0.78539816f);
tmp56=sin(tmp55);
tmp57=tmp12+0.78539816f;
tmp58=sin(tmp57);
tmp59=tmp3+(-0.78539816f);
tmp60=sin(tmp59);
tmp61=sin(tmp6);
tmp62=(-0.017453293f)*tmp58*tmp60*tmp61;
tmp63=tmp3+(-0.78539816f);
tmp64=cos(tmp63);
tmp65=cos(tmp6);
tmp66=0.017453293f*tmp64*tmp65;
tmp67=tmp62+tmp66;
tmp68=(-15.0f)*tmp56*tmp67;
dresidual0_d_bicep=tmp68;
tmp69=tmp12+0.78539816f;
tmp70=cos(tmp69);
tmp71=tmp0+(-0.78539816f);
tmp72=sin(tmp71);
tmp73=tmp3+(-0.78539816f);
tmp74=sin(tmp73);
tmp75=(-0.26179939f)*tmp70*tmp72*tmp74;
tmp76=tmp0+(-0.78539816f);
tmp77=cos(tmp76);
tmp78=tmp3+(-0.78539816f);
tmp79=cos(tmp78);
tmp80=sin(tmp6);
tmp81=tmp79*tmp80;
tmp82=cos(tmp6);
tmp83=tmp12+0.78539816f;
tmp84=sin(tmp83);
tmp85=tmp3+(-0.78539816f);
tmp86=sin(tmp85);
tmp87=tmp82*tmp84*tmp86;
tmp88=tmp81+tmp87;
tmp89=(-0.26179939f)*tmp77*tmp88;
tmp90=tmp75+tmp89;
dresidual0_d_elbow=tmp90;
dresidual0_d_forearm=0.0f;
tmp91=tmp0+(-0.78539816f);
tmp92=sin(tmp91);
tmp93=tmp3+(-0.78539816f);
tmp94=cos(tmp93);
tmp95=sin(tmp6);
tmp96=(-0.017453293f)*tmp94*tmp95;
tmp97=cos(tmp6);
tmp98=tmp12+0.78539816f;
tmp99=sin(tmp98);
tmp100=tmp3+(-0.78539816f);
tmp101=sin(tmp100);
tmp102=(-0.017453293f)*tmp97*tmp99*tmp101;
tmp103=tmp96+tmp102;
tmp104=(-15.0f)*tmp92*tmp103;
tmp105=tmp12+0.78539816f;
tmp106=cos(tmp105);
tmp107=tmp0+(-0.78539816f);
tmp108=cos(tmp107);
tmp109=tmp3+(-0.78539816f);
tmp110=sin(tmp109);
tmp111=(-0.26179939f)*tmp106*tmp108*tmp110;
tmp112=tmp12+0.78539816f;
tmp113=cos(tmp112);
tmp114=tmp3+(-0.78539816f);
tmp115=sin(tmp114);
tmp116=(-0.15707963f)*tmp113*tmp115;
tmp117=tmp3+(-0.78539816f);
tmp118=sin(tmp117);
tmp119=(-0.063268185f)*tmp118;
tmp120=tmp104+tmp111+tmp116+tmp119;
dresidual1_d_shoulderio=tmp120;
tmp121=tmp3+(-0.78539816f);
tmp122=cos(tmp121);
tmp123=tmp12+0.78539816f;
tmp124=sin(tmp123);
tmp125=0.15707963f*tmp122*tmp124;
tmp126=tmp12+0.78539816f;
tmp127=cos(tmp126);
tmp128=tmp3+(-0.78539816f);
tmp129=cos(tmp128);
tmp130=cos(tmp6);
tmp131=tmp0+(-0.78539816f);
tmp132=sin(tmp131);
tmp133=0.26179939f*tmp127*tmp129*tmp130*tmp132;
tmp134=tmp0+(-0.78539816f);
tmp135=cos(tmp134);
tmp136=tmp3+(-0.78539816f);
tmp137=cos(tmp136);
tmp138=tmp12+0.78539816f;
tmp139=sin(tmp138);
tmp140=0.26179939f*tmp135*tmp137*tmp139;
tmp141=tmp125+tmp133+tmp140;
dresidual1_d_shoulderud=tmp141;
tmp142=tmp0+(-0.78539816f);
tmp143=sin(tmp142);
tmp144=tmp3+(-0.78539816f);
tmp145=cos(tmp144);
tmp146=tmp12+0.78539816f;
tmp147=sin(tmp146);
tmp148=sin(tmp6);
tmp149=(-0.017453293f)*tmp145*tmp147*tmp148;
tmp150=cos(tmp6);
tmp151=tmp3+(-0.78539816f);
tmp152=sin(tmp151);
tmp153=(-0.017453293f)*tmp150*tmp152;
tmp154=tmp149+tmp153;
tmp155=(-15.0f)*tmp143*tmp154;
dresidual1_d_bicep=tmp155;
tmp156=tmp12+0.78539816f;
tmp157=cos(tmp156);
tmp158=tmp3+(-0.78539816f);
tmp159=cos(tmp158);
tmp160=tmp0+(-0.78539816f);
tmp161=sin(tmp160);
tmp162=(-0.26179939f)*tmp157*tmp159*tmp161;
tmp163=tmp0+(-0.78539816f);
tmp164=cos(tmp163);
tmp165=tmp3+(-0.78539816f);
tmp166=cos(tmp165);
tmp167=cos(tmp6);
tmp168=tmp12+0.78539816f;
tmp169=sin(tmp168);
tmp170=tmp166*tmp167*tmp169;
tmp171=tmp3+(-0.78539816f);
tmp172=sin(tmp171);
tmp173=sin(tmp6);
tmp174=(-1.0f)*tmp172*tmp173;
tmp175=tmp170+tmp174;
tmp176=(-0.26179939f)*tmp164*tmp175;
tmp177=tmp162+tmp176;
dresidual1_d_elbow=tmp177;
dresidual1_d_forearm=0.0f;
dresidual2_d_shoulderio=0.0f;
tmp178=cos(tmp6);
tmp179=tmp12+0.78539816f;
tmp180=sin(tmp179);
tmp181=tmp0+(-0.78539816f);
tmp182=sin(tmp181);
tmp183=(-0.26179939f)*tmp178*tmp180*tmp182;
tmp184=tmp12+0.78539816f;
tmp185=cos(tmp184);
tmp186=0.15707963f*tmp185;
tmp187=tmp12+0.78539816f;
tmp188=cos(tmp187);
tmp189=tmp0+(-0.78539816f);
tmp190=cos(tmp189);
tmp191=0.26179939f*tmp188*tmp190;
tmp192=tmp183+tmp186+tmp191;
dresidual2_d_shoulderud=tmp192;
tmp193=tmp12+0.78539816f;
tmp194=cos(tmp193);
tmp195=tmp0+(-0.78539816f);
tmp196=sin(tmp195);
tmp197=sin(tmp6);
tmp198=0.26179939f*tmp194*tmp196*tmp197;
dresidual2_d_bicep=tmp198;
tmp199=tmp12+0.78539816f;
tmp200=cos(tmp199);
tmp201=tmp0+(-0.78539816f);
tmp202=cos(tmp201);
tmp203=cos(tmp6);
tmp204=(-0.26179939f)*tmp200*tmp202*tmp203;
tmp205=tmp12+0.78539816f;
tmp206=sin(tmp205);
tmp207=tmp0+(-0.78539816f);
tmp208=sin(tmp207);
tmp209=0.26179939f*tmp206*tmp208;
tmp210=tmp204+tmp209;
dresidual2_d_elbow=tmp210;
dresidual2_d_forearm=0.0f;
tmp211=0.017453293f*_forearm;
tmp212=tmp211+0.52359878f;
tmp213=cos(tmp212);
tmp214=tmp0+(-0.78539816f);
tmp215=cos(tmp214);
tmp216=tmp3+(-0.78539816f);
tmp217=sin(tmp216);
tmp218=sin(tmp6);
tmp219=(-0.017453293f)*tmp217*tmp218;
tmp220=tmp3+(-0.78539816f);
tmp221=cos(tmp220);
tmp222=cos(tmp6);
tmp223=tmp12+0.78539816f;
tmp224=sin(tmp223);
tmp225=0.017453293f*tmp221*tmp222*tmp224;
tmp226=tmp219+tmp225;
tmp227=tmp215*tmp226;
tmp228=tmp12+0.78539816f;
tmp229=cos(tmp228);
tmp230=tmp3+(-0.78539816f);
tmp231=cos(tmp230);
tmp232=tmp0+(-0.78539816f);
tmp233=sin(tmp232);
tmp234=0.017453293f*tmp229*tmp231*tmp233;
tmp235=tmp227+tmp234;
tmp236=tmp213*tmp235;
tmp237=tmp211+0.52359878f;
tmp238=sin(tmp237);
tmp239=tmp3+(-0.78539816f);
tmp240=cos(tmp239);
tmp241=tmp12+0.78539816f;
tmp242=sin(tmp241);
tmp243=sin(tmp6);
tmp244=(-0.017453293f)*tmp240*tmp242*tmp243;
tmp245=cos(tmp6);
tmp246=tmp3+(-0.78539816f);
tmp247=sin(tmp246);
tmp248=(-0.017453293f)*tmp245*tmp247;
tmp249=tmp244+tmp248;
tmp250=tmp238*tmp249;
tmp251=tmp236+tmp250;
dresidual3_d_shoulderio=tmp251;
tmp252=tmp211+0.52359878f;
tmp253=cos(tmp252);
tmp254=tmp12+0.78539816f;
tmp255=cos(tmp254);
tmp256=tmp0+(-0.78539816f);
tmp257=cos(tmp256);
tmp258=cos(tmp6);
tmp259=tmp3+(-0.78539816f);
tmp260=sin(tmp259);
tmp261=(-0.017453293f)*tmp255*tmp257*tmp258*tmp260;
tmp262=tmp12+0.78539816f;
tmp263=sin(tmp262);
tmp264=tmp0+(-0.78539816f);
tmp265=sin(tmp264);
tmp266=tmp3+(-0.78539816f);
tmp267=sin(tmp266);
tmp268=0.017453293f*tmp263*tmp265*tmp267;
tmp269=tmp261+tmp268;
tmp270=tmp253*tmp269;
tmp271=tmp12+0.78539816f;
tmp272=cos(tmp271);
tmp273=tmp211+0.52359878f;
tmp274=sin(tmp273);
tmp275=tmp3+(-0.78539816f);
tmp276=sin(tmp275);
tmp277=sin(tmp6);
tmp278=0.017453293f*tmp272*tmp274*tmp276*tmp277;
tmp279=tmp270+tmp278;
dresidual3_d_shoulderud=tmp279;
tmp280=tmp0+(-0.78539816f);
tmp281=cos(tmp280);
tmp282=tmp211+0.52359878f;
tmp283=cos(tmp282);
tmp284=tmp12+0.78539816f;
tmp285=sin(tmp284);
tmp286=tmp3+(-0.78539816f);
tmp287=sin(tmp286);
tmp288=sin(tmp6);
tmp289=(-0.017453293f)*tmp285*tmp287*tmp288;
tmp290=tmp3+(-0.78539816f);
tmp291=cos(tmp290);
tmp292=cos(tmp6);
tmp293=0.017453293f*tmp291*tmp292;
tmp294=tmp289+tmp293;
tmp295=tmp281*tmp283*tmp294;
tmp296=tmp211+0.52359878f;
tmp297=sin(tmp296);
tmp298=tmp3+(-0.78539816f);
tmp299=cos(tmp298);
tmp300=sin(tmp6);
tmp301=(-0.017453293f)*tmp299*tmp300;
tmp302=cos(tmp6);
tmp303=tmp12+0.78539816f;
tmp304=sin(tmp303);
tmp305=tmp3+(-0.78539816f);
tmp306=sin(tmp305);
tmp307=(-0.017453293f)*tmp302*tmp304*tmp306;
tmp308=tmp301+tmp307;
tmp309=tmp297*tmp308;
tmp310=tmp295+tmp309;
dresidual3_d_bicep=tmp310;
tmp311=tmp211+0.52359878f;
tmp312=cos(tmp311);
tmp313=tmp0+(-0.78539816f);
tmp314=sin(tmp313);
tmp315=tmp3+(-0.78539816f);
tmp316=cos(tmp315);
tmp317=sin(tmp6);
tmp318=tmp316*tmp317;
tmp319=cos(tmp6);
tmp320=tmp12+0.78539816f;
tmp321=sin(tmp320);
tmp322=tmp3+(-0.78539816f);
tmp323=sin(tmp322);
tmp324=tmp319*tmp321*tmp323;
tmp325=tmp318+tmp324;
tmp326=(-0.017453293f)*tmp314*tmp325;
tmp327=tmp12+0.78539816f;
tmp328=cos(tmp327);
tmp329=tmp0+(-0.78539816f);
tmp330=cos(tmp329);
tmp331=tmp3+(-0.78539816f);
tmp332=sin(tmp331);
tmp333=0.017453293f*tmp328*tmp330*tmp332;
tmp334=tmp326+tmp333;
tmp335=tmp312*tmp334;
dresidual3_d_elbow=tmp335;
tmp336=tmp211+0.52359878f;
tmp337=sin(tmp336);
tmp338=tmp12+0.78539816f;
tmp339=cos(tmp338);
tmp340=tmp0+(-0.78539816f);
tmp341=sin(tmp340);
tmp342=tmp3+(-0.78539816f);
tmp343=sin(tmp342);
tmp344=tmp339*tmp341*tmp343;
tmp345=tmp0+(-0.78539816f);
tmp346=cos(tmp345);
tmp347=tmp3+(-0.78539816f);
tmp348=cos(tmp347);
tmp349=sin(tmp6);
tmp350=tmp348*tmp349;
tmp351=cos(tmp6);
tmp352=tmp12+0.78539816f;
tmp353=sin(tmp352);
tmp354=tmp3+(-0.78539816f);
tmp355=sin(tmp354);
tmp356=tmp351*tmp353*tmp355;
tmp357=tmp350+tmp356;
tmp358=tmp346*tmp357;
tmp359=tmp344+tmp358;
tmp360=(-0.017453293f)*tmp337*tmp359;
tmp361=tmp211+0.52359878f;
tmp362=cos(tmp361);
tmp363=tmp3+(-0.78539816f);
tmp364=cos(tmp363);
tmp365=cos(tmp6);
tmp366=tmp364*tmp365;
tmp367=tmp12+0.78539816f;
tmp368=sin(tmp367);
tmp369=tmp3+(-0.78539816f);
tmp370=sin(tmp369);
tmp371=sin(tmp6);
tmp372=(-1.0f)*tmp368*tmp370*tmp371;
tmp373=tmp366+tmp372;
tmp374=0.017453293f*tmp362*tmp373;
tmp375=tmp360+tmp374;
dresidual3_d_forearm=tmp375;
tmp376=tmp0+(-0.78539816f);
tmp377=sin(tmp376);
tmp378=tmp3+(-0.78539816f);
tmp379=sin(tmp378);
tmp380=sin(tmp6);
tmp381=(-0.017453293f)*tmp379*tmp380;
tmp382=tmp3+(-0.78539816f);
tmp383=cos(tmp382);
tmp384=cos(tmp6);
tmp385=tmp12+0.78539816f;
tmp386=sin(tmp385);
tmp387=0.017453293f*tmp383*tmp384*tmp386;
tmp388=tmp381+tmp387;
tmp389=(-1.0f)*tmp377*tmp388;
tmp390=tmp12+0.78539816f;
tmp391=cos(tmp390);
tmp392=tmp0+(-0.78539816f);
tmp393=cos(tmp392);
tmp394=tmp3+(-0.78539816f);
tmp395=cos(tmp394);
tmp396=0.017453293f*tmp391*tmp393*tmp395;
tmp397=tmp389+tmp396;
dresidual4_d_shoulderio=tmp397;
tmp398=tmp12+0.78539816f;
tmp399=cos(tmp398);
tmp400=cos(tmp6);
tmp401=tmp0+(-0.78539816f);
tmp402=sin(tmp401);
tmp403=tmp3+(-0.78539816f);
tmp404=sin(tmp403);
tmp405=0.017453293f*tmp399*tmp400*tmp402*tmp404;
tmp406=tmp0+(-0.78539816f);
tmp407=cos(tmp406);
tmp408=tmp12+0.78539816f;
tmp409=sin(tmp408);
tmp410=tmp3+(-0.78539816f);
tmp411=sin(tmp410);
tmp412=0.017453293f*tmp407*tmp409*tmp411;
tmp413=tmp405+tmp412;
dresidual4_d_shoulderud=tmp413;
tmp414=tmp0+(-0.78539816f);
tmp415=sin(tmp414);
tmp416=tmp12+0.78539816f;
tmp417=sin(tmp416);
tmp418=tmp3+(-0.78539816f);
tmp419=sin(tmp418);
tmp420=sin(tmp6);
tmp421=(-0.017453293f)*tmp417*tmp419*tmp420;
tmp422=tmp3+(-0.78539816f);
tmp423=cos(tmp422);
tmp424=cos(tmp6);
tmp425=0.017453293f*tmp423*tmp424;
tmp426=tmp421+tmp425;
tmp427=(-1.0f)*tmp415*tmp426;
dresidual4_d_bicep=tmp427;
tmp428=tmp12+0.78539816f;
tmp429=cos(tmp428);
tmp430=tmp0+(-0.78539816f);
tmp431=sin(tmp430);
tmp432=tmp3+(-0.78539816f);
tmp433=sin(tmp432);
tmp434=(-0.017453293f)*tmp429*tmp431*tmp433;
tmp435=tmp0+(-0.78539816f);
tmp436=cos(tmp435);
tmp437=tmp3+(-0.78539816f);
tmp438=cos(tmp437);
tmp439=sin(tmp6);
tmp440=tmp438*tmp439;
tmp441=cos(tmp6);
tmp442=tmp12+0.78539816f;
tmp443=sin(tmp442);
tmp444=tmp3+(-0.78539816f);
tmp445=sin(tmp444);
tmp446=tmp441*tmp443*tmp445;
tmp447=tmp440+tmp446;
tmp448=(-0.017453293f)*tmp436*tmp447;
tmp449=tmp434+tmp448;
dresidual4_d_elbow=tmp449;
dresidual4_d_forearm=0.0f;
} // ik_leftarmdf()
