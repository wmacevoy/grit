#include "fk_lidar.h"
#include <math.h>
Mat3d fk_lidar(float waist, float necklr, float neckud)
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
tmp0=0.017453293f*necklr;
tmp1=cos(tmp0);
ans(0,0)=tmp1;
tmp2=(-0.017453293f)*neckud;
tmp3=cos(tmp2);
tmp4=sin(tmp0);
tmp5=(-1.0f)*tmp3*tmp4;
ans(0,1)=tmp5;
tmp6=sin(tmp2);
tmp7=sin(tmp0);
tmp8=tmp6*tmp7;
ans(0,2)=tmp8;
tmp9=sin(tmp2);
tmp10=sin(tmp0);
tmp11=1.375f*tmp9*tmp10;
tmp12=cos(tmp2);
tmp13=sin(tmp0);
tmp14=1.5f*tmp12*tmp13;
tmp15=sin(tmp2);
tmp16=sin(tmp0);
tmp17=1.75f*tmp15*tmp16;
tmp18=tmp11+tmp14+tmp17;
ans(0,3)=tmp18;
tmp19=(-0.017453293f)*waist;
tmp20=cos(tmp19);
tmp21=sin(tmp0);
tmp22=tmp20*tmp21;
ans(1,0)=tmp22;
tmp23=cos(tmp2);
tmp24=cos(tmp19);
tmp25=tmp23*tmp24*ans(0,0);
tmp26=sin(tmp2);
tmp27=sin(tmp19);
tmp28=(-1.0f)*tmp26*tmp27;
tmp29=tmp25+tmp28;
ans(1,1)=tmp29;
tmp30=cos(tmp2);
tmp31=sin(tmp19);
tmp32=(-1.0f)*tmp30*tmp31;
tmp33=cos(tmp19);
tmp34=sin(tmp2);
tmp35=(-1.0f)*tmp33*ans(0,0)*tmp34;
tmp36=tmp32+tmp35;
ans(1,2)=tmp36;
tmp37=sin(tmp19);
tmp38=(-13.5f)*tmp37;
tmp39=sin(tmp19);
tmp40=(-2.5f)*tmp39;
tmp41=cos(tmp2);
tmp42=sin(tmp19);
tmp43=(-1.75f)*tmp41*tmp42;
tmp44=cos(tmp19);
tmp45=sin(tmp2);
tmp46=(-1.75f)*tmp44*ans(0,0)*tmp45;
tmp47=sin(tmp19);
tmp48=(-1.75f)*tmp47;
tmp49=cos(tmp2);
tmp50=cos(tmp19);
tmp51=(-1.5f)*tmp49*tmp50*ans(0,0);
tmp52=cos(tmp2);
tmp53=sin(tmp19);
tmp54=(-1.375f)*tmp52*tmp53;
tmp55=cos(tmp19);
tmp56=sin(tmp2);
tmp57=(-1.375f)*tmp55*ans(0,0)*tmp56;
tmp58=sin(tmp2);
tmp59=sin(tmp19);
tmp60=1.5f*tmp58*tmp59;
tmp61=tmp38+tmp40+tmp43+tmp46+tmp48+tmp51+tmp54+tmp57+tmp60;
ans(1,3)=tmp61;
tmp62=sin(tmp19);
tmp63=sin(tmp0);
tmp64=tmp62*tmp63;
ans(2,0)=tmp64;
tmp65=cos(tmp2);
tmp66=sin(tmp19);
tmp67=tmp65*ans(0,0)*tmp66;
tmp68=cos(tmp19);
tmp69=sin(tmp2);
tmp70=tmp68*tmp69;
tmp71=tmp67+tmp70;
ans(2,1)=tmp71;
tmp72=cos(tmp2);
tmp73=cos(tmp19);
tmp74=tmp72*tmp73;
tmp75=sin(tmp2);
tmp76=sin(tmp19);
tmp77=(-1.0f)*ans(0,0)*tmp75*tmp76;
tmp78=tmp74+tmp77;
ans(2,2)=tmp78;
tmp79=sin(tmp2);
tmp80=sin(tmp19);
tmp81=(-1.75f)*ans(0,0)*tmp79*tmp80;
tmp82=cos(tmp2);
tmp83=sin(tmp19);
tmp84=(-1.5f)*tmp82*ans(0,0)*tmp83;
tmp85=cos(tmp19);
tmp86=sin(tmp2);
tmp87=(-1.5f)*tmp85*tmp86;
tmp88=sin(tmp2);
tmp89=sin(tmp19);
tmp90=(-1.375f)*ans(0,0)*tmp88*tmp89;
tmp91=cos(tmp2);
tmp92=cos(tmp19);
tmp93=1.375f*tmp91*tmp92;
tmp94=cos(tmp2);
tmp95=cos(tmp19);
tmp96=1.75f*tmp94*tmp95;
tmp97=cos(tmp19);
tmp98=1.75f*tmp97;
tmp99=cos(tmp19);
tmp100=2.5f*tmp99;
tmp101=cos(tmp19);
tmp102=13.5f*tmp101;
tmp103=tmp81+tmp84+tmp87+tmp90+tmp93+tmp96+tmp98+tmp100+tmp102+12.0f;
ans(2,3)=tmp103;
  return ans;
}