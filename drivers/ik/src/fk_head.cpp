#include "fk_head.h"
#include <math.h>
Mat3d fk_head(float waist, float necklr, float neckud)
{
  Mat3d ans;
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
tmp0=0.017453293f*necklr;
tmp1=cos(tmp0);
ans(0,0)=tmp1;
tmp2=0.017453293f*neckud;
tmp3=cos(tmp2);
tmp4=sin(tmp0);
tmp5=(-1.0f)*tmp3*tmp4;
ans(0,1)=tmp5;
tmp6=sin(tmp0);
tmp7=sin(tmp2);
tmp8=tmp6*tmp7;
ans(0,2)=tmp8;
tmp9=sin(tmp0);
tmp10=sin(tmp2);
tmp11=1.375f*tmp9*tmp10;
ans(0,3)=tmp11;
tmp12=(-0.017453293f)*waist;
tmp13=cos(tmp12);
tmp14=sin(tmp0);
tmp15=tmp13*tmp14;
ans(1,0)=tmp15;
tmp16=cos(tmp12);
tmp17=cos(tmp2);
tmp18=tmp16*ans(0,0)*tmp17;
tmp19=sin(tmp12);
tmp20=sin(tmp2);
tmp21=(-1.0f)*tmp19*tmp20;
tmp22=tmp18+tmp21;
ans(1,1)=tmp22;
tmp23=cos(tmp12);
tmp24=sin(tmp2);
tmp25=(-1.0f)*tmp23*ans(0,0)*tmp24;
tmp26=cos(tmp2);
tmp27=sin(tmp12);
tmp28=(-1.0f)*tmp26*tmp27;
tmp29=tmp25+tmp28;
ans(1,2)=tmp29;
tmp30=sin(tmp12);
tmp31=(-13.5f)*tmp30;
tmp32=sin(tmp12);
tmp33=(-2.5f)*tmp32;
tmp34=sin(tmp12);
tmp35=(-1.75f)*tmp34;
tmp36=cos(tmp12);
tmp37=sin(tmp2);
tmp38=(-1.375f)*tmp36*ans(0,0)*tmp37;
tmp39=cos(tmp2);
tmp40=sin(tmp12);
tmp41=(-1.375f)*tmp39*tmp40;
tmp42=tmp31+tmp33+tmp35+tmp38+tmp41;
ans(1,3)=tmp42;
tmp43=sin(tmp12);
tmp44=sin(tmp0);
tmp45=tmp43*tmp44;
ans(2,0)=tmp45;
tmp46=cos(tmp12);
tmp47=sin(tmp2);
tmp48=tmp46*tmp47;
tmp49=cos(tmp2);
tmp50=sin(tmp12);
tmp51=ans(0,0)*tmp49*tmp50;
tmp52=tmp48+tmp51;
ans(2,1)=tmp52;
tmp53=cos(tmp12);
tmp54=cos(tmp2);
tmp55=tmp53*tmp54;
tmp56=sin(tmp12);
tmp57=sin(tmp2);
tmp58=(-1.0f)*ans(0,0)*tmp56*tmp57;
tmp59=tmp55+tmp58;
ans(2,2)=tmp59;
tmp60=sin(tmp12);
tmp61=sin(tmp2);
tmp62=(-1.375f)*ans(0,0)*tmp60*tmp61;
tmp63=cos(tmp12);
tmp64=cos(tmp2);
tmp65=1.375f*tmp63*tmp64;
tmp66=cos(tmp12);
tmp67=1.75f*tmp66;
tmp68=cos(tmp12);
tmp69=2.5f*tmp68;
tmp70=cos(tmp12);
tmp71=13.5f*tmp70;
tmp72=tmp62+tmp65+tmp67+tmp69+tmp71+12.0f;
ans(2,3)=tmp72;
  return ans;
}