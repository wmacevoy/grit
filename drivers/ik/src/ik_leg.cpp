#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <cassert>
enum { ik_legglobal_count=2 };
extern const char *ik_legglobal_names[2];

enum { ik_legparameter_count=14 };
extern const char *ik_legparameter_names[14];

enum { ik_legvariable_count=3 };
extern const char *ik_legvariable_names[3];

enum { ik_legequation_count=3 };
extern const char *ik_legequation_names[3];
const char *ik_legglobal_names[]={
  "epsilon",
  "steps"
};

const char *ik_legparameter_names[]={
  "knee",
  "femur",
  "hip",
  "chasis",
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

const char *ik_legvariable_names[]={
  "_knee",
  "_femur",
  "_hip"
};

const char *ik_legequation_names[]={
  "residual0",
  "residual1",
  "residual2"
};

class ik_legStopwatch
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
 float ik_legdist2(int dim, const float *x, const float *y)
{
  float sum2=0;
  for (int k=0; k<dim; ++k) {
    float dx=x[k]-y[k];
    sum2 += dx*dx;
  }
  return sum2;
} // dist2()
 float ik_legdist(int dim, const float *x, const float *y)
{
  return sqrt(ik_legdist2(dim,x,y));
} // dist()
 float ik_legnorm2(int dim, const float *x)
{
  float sum2=0;
  for (int k=0; k<dim; ++k) {
    float dx=x[k];
    sum2 += dx*dx;
  }
  return sum2;
} // dist2()
 float ik_legnorm(int dim, const float *x)
{
  return sqrt(ik_legnorm2(dim,x));
} // dist()
 int ik_legDoolittle_LU_Decomposition_with_Pivoting(float *A, int pivot[], int n)
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


 int ik_legDoolittle_LU_with_Pivoting_Solve(float *A, float B[], int pivot[],
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

 void ik_leglinear_solve(int dim, int *ipiv, float *A, float *b, float *x)
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
  
    ik_legDoolittle_LU_Decomposition_with_Pivoting(A,ipiv,dim);
    ik_legDoolittle_LU_with_Pivoting_Solve(A,b,ipiv,x,dim);
}
void ik_legf(
  const float ik_legglobals[2],
  const float ik_legparameters[14],
  const float ik_legx[3],
  float ik_legy[3]
);
void ik_legdf(
  const float ik_legglobals[2],
  const float ik_legparameters[14],
  const float ik_legx[3],
  float ik_legdy[9]
);
void ik_leginitialize(
  const float ik_legglobals[2],
  const float ik_legparameters[14],
  float ik_legx[3]
);
void ik_legupdate(
  const float ik_legglobals[2],
  float ik_legparameters[14]
);
// damped newton solver

   void ik_legsolve(
    
    const float globals[2],
    float *parameters,
    float *x
  )
  {
    ik_leginitialize(globals,parameters,x);
    // newton iteration
    int steps=int(globals[1]);
    float epsilon=globals[0];
    float residual,new_residual;
  
    for (int step=0; step<steps; ++step) {
      float y[3];
      float dx[3];
      float dy[9];
      int iwork[3];
  
      ik_legf(globals,parameters,x,y);
      residual = ik_legnorm(3,y);
      ik_legdf(globals,parameters,x,dy);

      ik_leglinear_solve(3,iwork,dy,y,dx);
      for (int k=0; k<3; ++k) { x[k] -= dx[k]; }

      for (int damping=0; damping < steps; ++damping) {
	    ik_legf(globals,parameters,x,y);
	    new_residual=ik_legnorm(3,y);
	    if (new_residual < residual) break;
	    for (int k=0; k<3; ++k) { dx[k] /= 2; }
	    for (int k=0; k<3; ++k) { x[k] += dx[k] ; }
      }
      residual=new_residual;
      if (residual <= epsilon) break;
    }
    parameters[13]=residual;
  } //  ik_legsolve()
typedef struct {
  const char *ik_legname;
  float ik_legglobals[2];
  float ik_legparameters[14];
  float ik_legx[3];
} ik_legtest_data_t;

ik_legtest_data_t ik_legtest_data[] = {
}; // ik_legtest_data
void ik_legtests(const std::string &ik_legname)
{
  std::string ik_legcase_name;
  std::string ik_legmax_x_error_name="none";
  int ik_legmax_x_error_count=0;
  float ik_legmax_x_error=0;
  float ik_legx_error=0;
  std::string ik_legmax_y_error_name="none";
  int ik_legmax_y_error_count=0;
  float ik_legmax_y_error=0;
  float ik_legy_error=0;

  float ik_legx_test[3];
  float ik_legy_test[3];

  for (int ik_legcase_count=0; ik_legcase_count < 0; ++ik_legcase_count) {
    if (ik_legname == "all" || ik_legname == ik_legtest_data[ik_legcase_count].ik_legname) {
       const char *ik_legname=ik_legtest_data[ik_legcase_count].ik_legname;
       float *ik_legglobals=ik_legtest_data[ik_legcase_count].ik_legglobals;
       float *ik_legparameters=ik_legtest_data[ik_legcase_count].ik_legparameters;
       float *ik_legx=ik_legtest_data[ik_legcase_count].ik_legx;

       ik_legsolve(ik_legglobals,ik_legparameters,ik_legx_test);
       ik_legf(ik_legglobals,ik_legparameters,ik_legx_test,ik_legy_test);
       ik_legx_error=ik_legdist(3,ik_legx,ik_legx_test);
       ik_legy_error=ik_legnorm(3,ik_legy_test);

       if (ik_legx_error > ik_legmax_x_error) {
         ik_legmax_x_error=ik_legx_error;
         ik_legmax_x_error_name=ik_legname;
         ik_legmax_x_error_count=ik_legcase_count;
       }

       if (ik_legy_error > ik_legmax_y_error) {
         ik_legmax_y_error=ik_legy_error;
         ik_legmax_y_error_name=ik_legname;
         ik_legmax_y_error_count=ik_legcase_count;
       }

       std::cout << "test " << (ik_legcase_count+1) << "/" << "\"" << ik_legname << "\"" << ": " << "x_error=" << std::setprecision(15) << ik_legx_error << "," << "y_error=" << std::setprecision(15) << ik_legy_error << std::endl;
     }
   }
   if (ik_legname == "all") {
     std::cout << "test summary: " 
       << "max_x_error=" << std::setprecision(15) << ik_legmax_x_error
          << " from " << ik_legmax_x_error_count << "/" 
          << "\"" << ik_legmax_x_error_name << "\""
       << ","
       << "max_y_error=" << std::setprecision(15) << ik_legmax_y_error
          << " from " << ik_legmax_y_error_count << "/" 
          << "\"" << ik_legmax_y_error_name << "\""
       << std::endl;
   }
}
typedef struct {
  const char *ik_legname;
  float ik_legglobals[2];
  float ik_legparameters[14];
} ik_legrun_data_t;

ik_legrun_data_t ik_legrun_data[] = {
}; // ik_legrun_data
void ik_legruns(const std::string &ik_legname)
{
  std::string ik_legcase_name;
  for (int ik_legcase_count=0; ik_legcase_count < 0; ++ik_legcase_count) {
    if (ik_legname == "all" || ik_legname == ik_legrun_data[ik_legcase_count].ik_legname) {
      const char *ik_legcase_name=ik_legrun_data[ik_legcase_count].ik_legname;
      float *ik_legglobals=ik_legrun_data[ik_legcase_count].ik_legglobals;
      float *ik_legparameters=ik_legrun_data[ik_legcase_count].ik_legparameters;
      float ik_legx[3];

      ik_legsolve(ik_legglobals,ik_legparameters,ik_legx);
      std::cout << "[case] # " << (ik_legcase_count+1) << std::endl;
      std::cout << "name=\"" << ik_legcase_name << "\"" << std::endl;

      for (int i=0; i<ik_legglobal_count; ++i) {
        std::cout << ik_legglobal_names[i] << "=" << std::setprecision(15) << ik_legglobals[i] << std::endl;
      }

      for (int i=0; i<ik_legparameter_count; ++i) {
        std::cout << ik_legparameter_names[i] << "=" << std::setprecision(15) << ik_legparameters[i] << std::endl;
      }

      for (int i=0; i<ik_legvariable_count; ++i) {
        std::cout << ik_legvariable_names[i] << "=" << std::setprecision(15) << ik_legx[i] << std::endl;
      }
    }
  }
} // ik_legruns()
// global aliases
#define epsilon ik_legglobals[0]
#define steps ik_legglobals[1]

// parameter aliases
#define knee ik_legparameters[0]
#define femur ik_legparameters[1]
#define hip ik_legparameters[2]
#define chasis ik_legparameters[3]
#define dfemur2knee ik_legparameters[4]
#define dhip2femur ik_legparameters[5]
#define drchasis2hip ik_legparameters[6]
#define dyknee2tip ik_legparameters[7]
#define dzchasis2hip ik_legparameters[8]
#define dzknee2tip ik_legparameters[9]
#define px ik_legparameters[10]
#define py ik_legparameters[11]
#define pz ik_legparameters[12]
#define residual ik_legparameters[13]

// variable aliases
#define _knee ik_legx[0]
#define _femur ik_legx[1]
#define _hip ik_legx[2]

// residual aliases
#define residual0 ik_legy[0]
#define residual1 ik_legy[1]
#define residual2 ik_legy[2]

// jacobian aliases (fortran order)
#define dresidual0_d_knee ik_legdy[0]
#define dresidual0_d_femur ik_legdy[3]
#define dresidual0_d_hip ik_legdy[6]
#define dresidual1_d_knee ik_legdy[1]
#define dresidual1_d_femur ik_legdy[4]
#define dresidual1_d_hip ik_legdy[7]
#define dresidual2_d_knee ik_legdy[2]
#define dresidual2_d_femur ik_legdy[5]
#define dresidual2_d_hip ik_legdy[8]

void ik_leginitialize(
  const float ik_legglobals[2],
  const float ik_legparameters[14],
  float ik_legx[3]
)
{
  // initialize unknowns from parameters
  // _knee=knee;
  _knee=knee;
  // _femur=femur;
  _femur=femur;
  // _hip=hip;
  _hip=hip;
} // ik_leginitialize()
void ik_legupdate(
  const float ik_legglobals[2],
  float ik_legparameters[14]
)
{
  float ik_legx[3];
  ik_legsolve(ik_legglobals,ik_legparameters,ik_legx);

  // knee=_knee
  knee=_knee;
  // femur=_femur
  femur=_femur;
  // hip=_hip
  hip=_hip;
} // ik_legupdate()

void ik_legf(
  const float ik_legglobals[2],
  const float ik_legparameters[14],
  const float ik_legx[3],
  float ik_legy[3]
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
tmp0=0.017453293f*_femur;
tmp1=cos(tmp0);
tmp2=0.017453293f*_hip;
tmp3=cos(tmp2);
tmp4=0.017453293f*chasis;
tmp5=sin(tmp4);
tmp6=(-1.0f)*tmp3*tmp5;
tmp7=cos(tmp4);
tmp8=sin(tmp2);
tmp9=(-1.0f)*tmp7*tmp8;
tmp10=tmp6+tmp9;
tmp11=tmp1*tmp10*dfemur2knee;
tmp12=cos(tmp0);
tmp13=0.017453293f*_knee;
tmp14=cos(tmp13);
tmp15=cos(tmp2);
tmp16=sin(tmp4);
tmp17=(-1.0f)*tmp15*tmp16;
tmp18=cos(tmp4);
tmp19=sin(tmp2);
tmp20=(-1.0f)*tmp18*tmp19;
tmp21=tmp17+tmp20;
tmp22=tmp12*tmp14*tmp21;
tmp23=sin(tmp0);
tmp24=sin(tmp13);
tmp25=cos(tmp2);
tmp26=sin(tmp4);
tmp27=(-1.0f)*tmp25*tmp26;
tmp28=cos(tmp4);
tmp29=sin(tmp2);
tmp30=(-1.0f)*tmp28*tmp29;
tmp31=tmp27+tmp30;
tmp32=(-1.0f)*tmp23*tmp24*tmp31;
tmp33=tmp22+tmp32;
tmp34=tmp33*dyknee2tip;
tmp35=cos(tmp0);
tmp36=sin(tmp13);
tmp37=cos(tmp2);
tmp38=sin(tmp4);
tmp39=(-1.0f)*tmp37*tmp38;
tmp40=cos(tmp4);
tmp41=sin(tmp2);
tmp42=(-1.0f)*tmp40*tmp41;
tmp43=tmp39+tmp42;
tmp44=(-1.0f)*tmp35*tmp36*tmp43;
tmp45=cos(tmp13);
tmp46=sin(tmp0);
tmp47=cos(tmp2);
tmp48=sin(tmp4);
tmp49=(-1.0f)*tmp47*tmp48;
tmp50=cos(tmp4);
tmp51=sin(tmp2);
tmp52=(-1.0f)*tmp50*tmp51;
tmp53=tmp49+tmp52;
tmp54=(-1.0f)*tmp45*tmp46*tmp53;
tmp55=tmp44+tmp54;
tmp56=tmp55*dzknee2tip;
tmp57=cos(tmp2);
tmp58=sin(tmp4);
tmp59=(-1.0f)*tmp57*tmp58;
tmp60=cos(tmp4);
tmp61=sin(tmp2);
tmp62=(-1.0f)*tmp60*tmp61;
tmp63=tmp59+tmp62;
tmp64=tmp63*dhip2femur;
tmp65=sin(tmp4);
tmp66=(-1.0f)*tmp65*drchasis2hip;
tmp67=(-1.0f)*px;
tmp68=tmp11+tmp34+tmp56+tmp64+tmp66+tmp67;
residual0=tmp68;
tmp69=cos(tmp0);
tmp70=cos(tmp2);
tmp71=cos(tmp4);
tmp72=tmp70*tmp71;
tmp73=sin(tmp2);
tmp74=sin(tmp4);
tmp75=(-1.0f)*tmp73*tmp74;
tmp76=tmp72+tmp75;
tmp77=tmp69*tmp76*dfemur2knee;
tmp78=cos(tmp4);
tmp79=tmp78*drchasis2hip;
tmp80=cos(tmp0);
tmp81=cos(tmp13);
tmp82=cos(tmp2);
tmp83=cos(tmp4);
tmp84=tmp82*tmp83;
tmp85=sin(tmp2);
tmp86=sin(tmp4);
tmp87=(-1.0f)*tmp85*tmp86;
tmp88=tmp84+tmp87;
tmp89=tmp80*tmp81*tmp88;
tmp90=sin(tmp0);
tmp91=sin(tmp13);
tmp92=cos(tmp2);
tmp93=cos(tmp4);
tmp94=tmp92*tmp93;
tmp95=sin(tmp2);
tmp96=sin(tmp4);
tmp97=(-1.0f)*tmp95*tmp96;
tmp98=tmp94+tmp97;
tmp99=(-1.0f)*tmp90*tmp91*tmp98;
tmp100=tmp89+tmp99;
tmp101=tmp100*dyknee2tip;
tmp102=cos(tmp2);
tmp103=cos(tmp4);
tmp104=tmp102*tmp103;
tmp105=sin(tmp2);
tmp106=sin(tmp4);
tmp107=(-1.0f)*tmp105*tmp106;
tmp108=tmp104+tmp107;
tmp109=tmp108*dhip2femur;
tmp110=cos(tmp0);
tmp111=sin(tmp13);
tmp112=cos(tmp2);
tmp113=cos(tmp4);
tmp114=tmp112*tmp113;
tmp115=sin(tmp2);
tmp116=sin(tmp4);
tmp117=(-1.0f)*tmp115*tmp116;
tmp118=tmp114+tmp117;
tmp119=(-1.0f)*tmp110*tmp111*tmp118;
tmp120=cos(tmp13);
tmp121=sin(tmp0);
tmp122=cos(tmp2);
tmp123=cos(tmp4);
tmp124=tmp122*tmp123;
tmp125=sin(tmp2);
tmp126=sin(tmp4);
tmp127=(-1.0f)*tmp125*tmp126;
tmp128=tmp124+tmp127;
tmp129=(-1.0f)*tmp120*tmp121*tmp128;
tmp130=tmp119+tmp129;
tmp131=tmp130*dzknee2tip;
tmp132=(-1.0f)*py;
tmp133=tmp77+tmp79+tmp101+tmp109+tmp131+tmp132;
residual1=tmp133;
tmp134=sin(tmp0);
tmp135=tmp134*dfemur2knee;
tmp136=cos(tmp0);
tmp137=cos(tmp13);
tmp138=tmp136*tmp137;
tmp139=sin(tmp0);
tmp140=sin(tmp13);
tmp141=(-1.0f)*tmp139*tmp140;
tmp142=tmp138+tmp141;
tmp143=tmp142*dzknee2tip;
tmp144=cos(tmp0);
tmp145=sin(tmp13);
tmp146=tmp144*tmp145;
tmp147=cos(tmp13);
tmp148=sin(tmp0);
tmp149=tmp147*tmp148;
tmp150=tmp146+tmp149;
tmp151=tmp150*dyknee2tip;
tmp152=(-1.0f)*pz;
tmp153=tmp135+tmp143+tmp151+tmp152+dzchasis2hip;
residual2=tmp153;
} // ik_legf()

void ik_legdf(
  const float ik_legglobals[2],
  const float ik_legparameters[14],
  const float ik_legx[3],
  float ik_legdy[9]
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
tmp0=0.017453293f*_femur;
tmp1=cos(tmp0);
tmp2=0.017453293f*_knee;
tmp3=cos(tmp2);
tmp4=0.017453293f*_hip;
tmp5=cos(tmp4);
tmp6=0.017453293f*chasis;
tmp7=sin(tmp6);
tmp8=(-1.0f)*tmp5*tmp7;
tmp9=cos(tmp6);
tmp10=sin(tmp4);
tmp11=(-1.0f)*tmp9*tmp10;
tmp12=tmp8+tmp11;
tmp13=(-0.017453293f)*tmp1*tmp3*tmp12;
tmp14=sin(tmp0);
tmp15=sin(tmp2);
tmp16=cos(tmp4);
tmp17=sin(tmp6);
tmp18=(-1.0f)*tmp16*tmp17;
tmp19=cos(tmp6);
tmp20=sin(tmp4);
tmp21=(-1.0f)*tmp19*tmp20;
tmp22=tmp18+tmp21;
tmp23=0.017453293f*tmp14*tmp15*tmp22;
tmp24=tmp13+tmp23;
tmp25=tmp24*dzknee2tip;
tmp26=cos(tmp0);
tmp27=sin(tmp2);
tmp28=cos(tmp4);
tmp29=sin(tmp6);
tmp30=(-1.0f)*tmp28*tmp29;
tmp31=cos(tmp6);
tmp32=sin(tmp4);
tmp33=(-1.0f)*tmp31*tmp32;
tmp34=tmp30+tmp33;
tmp35=(-0.017453293f)*tmp26*tmp27*tmp34;
tmp36=cos(tmp2);
tmp37=sin(tmp0);
tmp38=cos(tmp4);
tmp39=sin(tmp6);
tmp40=(-1.0f)*tmp38*tmp39;
tmp41=cos(tmp6);
tmp42=sin(tmp4);
tmp43=(-1.0f)*tmp41*tmp42;
tmp44=tmp40+tmp43;
tmp45=(-0.017453293f)*tmp36*tmp37*tmp44;
tmp46=tmp35+tmp45;
tmp47=tmp46*dyknee2tip;
tmp48=tmp25+tmp47;
dresidual0_d_knee=tmp48;
tmp49=cos(tmp0);
tmp50=cos(tmp2);
tmp51=cos(tmp4);
tmp52=sin(tmp6);
tmp53=(-1.0f)*tmp51*tmp52;
tmp54=cos(tmp6);
tmp55=sin(tmp4);
tmp56=(-1.0f)*tmp54*tmp55;
tmp57=tmp53+tmp56;
tmp58=(-0.017453293f)*tmp49*tmp50*tmp57;
tmp59=sin(tmp0);
tmp60=sin(tmp2);
tmp61=cos(tmp4);
tmp62=sin(tmp6);
tmp63=(-1.0f)*tmp61*tmp62;
tmp64=cos(tmp6);
tmp65=sin(tmp4);
tmp66=(-1.0f)*tmp64*tmp65;
tmp67=tmp63+tmp66;
tmp68=0.017453293f*tmp59*tmp60*tmp67;
tmp69=tmp58+tmp68;
tmp70=tmp69*dzknee2tip;
tmp71=cos(tmp0);
tmp72=sin(tmp2);
tmp73=cos(tmp4);
tmp74=sin(tmp6);
tmp75=(-1.0f)*tmp73*tmp74;
tmp76=cos(tmp6);
tmp77=sin(tmp4);
tmp78=(-1.0f)*tmp76*tmp77;
tmp79=tmp75+tmp78;
tmp80=(-0.017453293f)*tmp71*tmp72*tmp79;
tmp81=cos(tmp2);
tmp82=sin(tmp0);
tmp83=cos(tmp4);
tmp84=sin(tmp6);
tmp85=(-1.0f)*tmp83*tmp84;
tmp86=cos(tmp6);
tmp87=sin(tmp4);
tmp88=(-1.0f)*tmp86*tmp87;
tmp89=tmp85+tmp88;
tmp90=(-0.017453293f)*tmp81*tmp82*tmp89;
tmp91=tmp80+tmp90;
tmp92=tmp91*dyknee2tip;
tmp93=sin(tmp0);
tmp94=cos(tmp4);
tmp95=sin(tmp6);
tmp96=(-1.0f)*tmp94*tmp95;
tmp97=cos(tmp6);
tmp98=sin(tmp4);
tmp99=(-1.0f)*tmp97*tmp98;
tmp100=tmp96+tmp99;
tmp101=(-0.017453293f)*tmp93*tmp100*dfemur2knee;
tmp102=tmp70+tmp92+tmp101;
dresidual0_d_femur=tmp102;
tmp103=cos(tmp0);
tmp104=cos(tmp4);
tmp105=cos(tmp6);
tmp106=(-0.017453293f)*tmp104*tmp105;
tmp107=sin(tmp4);
tmp108=sin(tmp6);
tmp109=0.017453293f*tmp107*tmp108;
tmp110=tmp106+tmp109;
tmp111=tmp103*tmp110*dfemur2knee;
tmp112=cos(tmp0);
tmp113=cos(tmp2);
tmp114=cos(tmp4);
tmp115=cos(tmp6);
tmp116=(-0.017453293f)*tmp114*tmp115;
tmp117=sin(tmp4);
tmp118=sin(tmp6);
tmp119=0.017453293f*tmp117*tmp118;
tmp120=tmp116+tmp119;
tmp121=tmp112*tmp113*tmp120;
tmp122=sin(tmp0);
tmp123=sin(tmp2);
tmp124=cos(tmp4);
tmp125=cos(tmp6);
tmp126=(-0.017453293f)*tmp124*tmp125;
tmp127=sin(tmp4);
tmp128=sin(tmp6);
tmp129=0.017453293f*tmp127*tmp128;
tmp130=tmp126+tmp129;
tmp131=(-1.0f)*tmp122*tmp123*tmp130;
tmp132=tmp121+tmp131;
tmp133=tmp132*dyknee2tip;
tmp134=cos(tmp0);
tmp135=sin(tmp2);
tmp136=cos(tmp4);
tmp137=cos(tmp6);
tmp138=(-0.017453293f)*tmp136*tmp137;
tmp139=sin(tmp4);
tmp140=sin(tmp6);
tmp141=0.017453293f*tmp139*tmp140;
tmp142=tmp138+tmp141;
tmp143=(-1.0f)*tmp134*tmp135*tmp142;
tmp144=cos(tmp2);
tmp145=sin(tmp0);
tmp146=cos(tmp4);
tmp147=cos(tmp6);
tmp148=(-0.017453293f)*tmp146*tmp147;
tmp149=sin(tmp4);
tmp150=sin(tmp6);
tmp151=0.017453293f*tmp149*tmp150;
tmp152=tmp148+tmp151;
tmp153=(-1.0f)*tmp144*tmp145*tmp152;
tmp154=tmp143+tmp153;
tmp155=tmp154*dzknee2tip;
tmp156=cos(tmp4);
tmp157=cos(tmp6);
tmp158=(-0.017453293f)*tmp156*tmp157;
tmp159=sin(tmp4);
tmp160=sin(tmp6);
tmp161=0.017453293f*tmp159*tmp160;
tmp162=tmp158+tmp161;
tmp163=tmp162*dhip2femur;
tmp164=tmp111+tmp133+tmp155+tmp163;
dresidual0_d_hip=tmp164;
tmp165=cos(tmp0);
tmp166=cos(tmp2);
tmp167=cos(tmp4);
tmp168=cos(tmp6);
tmp169=tmp167*tmp168;
tmp170=sin(tmp4);
tmp171=sin(tmp6);
tmp172=(-1.0f)*tmp170*tmp171;
tmp173=tmp169+tmp172;
tmp174=(-0.017453293f)*tmp165*tmp166*tmp173;
tmp175=sin(tmp0);
tmp176=sin(tmp2);
tmp177=cos(tmp4);
tmp178=cos(tmp6);
tmp179=tmp177*tmp178;
tmp180=sin(tmp4);
tmp181=sin(tmp6);
tmp182=(-1.0f)*tmp180*tmp181;
tmp183=tmp179+tmp182;
tmp184=0.017453293f*tmp175*tmp176*tmp183;
tmp185=tmp174+tmp184;
tmp186=tmp185*dzknee2tip;
tmp187=cos(tmp0);
tmp188=sin(tmp2);
tmp189=cos(tmp4);
tmp190=cos(tmp6);
tmp191=tmp189*tmp190;
tmp192=sin(tmp4);
tmp193=sin(tmp6);
tmp194=(-1.0f)*tmp192*tmp193;
tmp195=tmp191+tmp194;
tmp196=(-0.017453293f)*tmp187*tmp188*tmp195;
tmp197=cos(tmp2);
tmp198=sin(tmp0);
tmp199=cos(tmp4);
tmp200=cos(tmp6);
tmp201=tmp199*tmp200;
tmp202=sin(tmp4);
tmp203=sin(tmp6);
tmp204=(-1.0f)*tmp202*tmp203;
tmp205=tmp201+tmp204;
tmp206=(-0.017453293f)*tmp197*tmp198*tmp205;
tmp207=tmp196+tmp206;
tmp208=tmp207*dyknee2tip;
tmp209=tmp186+tmp208;
dresidual1_d_knee=tmp209;
tmp210=cos(tmp0);
tmp211=cos(tmp2);
tmp212=cos(tmp4);
tmp213=cos(tmp6);
tmp214=tmp212*tmp213;
tmp215=sin(tmp4);
tmp216=sin(tmp6);
tmp217=(-1.0f)*tmp215*tmp216;
tmp218=tmp214+tmp217;
tmp219=(-0.017453293f)*tmp210*tmp211*tmp218;
tmp220=sin(tmp0);
tmp221=sin(tmp2);
tmp222=cos(tmp4);
tmp223=cos(tmp6);
tmp224=tmp222*tmp223;
tmp225=sin(tmp4);
tmp226=sin(tmp6);
tmp227=(-1.0f)*tmp225*tmp226;
tmp228=tmp224+tmp227;
tmp229=0.017453293f*tmp220*tmp221*tmp228;
tmp230=tmp219+tmp229;
tmp231=tmp230*dzknee2tip;
tmp232=cos(tmp0);
tmp233=sin(tmp2);
tmp234=cos(tmp4);
tmp235=cos(tmp6);
tmp236=tmp234*tmp235;
tmp237=sin(tmp4);
tmp238=sin(tmp6);
tmp239=(-1.0f)*tmp237*tmp238;
tmp240=tmp236+tmp239;
tmp241=(-0.017453293f)*tmp232*tmp233*tmp240;
tmp242=cos(tmp2);
tmp243=sin(tmp0);
tmp244=cos(tmp4);
tmp245=cos(tmp6);
tmp246=tmp244*tmp245;
tmp247=sin(tmp4);
tmp248=sin(tmp6);
tmp249=(-1.0f)*tmp247*tmp248;
tmp250=tmp246+tmp249;
tmp251=(-0.017453293f)*tmp242*tmp243*tmp250;
tmp252=tmp241+tmp251;
tmp253=tmp252*dyknee2tip;
tmp254=sin(tmp0);
tmp255=cos(tmp4);
tmp256=cos(tmp6);
tmp257=tmp255*tmp256;
tmp258=sin(tmp4);
tmp259=sin(tmp6);
tmp260=(-1.0f)*tmp258*tmp259;
tmp261=tmp257+tmp260;
tmp262=(-0.017453293f)*tmp254*tmp261*dfemur2knee;
tmp263=tmp231+tmp253+tmp262;
dresidual1_d_femur=tmp263;
tmp264=cos(tmp0);
tmp265=cos(tmp4);
tmp266=sin(tmp6);
tmp267=(-0.017453293f)*tmp265*tmp266;
tmp268=cos(tmp6);
tmp269=sin(tmp4);
tmp270=(-0.017453293f)*tmp268*tmp269;
tmp271=tmp267+tmp270;
tmp272=tmp264*tmp271*dfemur2knee;
tmp273=cos(tmp0);
tmp274=cos(tmp2);
tmp275=cos(tmp4);
tmp276=sin(tmp6);
tmp277=(-0.017453293f)*tmp275*tmp276;
tmp278=cos(tmp6);
tmp279=sin(tmp4);
tmp280=(-0.017453293f)*tmp278*tmp279;
tmp281=tmp277+tmp280;
tmp282=tmp273*tmp274*tmp281;
tmp283=sin(tmp0);
tmp284=sin(tmp2);
tmp285=cos(tmp4);
tmp286=sin(tmp6);
tmp287=(-0.017453293f)*tmp285*tmp286;
tmp288=cos(tmp6);
tmp289=sin(tmp4);
tmp290=(-0.017453293f)*tmp288*tmp289;
tmp291=tmp287+tmp290;
tmp292=(-1.0f)*tmp283*tmp284*tmp291;
tmp293=tmp282+tmp292;
tmp294=tmp293*dyknee2tip;
tmp295=cos(tmp0);
tmp296=sin(tmp2);
tmp297=cos(tmp4);
tmp298=sin(tmp6);
tmp299=(-0.017453293f)*tmp297*tmp298;
tmp300=cos(tmp6);
tmp301=sin(tmp4);
tmp302=(-0.017453293f)*tmp300*tmp301;
tmp303=tmp299+tmp302;
tmp304=(-1.0f)*tmp295*tmp296*tmp303;
tmp305=cos(tmp2);
tmp306=sin(tmp0);
tmp307=cos(tmp4);
tmp308=sin(tmp6);
tmp309=(-0.017453293f)*tmp307*tmp308;
tmp310=cos(tmp6);
tmp311=sin(tmp4);
tmp312=(-0.017453293f)*tmp310*tmp311;
tmp313=tmp309+tmp312;
tmp314=(-1.0f)*tmp305*tmp306*tmp313;
tmp315=tmp304+tmp314;
tmp316=tmp315*dzknee2tip;
tmp317=cos(tmp4);
tmp318=sin(tmp6);
tmp319=(-0.017453293f)*tmp317*tmp318;
tmp320=cos(tmp6);
tmp321=sin(tmp4);
tmp322=(-0.017453293f)*tmp320*tmp321;
tmp323=tmp319+tmp322;
tmp324=tmp323*dhip2femur;
tmp325=tmp272+tmp294+tmp316+tmp324;
dresidual1_d_hip=tmp325;
tmp326=cos(tmp0);
tmp327=sin(tmp2);
tmp328=(-0.017453293f)*tmp326*tmp327;
tmp329=cos(tmp2);
tmp330=sin(tmp0);
tmp331=(-0.017453293f)*tmp329*tmp330;
tmp332=tmp328+tmp331;
tmp333=tmp332*dzknee2tip;
tmp334=sin(tmp0);
tmp335=sin(tmp2);
tmp336=(-0.017453293f)*tmp334*tmp335;
tmp337=cos(tmp0);
tmp338=cos(tmp2);
tmp339=0.017453293f*tmp337*tmp338;
tmp340=tmp336+tmp339;
tmp341=tmp340*dyknee2tip;
tmp342=tmp333+tmp341;
dresidual2_d_knee=tmp342;
tmp343=cos(tmp0);
tmp344=sin(tmp2);
tmp345=(-0.017453293f)*tmp343*tmp344;
tmp346=cos(tmp2);
tmp347=sin(tmp0);
tmp348=(-0.017453293f)*tmp346*tmp347;
tmp349=tmp345+tmp348;
tmp350=tmp349*dzknee2tip;
tmp351=sin(tmp0);
tmp352=sin(tmp2);
tmp353=(-0.017453293f)*tmp351*tmp352;
tmp354=cos(tmp0);
tmp355=cos(tmp2);
tmp356=0.017453293f*tmp354*tmp355;
tmp357=tmp353+tmp356;
tmp358=tmp357*dyknee2tip;
tmp359=cos(tmp0);
tmp360=0.017453293f*tmp359*dfemur2knee;
tmp361=tmp350+tmp358+tmp360;
dresidual2_d_femur=tmp361;
dresidual2_d_hip=0.0f;
} // ik_legdf()
