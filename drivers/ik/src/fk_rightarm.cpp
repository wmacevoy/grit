#include <math.h>
#include "fk_rightarm.h"

void fk_rightarm(
float RIGHT_SHOULDER_IO,
float RIGHT_SHOULDER_UD,
float RIGHT_BICEP_ROTATE,
float RIGHT_ELBOW,
float RIGHT_FOREARM_ROTATE,
float pose[4][4]
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
tmp0=0.017453293f*RIGHT_FOREARM_ROTATE;
tmp1=cos(tmp0);
tmp2=0.017453293f*RIGHT_BICEP_ROTATE;
tmp3=cos(tmp2);
tmp4=0.017453293f*RIGHT_SHOULDER_IO;
tmp5=cos(tmp4);
tmp6=tmp3*tmp5;
tmp7=sin(tmp2);
tmp8=sin(tmp4);
tmp9=0.017453293f*RIGHT_SHOULDER_UD;
tmp10=sin(tmp9);
tmp11=tmp7*tmp8*tmp10;
tmp12=tmp6+tmp11;
tmp13=tmp1*tmp12;
tmp14=sin(tmp0);
tmp15=0.017453293f*RIGHT_ELBOW;
tmp16=cos(tmp15);
tmp17=cos(tmp4);
tmp18=sin(tmp2);
tmp19=tmp17*tmp18;
tmp20=cos(tmp2);
tmp21=sin(tmp4);
tmp22=sin(tmp9);
tmp23=(-1.0f)*tmp20*tmp21*tmp22;
tmp24=tmp19+tmp23;
tmp25=tmp16*tmp24;
tmp26=cos(tmp9);
tmp27=sin(tmp15);
tmp28=sin(tmp4);
tmp29=(-1.0f)*tmp26*tmp27*tmp28;
tmp30=tmp25+tmp29;
tmp31=(-1.0f)*tmp14*tmp30;
tmp32=tmp13+tmp31;
pose[0][0]=tmp32;
tmp33=cos(tmp15);
tmp34=cos(tmp9);
tmp35=sin(tmp4);
tmp36=tmp33*tmp34*tmp35;
tmp37=sin(tmp15);
tmp38=cos(tmp4);
tmp39=sin(tmp2);
tmp40=tmp38*tmp39;
tmp41=cos(tmp2);
tmp42=sin(tmp4);
tmp43=sin(tmp9);
tmp44=(-1.0f)*tmp41*tmp42*tmp43;
tmp45=tmp40+tmp44;
tmp46=tmp37*tmp45;
tmp47=tmp36+tmp46;
pose[0][1]=tmp47;
tmp48=cos(tmp0);
tmp49=cos(tmp15);
tmp50=cos(tmp4);
tmp51=sin(tmp2);
tmp52=tmp50*tmp51;
tmp53=cos(tmp2);
tmp54=sin(tmp4);
tmp55=sin(tmp9);
tmp56=(-1.0f)*tmp53*tmp54*tmp55;
tmp57=tmp52+tmp56;
tmp58=tmp49*tmp57;
tmp59=cos(tmp9);
tmp60=sin(tmp15);
tmp61=sin(tmp4);
tmp62=(-1.0f)*tmp59*tmp60*tmp61;
tmp63=tmp58+tmp62;
tmp64=tmp48*tmp63;
tmp65=sin(tmp0);
tmp66=cos(tmp2);
tmp67=cos(tmp4);
tmp68=tmp66*tmp67;
tmp69=sin(tmp2);
tmp70=sin(tmp4);
tmp71=sin(tmp9);
tmp72=tmp69*tmp70*tmp71;
tmp73=tmp68+tmp72;
tmp74=tmp65*tmp73;
tmp75=tmp64+tmp74;
pose[0][2]=tmp75;
tmp76=sin(tmp4);
tmp77=3.625f*tmp76;
tmp78=cos(tmp9);
tmp79=sin(tmp4);
tmp80=9.0f*tmp78*tmp79;
tmp81=cos(tmp15);
tmp82=cos(tmp9);
tmp83=sin(tmp4);
tmp84=15.0f*tmp81*tmp82*tmp83;
tmp85=sin(tmp15);
tmp86=cos(tmp4);
tmp87=sin(tmp2);
tmp88=tmp86*tmp87;
tmp89=cos(tmp2);
tmp90=sin(tmp4);
tmp91=sin(tmp9);
tmp92=(-1.0f)*tmp89*tmp90*tmp91;
tmp93=tmp88+tmp92;
tmp94=15.0f*tmp85*tmp93;
tmp95=tmp77+tmp80+tmp84+tmp94+6.0f;
pose[0][3]=tmp95;
tmp96=cos(tmp0);
tmp97=cos(tmp4);
tmp98=sin(tmp2);
tmp99=sin(tmp9);
tmp100=tmp97*tmp98*tmp99;
tmp101=cos(tmp2);
tmp102=sin(tmp4);
tmp103=(-1.0f)*tmp101*tmp102;
tmp104=tmp100+tmp103;
tmp105=tmp96*tmp104;
tmp106=sin(tmp0);
tmp107=cos(tmp15);
tmp108=cos(tmp2);
tmp109=cos(tmp4);
tmp110=sin(tmp9);
tmp111=(-1.0f)*tmp108*tmp109*tmp110;
tmp112=sin(tmp2);
tmp113=sin(tmp4);
tmp114=(-1.0f)*tmp112*tmp113;
tmp115=tmp111+tmp114;
tmp116=tmp107*tmp115;
tmp117=cos(tmp4);
tmp118=cos(tmp9);
tmp119=sin(tmp15);
tmp120=(-1.0f)*tmp117*tmp118*tmp119;
tmp121=tmp116+tmp120;
tmp122=(-1.0f)*tmp106*tmp121;
tmp123=tmp105+tmp122;
pose[1][0]=tmp123;
tmp124=cos(tmp15);
tmp125=cos(tmp4);
tmp126=cos(tmp9);
tmp127=tmp124*tmp125*tmp126;
tmp128=sin(tmp15);
tmp129=cos(tmp2);
tmp130=cos(tmp4);
tmp131=sin(tmp9);
tmp132=(-1.0f)*tmp129*tmp130*tmp131;
tmp133=sin(tmp2);
tmp134=sin(tmp4);
tmp135=(-1.0f)*tmp133*tmp134;
tmp136=tmp132+tmp135;
tmp137=tmp128*tmp136;
tmp138=tmp127+tmp137;
pose[1][1]=tmp138;
tmp139=cos(tmp0);
tmp140=cos(tmp15);
tmp141=cos(tmp2);
tmp142=cos(tmp4);
tmp143=sin(tmp9);
tmp144=(-1.0f)*tmp141*tmp142*tmp143;
tmp145=sin(tmp2);
tmp146=sin(tmp4);
tmp147=(-1.0f)*tmp145*tmp146;
tmp148=tmp144+tmp147;
tmp149=tmp140*tmp148;
tmp150=cos(tmp4);
tmp151=cos(tmp9);
tmp152=sin(tmp15);
tmp153=(-1.0f)*tmp150*tmp151*tmp152;
tmp154=tmp149+tmp153;
tmp155=tmp139*tmp154;
tmp156=sin(tmp0);
tmp157=cos(tmp4);
tmp158=sin(tmp2);
tmp159=sin(tmp9);
tmp160=tmp157*tmp158*tmp159;
tmp161=cos(tmp2);
tmp162=sin(tmp4);
tmp163=(-1.0f)*tmp161*tmp162;
tmp164=tmp160+tmp163;
tmp165=tmp156*tmp164;
tmp166=tmp155+tmp165;
pose[1][2]=tmp166;
tmp167=cos(tmp4);
tmp168=3.625f*tmp167;
tmp169=cos(tmp4);
tmp170=cos(tmp9);
tmp171=9.0f*tmp169*tmp170;
tmp172=cos(tmp15);
tmp173=cos(tmp4);
tmp174=cos(tmp9);
tmp175=15.0f*tmp172*tmp173*tmp174;
tmp176=sin(tmp15);
tmp177=cos(tmp2);
tmp178=cos(tmp4);
tmp179=sin(tmp9);
tmp180=(-1.0f)*tmp177*tmp178*tmp179;
tmp181=sin(tmp2);
tmp182=sin(tmp4);
tmp183=(-1.0f)*tmp181*tmp182;
tmp184=tmp180+tmp183;
tmp185=15.0f*tmp176*tmp184;
tmp186=tmp168+tmp171+tmp175+tmp185+2.375f;
pose[1][3]=tmp186;
tmp187=cos(tmp0);
tmp188=cos(tmp9);
tmp189=sin(tmp2);
tmp190=(-1.0f)*tmp187*tmp188*tmp189;
tmp191=sin(tmp0);
tmp192=cos(tmp2);
tmp193=cos(tmp15);
tmp194=cos(tmp9);
tmp195=tmp192*tmp193*tmp194;
tmp196=sin(tmp15);
tmp197=sin(tmp9);
tmp198=(-1.0f)*tmp196*tmp197;
tmp199=tmp195+tmp198;
tmp200=(-1.0f)*tmp191*tmp199;
tmp201=tmp190+tmp200;
pose[2][0]=tmp201;
tmp202=cos(tmp2);
tmp203=cos(tmp9);
tmp204=sin(tmp15);
tmp205=tmp202*tmp203*tmp204;
tmp206=cos(tmp15);
tmp207=sin(tmp9);
tmp208=tmp206*tmp207;
tmp209=tmp205+tmp208;
pose[2][1]=tmp209;
tmp210=cos(tmp0);
tmp211=cos(tmp2);
tmp212=cos(tmp15);
tmp213=cos(tmp9);
tmp214=tmp211*tmp212*tmp213;
tmp215=sin(tmp15);
tmp216=sin(tmp9);
tmp217=(-1.0f)*tmp215*tmp216;
tmp218=tmp214+tmp217;
tmp219=tmp210*tmp218;
tmp220=cos(tmp9);
tmp221=sin(tmp2);
tmp222=sin(tmp0);
tmp223=(-1.0f)*tmp220*tmp221*tmp222;
tmp224=tmp219+tmp223;
pose[2][2]=tmp224;
tmp225=sin(tmp9);
tmp226=9.0f*tmp225;
tmp227=cos(tmp2);
tmp228=cos(tmp9);
tmp229=sin(tmp15);
tmp230=15.0f*tmp227*tmp228*tmp229;
tmp231=cos(tmp15);
tmp232=sin(tmp9);
tmp233=15.0f*tmp231*tmp232;
tmp234=tmp226+tmp230+tmp233;
pose[2][3]=tmp234;
pose[3][0]=0.0f;
pose[3][1]=0.0f;
pose[3][2]=0.0f;
pose[3][3]=1.0f;
}