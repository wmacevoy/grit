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

const char *ik_legvariable_names[]={
  "aknee_",
  "afemur_",
  "ahip_"
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
#define aknee ik_legparameters[0]
#define afemur ik_legparameters[1]
#define ahip ik_legparameters[2]
#define achasis ik_legparameters[3]
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
#define aknee_ ik_legx[0]
#define afemur_ ik_legx[1]
#define ahip_ ik_legx[2]

// residual aliases
#define residual0 ik_legy[0]
#define residual1 ik_legy[1]
#define residual2 ik_legy[2]

// jacobian aliases (fortran order)
#define dresidual0_daknee_ ik_legdy[0]
#define dresidual0_dafemur_ ik_legdy[3]
#define dresidual0_dahip_ ik_legdy[6]
#define dresidual1_daknee_ ik_legdy[1]
#define dresidual1_dafemur_ ik_legdy[4]
#define dresidual1_dahip_ ik_legdy[7]
#define dresidual2_daknee_ ik_legdy[2]
#define dresidual2_dafemur_ ik_legdy[5]
#define dresidual2_dahip_ ik_legdy[8]

void ik_leginitialize(
  const float ik_legglobals[2],
  const float ik_legparameters[14],
  float ik_legx[3]
)
{
  // initialize unknowns from parameters
  // aknee_=aknee;
  aknee_=aknee;
  // afemur_=afemur;
  afemur_=afemur;
  // ahip_=ahip;
  ahip_=ahip;
} // ik_leginitialize()
void ik_legupdate(
  const float ik_legglobals[2],
  float ik_legparameters[14]
)
{
  float ik_legx[3];
  ik_legsolve(ik_legglobals,ik_legparameters,ik_legx);

  // aknee=aknee_
  aknee=aknee_;
  // afemur=afemur_
  afemur=afemur_;
  // ahip=ahip_
  ahip=ahip_;
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
tmp0=cos(afemur_);
tmp1=cos(achasis);
tmp2=sin(ahip_);
tmp3=(-1.0f)*tmp1*tmp2;
tmp4=cos(ahip_);
tmp5=sin(achasis);
tmp6=(-1.0f)*tmp4*tmp5;
tmp7=tmp3+tmp6;
tmp8=tmp0*tmp7*dfemur2knee;
tmp9=cos(aknee_);
tmp10=(-1.0f)*tmp1*tmp2;
tmp11=(-1.0f)*tmp4*tmp5;
tmp12=tmp10+tmp11;
tmp13=tmp0*tmp9*tmp12;
tmp14=sin(afemur_);
tmp15=sin(aknee_);
tmp16=(-1.0f)*tmp1*tmp2;
tmp17=(-1.0f)*tmp4*tmp5;
tmp18=tmp16+tmp17;
tmp19=(-1.0f)*tmp14*tmp15*tmp18;
tmp20=tmp13+tmp19;
tmp21=tmp20*dyknee2tip;
tmp22=(-1.0f)*tmp1*tmp2;
tmp23=(-1.0f)*tmp4*tmp5;
tmp24=tmp22+tmp23;
tmp25=tmp24*dhip2femur;
tmp26=(-1.0f)*tmp1*tmp2;
tmp27=(-1.0f)*tmp4*tmp5;
tmp28=tmp26+tmp27;
tmp29=(-1.0f)*tmp0*tmp15*tmp28;
tmp30=(-1.0f)*tmp1*tmp2;
tmp31=(-1.0f)*tmp4*tmp5;
tmp32=tmp30+tmp31;
tmp33=(-1.0f)*tmp9*tmp14*tmp32;
tmp34=tmp29+tmp33;
tmp35=tmp34*dzknee2tip;
tmp36=(-1.0f)*tmp5*drchasis2hip;
tmp37=(-1.0f)*px;
tmp38=tmp8+tmp21+tmp25+tmp35+tmp36+tmp37;
residual0=tmp38;
tmp39=tmp1*drchasis2hip;
tmp40=tmp1*tmp4;
tmp41=(-1.0f)*tmp5*tmp2;
tmp42=tmp40+tmp41;
tmp43=tmp0*tmp42*dfemur2knee;
tmp44=tmp1*tmp4;
tmp45=(-1.0f)*tmp5*tmp2;
tmp46=tmp44+tmp45;
tmp47=tmp46*dhip2femur;
tmp48=tmp1*tmp4;
tmp49=(-1.0f)*tmp5*tmp2;
tmp50=tmp48+tmp49;
tmp51=tmp0*tmp9*tmp50;
tmp52=tmp1*tmp4;
tmp53=(-1.0f)*tmp5*tmp2;
tmp54=tmp52+tmp53;
tmp55=(-1.0f)*tmp14*tmp15*tmp54;
tmp56=tmp51+tmp55;
tmp57=tmp56*dyknee2tip;
tmp58=tmp1*tmp4;
tmp59=(-1.0f)*tmp5*tmp2;
tmp60=tmp58+tmp59;
tmp61=(-1.0f)*tmp0*tmp15*tmp60;
tmp62=tmp1*tmp4;
tmp63=(-1.0f)*tmp5*tmp2;
tmp64=tmp62+tmp63;
tmp65=(-1.0f)*tmp9*tmp14*tmp64;
tmp66=tmp61+tmp65;
tmp67=tmp66*dzknee2tip;
tmp68=(-1.0f)*py;
tmp69=tmp39+tmp43+tmp47+tmp57+tmp67+tmp68;
residual1=tmp69;
tmp70=tmp14*dfemur2knee;
tmp71=tmp0*tmp9;
tmp72=(-1.0f)*tmp14*tmp15;
tmp73=tmp71+tmp72;
tmp74=tmp73*dzknee2tip;
tmp75=tmp0*tmp15;
tmp76=tmp9*tmp14;
tmp77=tmp75+tmp76;
tmp78=tmp77*dyknee2tip;
tmp79=(-1.0f)*pz;
tmp80=tmp70+tmp74+tmp78+tmp79+dzchasis2hip;
residual2=tmp80;
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
tmp0=sin(afemur_);
tmp1=sin(aknee_);
tmp2=cos(achasis);
tmp3=sin(ahip_);
tmp4=(-1.0f)*tmp2*tmp3;
tmp5=cos(ahip_);
tmp6=sin(achasis);
tmp7=(-1.0f)*tmp5*tmp6;
tmp8=tmp4+tmp7;
tmp9=tmp0*tmp1*tmp8;
tmp10=cos(afemur_);
tmp11=cos(aknee_);
tmp12=(-1.0f)*tmp2*tmp3;
tmp13=(-1.0f)*tmp5*tmp6;
tmp14=tmp12+tmp13;
tmp15=(-1.0f)*tmp10*tmp11*tmp14;
tmp16=tmp9+tmp15;
tmp17=tmp16*dzknee2tip;
tmp18=(-1.0f)*tmp2*tmp3;
tmp19=(-1.0f)*tmp5*tmp6;
tmp20=tmp18+tmp19;
tmp21=(-1.0f)*tmp10*tmp1*tmp20;
tmp22=(-1.0f)*tmp2*tmp3;
tmp23=(-1.0f)*tmp5*tmp6;
tmp24=tmp22+tmp23;
tmp25=(-1.0f)*tmp11*tmp0*tmp24;
tmp26=tmp21+tmp25;
tmp27=tmp26*dyknee2tip;
tmp28=tmp17+tmp27;
dresidual0_daknee_=tmp28;
tmp29=(-1.0f)*tmp2*tmp3;
tmp30=(-1.0f)*tmp5*tmp6;
tmp31=tmp29+tmp30;
tmp32=tmp0*tmp1*tmp31;
tmp33=(-1.0f)*tmp2*tmp3;
tmp34=(-1.0f)*tmp5*tmp6;
tmp35=tmp33+tmp34;
tmp36=(-1.0f)*tmp10*tmp11*tmp35;
tmp37=tmp32+tmp36;
tmp38=tmp37*dzknee2tip;
tmp39=(-1.0f)*tmp2*tmp3;
tmp40=(-1.0f)*tmp5*tmp6;
tmp41=tmp39+tmp40;
tmp42=(-1.0f)*tmp10*tmp1*tmp41;
tmp43=(-1.0f)*tmp2*tmp3;
tmp44=(-1.0f)*tmp5*tmp6;
tmp45=tmp43+tmp44;
tmp46=(-1.0f)*tmp11*tmp0*tmp45;
tmp47=tmp42+tmp46;
tmp48=tmp47*dyknee2tip;
tmp49=(-1.0f)*tmp2*tmp3;
tmp50=(-1.0f)*tmp5*tmp6;
tmp51=tmp49+tmp50;
tmp52=(-1.0f)*tmp0*tmp51*dfemur2knee;
tmp53=tmp38+tmp48+tmp52;
dresidual0_dafemur_=tmp53;
tmp54=tmp6*tmp3;
tmp55=(-1.0f)*tmp2*tmp5;
tmp56=tmp54+tmp55;
tmp57=tmp10*tmp56*dfemur2knee;
tmp58=tmp6*tmp3;
tmp59=(-1.0f)*tmp2*tmp5;
tmp60=tmp58+tmp59;
tmp61=tmp10*tmp11*tmp60;
tmp62=tmp6*tmp3;
tmp63=(-1.0f)*tmp2*tmp5;
tmp64=tmp62+tmp63;
tmp65=(-1.0f)*tmp0*tmp1*tmp64;
tmp66=tmp61+tmp65;
tmp67=tmp66*dyknee2tip;
tmp68=tmp6*tmp3;
tmp69=(-1.0f)*tmp2*tmp5;
tmp70=tmp68+tmp69;
tmp71=tmp70*dhip2femur;
tmp72=tmp6*tmp3;
tmp73=(-1.0f)*tmp2*tmp5;
tmp74=tmp72+tmp73;
tmp75=(-1.0f)*tmp10*tmp1*tmp74;
tmp76=tmp6*tmp3;
tmp77=(-1.0f)*tmp2*tmp5;
tmp78=tmp76+tmp77;
tmp79=(-1.0f)*tmp11*tmp0*tmp78;
tmp80=tmp75+tmp79;
tmp81=tmp80*dzknee2tip;
tmp82=tmp57+tmp67+tmp71+tmp81;
dresidual0_dahip_=tmp82;
tmp83=tmp2*tmp5;
tmp84=(-1.0f)*tmp6*tmp3;
tmp85=tmp83+tmp84;
tmp86=tmp0*tmp1*tmp85;
tmp87=tmp2*tmp5;
tmp88=(-1.0f)*tmp6*tmp3;
tmp89=tmp87+tmp88;
tmp90=(-1.0f)*tmp10*tmp11*tmp89;
tmp91=tmp86+tmp90;
tmp92=tmp91*dzknee2tip;
tmp93=tmp2*tmp5;
tmp94=(-1.0f)*tmp6*tmp3;
tmp95=tmp93+tmp94;
tmp96=(-1.0f)*tmp10*tmp1*tmp95;
tmp97=tmp2*tmp5;
tmp98=(-1.0f)*tmp6*tmp3;
tmp99=tmp97+tmp98;
tmp100=(-1.0f)*tmp11*tmp0*tmp99;
tmp101=tmp96+tmp100;
tmp102=tmp101*dyknee2tip;
tmp103=tmp92+tmp102;
dresidual1_daknee_=tmp103;
tmp104=tmp2*tmp5;
tmp105=(-1.0f)*tmp6*tmp3;
tmp106=tmp104+tmp105;
tmp107=tmp0*tmp1*tmp106;
tmp108=tmp2*tmp5;
tmp109=(-1.0f)*tmp6*tmp3;
tmp110=tmp108+tmp109;
tmp111=(-1.0f)*tmp10*tmp11*tmp110;
tmp112=tmp107+tmp111;
tmp113=tmp112*dzknee2tip;
tmp114=tmp2*tmp5;
tmp115=(-1.0f)*tmp6*tmp3;
tmp116=tmp114+tmp115;
tmp117=(-1.0f)*tmp10*tmp1*tmp116;
tmp118=tmp2*tmp5;
tmp119=(-1.0f)*tmp6*tmp3;
tmp120=tmp118+tmp119;
tmp121=(-1.0f)*tmp11*tmp0*tmp120;
tmp122=tmp117+tmp121;
tmp123=tmp122*dyknee2tip;
tmp124=tmp2*tmp5;
tmp125=(-1.0f)*tmp6*tmp3;
tmp126=tmp124+tmp125;
tmp127=(-1.0f)*tmp0*tmp126*dfemur2knee;
tmp128=tmp113+tmp123+tmp127;
dresidual1_dafemur_=tmp128;
tmp129=(-1.0f)*tmp2*tmp3;
tmp130=(-1.0f)*tmp5*tmp6;
tmp131=tmp129+tmp130;
tmp132=tmp10*tmp131*dfemur2knee;
tmp133=(-1.0f)*tmp2*tmp3;
tmp134=(-1.0f)*tmp5*tmp6;
tmp135=tmp133+tmp134;
tmp136=tmp10*tmp11*tmp135;
tmp137=(-1.0f)*tmp2*tmp3;
tmp138=(-1.0f)*tmp5*tmp6;
tmp139=tmp137+tmp138;
tmp140=(-1.0f)*tmp0*tmp1*tmp139;
tmp141=tmp136+tmp140;
tmp142=tmp141*dyknee2tip;
tmp143=(-1.0f)*tmp2*tmp3;
tmp144=(-1.0f)*tmp5*tmp6;
tmp145=tmp143+tmp144;
tmp146=tmp145*dhip2femur;
tmp147=(-1.0f)*tmp2*tmp3;
tmp148=(-1.0f)*tmp5*tmp6;
tmp149=tmp147+tmp148;
tmp150=(-1.0f)*tmp10*tmp1*tmp149;
tmp151=(-1.0f)*tmp2*tmp3;
tmp152=(-1.0f)*tmp5*tmp6;
tmp153=tmp151+tmp152;
tmp154=(-1.0f)*tmp11*tmp0*tmp153;
tmp155=tmp150+tmp154;
tmp156=tmp155*dzknee2tip;
tmp157=tmp132+tmp142+tmp146+tmp156;
dresidual1_dahip_=tmp157;
tmp158=tmp10*tmp11;
tmp159=(-1.0f)*tmp0*tmp1;
tmp160=tmp158+tmp159;
tmp161=tmp160*dyknee2tip;
tmp162=(-1.0f)*tmp10*tmp1;
tmp163=(-1.0f)*tmp11*tmp0;
tmp164=tmp162+tmp163;
tmp165=tmp164*dzknee2tip;
tmp166=tmp161+tmp165;
dresidual2_daknee_=tmp166;
tmp167=tmp10*dfemur2knee;
tmp168=tmp10*tmp11;
tmp169=(-1.0f)*tmp0*tmp1;
tmp170=tmp168+tmp169;
tmp171=tmp170*dyknee2tip;
tmp172=(-1.0f)*tmp10*tmp1;
tmp173=(-1.0f)*tmp11*tmp0;
tmp174=tmp172+tmp173;
tmp175=tmp174*dzknee2tip;
tmp176=tmp167+tmp171+tmp175;
dresidual2_dafemur_=tmp176;
dresidual2_dahip_=0.0f;
} // ik_legdf()
