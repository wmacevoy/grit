#include "fk_rightarm.h"
#include <math.h>
Mat3d fk_rightarm(float waist,float shoulderio, float shoulderud, float bicep, float elbow, float forearm)
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
float tmp461;
float tmp462;
float tmp463;
float tmp464;
float tmp465;
float tmp466;
float tmp467;
float tmp468;
float tmp469;
float tmp470;
float tmp471;
float tmp472;
float tmp473;
float tmp474;
float tmp475;
float tmp476;
float tmp477;
float tmp478;
float tmp479;
float tmp480;
float tmp481;
float tmp482;
float tmp483;
float tmp484;
float tmp485;
float tmp486;
float tmp487;
float tmp488;
float tmp489;
float tmp490;
float tmp491;
float tmp492;
float tmp493;
float tmp494;
float tmp495;
float tmp496;
float tmp497;
float tmp498;
float tmp499;
float tmp500;
float tmp501;
float tmp502;
float tmp503;
float tmp504;
float tmp505;
float tmp506;
float tmp507;
float tmp508;
float tmp509;
float tmp510;
float tmp511;
float tmp512;
float tmp513;
float tmp514;
float tmp515;
float tmp516;
float tmp517;
float tmp518;
float tmp519;
float tmp520;
float tmp521;
float tmp522;
float tmp523;
float tmp524;
float tmp525;
float tmp526;
float tmp527;
float tmp528;
float tmp529;
float tmp530;
float tmp531;
float tmp532;
float tmp533;
float tmp534;
float tmp535;
float tmp536;
float tmp537;
float tmp538;
float tmp539;
float tmp540;
float tmp541;
float tmp542;
float tmp543;
float tmp544;
float tmp545;
float tmp546;
float tmp547;
float tmp548;
float tmp549;
float tmp550;
float tmp551;
float tmp552;
float tmp553;
float tmp554;
float tmp555;
float tmp556;
float tmp557;
float tmp558;
float tmp559;
float tmp560;
float tmp561;
float tmp562;
float tmp563;
float tmp564;
float tmp565;
float tmp566;
float tmp567;
float tmp568;
float tmp569;
float tmp570;
float tmp571;
float tmp572;
float tmp573;
tmp0=(-0.017453293f)*forearm;
tmp1=tmp0+1.134464f;
tmp2=cos(tmp1);
tmp3=0.017453293f*shoulderio;
tmp4=tmp3+(-0.78539816f);
tmp5=cos(tmp4);
tmp6=(-0.017453293f)*bicep;
tmp7=cos(tmp6);
tmp8=tmp5*tmp7;
tmp9=tmp3+(-0.78539816f);
tmp10=sin(tmp9);
tmp11=0.017453293f*shoulderud;
tmp12=tmp11+(-0.78539816f);
tmp13=sin(tmp12);
tmp14=sin(tmp6);
tmp15=(-1.0f)*tmp10*tmp13*tmp14;
tmp16=tmp8+tmp15;
tmp17=tmp2*tmp16;
tmp18=tmp0+1.134464f;
tmp19=sin(tmp18);
tmp20=(-0.017453293f)*elbow;
tmp21=tmp20+0.78539816f;
tmp22=cos(tmp21);
tmp23=tmp3+(-0.78539816f);
tmp24=cos(tmp23);
tmp25=sin(tmp6);
tmp26=tmp24*tmp25;
tmp27=cos(tmp6);
tmp28=tmp3+(-0.78539816f);
tmp29=sin(tmp28);
tmp30=tmp11+(-0.78539816f);
tmp31=sin(tmp30);
tmp32=tmp27*tmp29*tmp31;
tmp33=tmp26+tmp32;
tmp34=tmp22*tmp33;
tmp35=tmp11+(-0.78539816f);
tmp36=cos(tmp35);
tmp37=tmp20+0.78539816f;
tmp38=sin(tmp37);
tmp39=tmp3+(-0.78539816f);
tmp40=sin(tmp39);
tmp41=tmp36*tmp38*tmp40;
tmp42=tmp34+tmp41;
tmp43=(-1.0f)*tmp19*tmp42;
tmp44=tmp17+tmp43;
ans(0,0)=tmp44;
tmp45=tmp20+0.78539816f;
tmp46=sin(tmp45);
tmp47=tmp3+(-0.78539816f);
tmp48=cos(tmp47);
tmp49=sin(tmp6);
tmp50=tmp48*tmp49;
tmp51=cos(tmp6);
tmp52=tmp3+(-0.78539816f);
tmp53=sin(tmp52);
tmp54=tmp11+(-0.78539816f);
tmp55=sin(tmp54);
tmp56=tmp51*tmp53*tmp55;
tmp57=tmp50+tmp56;
tmp58=tmp46*tmp57;
tmp59=tmp20+0.78539816f;
tmp60=cos(tmp59);
tmp61=tmp11+(-0.78539816f);
tmp62=cos(tmp61);
tmp63=tmp3+(-0.78539816f);
tmp64=sin(tmp63);
tmp65=(-1.0f)*tmp60*tmp62*tmp64;
tmp66=tmp58+tmp65;
ans(0,1)=tmp66;
tmp67=tmp0+1.134464f;
tmp68=cos(tmp67);
tmp69=tmp20+0.78539816f;
tmp70=cos(tmp69);
tmp71=tmp3+(-0.78539816f);
tmp72=cos(tmp71);
tmp73=sin(tmp6);
tmp74=tmp72*tmp73;
tmp75=cos(tmp6);
tmp76=tmp3+(-0.78539816f);
tmp77=sin(tmp76);
tmp78=tmp11+(-0.78539816f);
tmp79=sin(tmp78);
tmp80=tmp75*tmp77*tmp79;
tmp81=tmp74+tmp80;
tmp82=tmp70*tmp81;
tmp83=tmp11+(-0.78539816f);
tmp84=cos(tmp83);
tmp85=tmp20+0.78539816f;
tmp86=sin(tmp85);
tmp87=tmp3+(-0.78539816f);
tmp88=sin(tmp87);
tmp89=tmp84*tmp86*tmp88;
tmp90=tmp82+tmp89;
tmp91=tmp68*tmp90;
tmp92=tmp0+1.134464f;
tmp93=sin(tmp92);
tmp94=tmp3+(-0.78539816f);
tmp95=cos(tmp94);
tmp96=cos(tmp6);
tmp97=tmp95*tmp96;
tmp98=tmp3+(-0.78539816f);
tmp99=sin(tmp98);
tmp100=tmp11+(-0.78539816f);
tmp101=sin(tmp100);
tmp102=sin(tmp6);
tmp103=(-1.0f)*tmp99*tmp101*tmp102;
tmp104=tmp97+tmp103;
tmp105=tmp93*tmp104;
tmp106=tmp91+tmp105;
ans(0,2)=tmp106;
tmp107=tmp20+0.78539816f;
tmp108=cos(tmp107);
tmp109=tmp11+(-0.78539816f);
tmp110=cos(tmp109);
tmp111=tmp3+(-0.78539816f);
tmp112=sin(tmp111);
tmp113=(-14.0f)*tmp108*tmp110*tmp112;
tmp114=tmp11+(-0.78539816f);
tmp115=cos(tmp114);
tmp116=tmp3+(-0.78539816f);
tmp117=sin(tmp116);
tmp118=(-8.5f)*tmp115*tmp117;
tmp119=tmp3+(-0.78539816f);
tmp120=sin(tmp119);
tmp121=(-3.625f)*tmp120;
tmp122=tmp20+0.78539816f;
tmp123=sin(tmp122);
tmp124=tmp3+(-0.78539816f);
tmp125=cos(tmp124);
tmp126=sin(tmp6);
tmp127=tmp125*tmp126;
tmp128=cos(tmp6);
tmp129=tmp3+(-0.78539816f);
tmp130=sin(tmp129);
tmp131=tmp11+(-0.78539816f);
tmp132=sin(tmp131);
tmp133=tmp128*tmp130*tmp132;
tmp134=tmp127+tmp133;
tmp135=14.0f*tmp123*tmp134;
tmp136=tmp113+tmp118+tmp121+tmp135+4.875f;
ans(0,3)=tmp136;
tmp137=tmp0+1.134464f;
tmp138=cos(tmp137);
tmp139=cos(tmp6);
tmp140=(-0.017453293f)*waist;
tmp141=cos(tmp140);
tmp142=tmp3+(-0.78539816f);
tmp143=sin(tmp142);
tmp144=tmp139*tmp141*tmp143;
tmp145=sin(tmp6);
tmp146=tmp3+(-0.78539816f);
tmp147=cos(tmp146);
tmp148=cos(tmp140);
tmp149=tmp11+(-0.78539816f);
tmp150=sin(tmp149);
tmp151=(-1.0f)*tmp147*tmp148*tmp150;
tmp152=tmp11+(-0.78539816f);
tmp153=cos(tmp152);
tmp154=sin(tmp140);
tmp155=(-1.0f)*tmp153*tmp154;
tmp156=tmp151+tmp155;
tmp157=(-1.0f)*tmp145*tmp156;
tmp158=tmp144+tmp157;
tmp159=tmp138*tmp158;
tmp160=tmp0+1.134464f;
tmp161=sin(tmp160);
tmp162=tmp20+0.78539816f;
tmp163=cos(tmp162);
tmp164=cos(tmp6);
tmp165=tmp3+(-0.78539816f);
tmp166=cos(tmp165);
tmp167=cos(tmp140);
tmp168=tmp11+(-0.78539816f);
tmp169=sin(tmp168);
tmp170=(-1.0f)*tmp166*tmp167*tmp169;
tmp171=tmp11+(-0.78539816f);
tmp172=cos(tmp171);
tmp173=sin(tmp140);
tmp174=(-1.0f)*tmp172*tmp173;
tmp175=tmp170+tmp174;
tmp176=tmp164*tmp175;
tmp177=cos(tmp140);
tmp178=tmp3+(-0.78539816f);
tmp179=sin(tmp178);
tmp180=sin(tmp6);
tmp181=tmp177*tmp179*tmp180;
tmp182=tmp176+tmp181;
tmp183=tmp163*tmp182;
tmp184=tmp20+0.78539816f;
tmp185=sin(tmp184);
tmp186=tmp3+(-0.78539816f);
tmp187=cos(tmp186);
tmp188=tmp11+(-0.78539816f);
tmp189=cos(tmp188);
tmp190=cos(tmp140);
tmp191=tmp187*tmp189*tmp190;
tmp192=tmp11+(-0.78539816f);
tmp193=sin(tmp192);
tmp194=sin(tmp140);
tmp195=(-1.0f)*tmp193*tmp194;
tmp196=tmp191+tmp195;
tmp197=(-1.0f)*tmp185*tmp196;
tmp198=tmp183+tmp197;
tmp199=(-1.0f)*tmp161*tmp198;
tmp200=tmp159+tmp199;
ans(1,0)=tmp200;
tmp201=tmp20+0.78539816f;
tmp202=cos(tmp201);
tmp203=tmp3+(-0.78539816f);
tmp204=cos(tmp203);
tmp205=tmp11+(-0.78539816f);
tmp206=cos(tmp205);
tmp207=cos(tmp140);
tmp208=tmp204*tmp206*tmp207;
tmp209=tmp11+(-0.78539816f);
tmp210=sin(tmp209);
tmp211=sin(tmp140);
tmp212=(-1.0f)*tmp210*tmp211;
tmp213=tmp208+tmp212;
tmp214=tmp202*tmp213;
tmp215=tmp20+0.78539816f;
tmp216=sin(tmp215);
tmp217=cos(tmp6);
tmp218=tmp3+(-0.78539816f);
tmp219=cos(tmp218);
tmp220=cos(tmp140);
tmp221=tmp11+(-0.78539816f);
tmp222=sin(tmp221);
tmp223=(-1.0f)*tmp219*tmp220*tmp222;
tmp224=tmp11+(-0.78539816f);
tmp225=cos(tmp224);
tmp226=sin(tmp140);
tmp227=(-1.0f)*tmp225*tmp226;
tmp228=tmp223+tmp227;
tmp229=tmp217*tmp228;
tmp230=cos(tmp140);
tmp231=tmp3+(-0.78539816f);
tmp232=sin(tmp231);
tmp233=sin(tmp6);
tmp234=tmp230*tmp232*tmp233;
tmp235=tmp229+tmp234;
tmp236=tmp216*tmp235;
tmp237=tmp214+tmp236;
ans(1,1)=tmp237;
tmp238=tmp0+1.134464f;
tmp239=cos(tmp238);
tmp240=tmp20+0.78539816f;
tmp241=cos(tmp240);
tmp242=cos(tmp6);
tmp243=tmp3+(-0.78539816f);
tmp244=cos(tmp243);
tmp245=cos(tmp140);
tmp246=tmp11+(-0.78539816f);
tmp247=sin(tmp246);
tmp248=(-1.0f)*tmp244*tmp245*tmp247;
tmp249=tmp11+(-0.78539816f);
tmp250=cos(tmp249);
tmp251=sin(tmp140);
tmp252=(-1.0f)*tmp250*tmp251;
tmp253=tmp248+tmp252;
tmp254=tmp242*tmp253;
tmp255=cos(tmp140);
tmp256=tmp3+(-0.78539816f);
tmp257=sin(tmp256);
tmp258=sin(tmp6);
tmp259=tmp255*tmp257*tmp258;
tmp260=tmp254+tmp259;
tmp261=tmp241*tmp260;
tmp262=tmp20+0.78539816f;
tmp263=sin(tmp262);
tmp264=tmp3+(-0.78539816f);
tmp265=cos(tmp264);
tmp266=tmp11+(-0.78539816f);
tmp267=cos(tmp266);
tmp268=cos(tmp140);
tmp269=tmp265*tmp267*tmp268;
tmp270=tmp11+(-0.78539816f);
tmp271=sin(tmp270);
tmp272=sin(tmp140);
tmp273=(-1.0f)*tmp271*tmp272;
tmp274=tmp269+tmp273;
tmp275=(-1.0f)*tmp263*tmp274;
tmp276=tmp261+tmp275;
tmp277=tmp239*tmp276;
tmp278=tmp0+1.134464f;
tmp279=sin(tmp278);
tmp280=cos(tmp6);
tmp281=cos(tmp140);
tmp282=tmp3+(-0.78539816f);
tmp283=sin(tmp282);
tmp284=tmp280*tmp281*tmp283;
tmp285=sin(tmp6);
tmp286=tmp3+(-0.78539816f);
tmp287=cos(tmp286);
tmp288=cos(tmp140);
tmp289=tmp11+(-0.78539816f);
tmp290=sin(tmp289);
tmp291=(-1.0f)*tmp287*tmp288*tmp290;
tmp292=tmp11+(-0.78539816f);
tmp293=cos(tmp292);
tmp294=sin(tmp140);
tmp295=(-1.0f)*tmp293*tmp294;
tmp296=tmp291+tmp295;
tmp297=(-1.0f)*tmp285*tmp296;
tmp298=tmp284+tmp297;
tmp299=tmp279*tmp298;
tmp300=tmp277+tmp299;
ans(1,2)=tmp300;
tmp301=sin(tmp140);
tmp302=(-14.0f)*tmp301;
tmp303=tmp11+(-0.78539816f);
tmp304=sin(tmp303);
tmp305=sin(tmp140);
tmp306=(-8.5f)*tmp304*tmp305;
tmp307=cos(tmp140);
tmp308=2.375f*tmp307;
tmp309=tmp3+(-0.78539816f);
tmp310=cos(tmp309);
tmp311=cos(tmp140);
tmp312=3.625f*tmp310*tmp311;
tmp313=tmp3+(-0.78539816f);
tmp314=cos(tmp313);
tmp315=tmp11+(-0.78539816f);
tmp316=cos(tmp315);
tmp317=cos(tmp140);
tmp318=8.5f*tmp314*tmp316*tmp317;
tmp319=tmp20+0.78539816f;
tmp320=cos(tmp319);
tmp321=tmp3+(-0.78539816f);
tmp322=cos(tmp321);
tmp323=tmp11+(-0.78539816f);
tmp324=cos(tmp323);
tmp325=cos(tmp140);
tmp326=tmp322*tmp324*tmp325;
tmp327=tmp11+(-0.78539816f);
tmp328=sin(tmp327);
tmp329=sin(tmp140);
tmp330=(-1.0f)*tmp328*tmp329;
tmp331=tmp326+tmp330;
tmp332=14.0f*tmp320*tmp331;
tmp333=tmp20+0.78539816f;
tmp334=sin(tmp333);
tmp335=cos(tmp6);
tmp336=tmp3+(-0.78539816f);
tmp337=cos(tmp336);
tmp338=cos(tmp140);
tmp339=tmp11+(-0.78539816f);
tmp340=sin(tmp339);
tmp341=(-1.0f)*tmp337*tmp338*tmp340;
tmp342=tmp11+(-0.78539816f);
tmp343=cos(tmp342);
tmp344=sin(tmp140);
tmp345=(-1.0f)*tmp343*tmp344;
tmp346=tmp341+tmp345;
tmp347=tmp335*tmp346;
tmp348=cos(tmp140);
tmp349=tmp3+(-0.78539816f);
tmp350=sin(tmp349);
tmp351=sin(tmp6);
tmp352=tmp348*tmp350*tmp351;
tmp353=tmp347+tmp352;
tmp354=14.0f*tmp334*tmp353;
tmp355=tmp302+tmp306+tmp308+tmp312+tmp318+tmp332+tmp354;
ans(1,3)=tmp355;
tmp356=tmp0+1.134464f;
tmp357=cos(tmp356);
tmp358=cos(tmp6);
tmp359=tmp3+(-0.78539816f);
tmp360=sin(tmp359);
tmp361=sin(tmp140);
tmp362=tmp358*tmp360*tmp361;
tmp363=sin(tmp6);
tmp364=tmp11+(-0.78539816f);
tmp365=cos(tmp364);
tmp366=cos(tmp140);
tmp367=tmp365*tmp366;
tmp368=tmp3+(-0.78539816f);
tmp369=cos(tmp368);
tmp370=tmp11+(-0.78539816f);
tmp371=sin(tmp370);
tmp372=sin(tmp140);
tmp373=(-1.0f)*tmp369*tmp371*tmp372;
tmp374=tmp367+tmp373;
tmp375=(-1.0f)*tmp363*tmp374;
tmp376=tmp362+tmp375;
tmp377=tmp357*tmp376;
tmp378=tmp0+1.134464f;
tmp379=sin(tmp378);
tmp380=tmp20+0.78539816f;
tmp381=cos(tmp380);
tmp382=cos(tmp6);
tmp383=tmp11+(-0.78539816f);
tmp384=cos(tmp383);
tmp385=cos(tmp140);
tmp386=tmp384*tmp385;
tmp387=tmp3+(-0.78539816f);
tmp388=cos(tmp387);
tmp389=tmp11+(-0.78539816f);
tmp390=sin(tmp389);
tmp391=sin(tmp140);
tmp392=(-1.0f)*tmp388*tmp390*tmp391;
tmp393=tmp386+tmp392;
tmp394=tmp382*tmp393;
tmp395=tmp3+(-0.78539816f);
tmp396=sin(tmp395);
tmp397=sin(tmp6);
tmp398=sin(tmp140);
tmp399=tmp396*tmp397*tmp398;
tmp400=tmp394+tmp399;
tmp401=tmp381*tmp400;
tmp402=tmp20+0.78539816f;
tmp403=sin(tmp402);
tmp404=tmp3+(-0.78539816f);
tmp405=cos(tmp404);
tmp406=tmp11+(-0.78539816f);
tmp407=cos(tmp406);
tmp408=sin(tmp140);
tmp409=tmp405*tmp407*tmp408;
tmp410=cos(tmp140);
tmp411=tmp11+(-0.78539816f);
tmp412=sin(tmp411);
tmp413=tmp410*tmp412;
tmp414=tmp409+tmp413;
tmp415=(-1.0f)*tmp403*tmp414;
tmp416=tmp401+tmp415;
tmp417=(-1.0f)*tmp379*tmp416;
tmp418=tmp377+tmp417;
ans(2,0)=tmp418;
tmp419=tmp20+0.78539816f;
tmp420=cos(tmp419);
tmp421=tmp3+(-0.78539816f);
tmp422=cos(tmp421);
tmp423=tmp11+(-0.78539816f);
tmp424=cos(tmp423);
tmp425=sin(tmp140);
tmp426=tmp422*tmp424*tmp425;
tmp427=cos(tmp140);
tmp428=tmp11+(-0.78539816f);
tmp429=sin(tmp428);
tmp430=tmp427*tmp429;
tmp431=tmp426+tmp430;
tmp432=tmp420*tmp431;
tmp433=tmp20+0.78539816f;
tmp434=sin(tmp433);
tmp435=cos(tmp6);
tmp436=tmp11+(-0.78539816f);
tmp437=cos(tmp436);
tmp438=cos(tmp140);
tmp439=tmp437*tmp438;
tmp440=tmp3+(-0.78539816f);
tmp441=cos(tmp440);
tmp442=tmp11+(-0.78539816f);
tmp443=sin(tmp442);
tmp444=sin(tmp140);
tmp445=(-1.0f)*tmp441*tmp443*tmp444;
tmp446=tmp439+tmp445;
tmp447=tmp435*tmp446;
tmp448=tmp3+(-0.78539816f);
tmp449=sin(tmp448);
tmp450=sin(tmp6);
tmp451=sin(tmp140);
tmp452=tmp449*tmp450*tmp451;
tmp453=tmp447+tmp452;
tmp454=tmp434*tmp453;
tmp455=tmp432+tmp454;
ans(2,1)=tmp455;
tmp456=tmp0+1.134464f;
tmp457=cos(tmp456);
tmp458=tmp20+0.78539816f;
tmp459=cos(tmp458);
tmp460=cos(tmp6);
tmp461=tmp11+(-0.78539816f);
tmp462=cos(tmp461);
tmp463=cos(tmp140);
tmp464=tmp462*tmp463;
tmp465=tmp3+(-0.78539816f);
tmp466=cos(tmp465);
tmp467=tmp11+(-0.78539816f);
tmp468=sin(tmp467);
tmp469=sin(tmp140);
tmp470=(-1.0f)*tmp466*tmp468*tmp469;
tmp471=tmp464+tmp470;
tmp472=tmp460*tmp471;
tmp473=tmp3+(-0.78539816f);
tmp474=sin(tmp473);
tmp475=sin(tmp6);
tmp476=sin(tmp140);
tmp477=tmp474*tmp475*tmp476;
tmp478=tmp472+tmp477;
tmp479=tmp459*tmp478;
tmp480=tmp20+0.78539816f;
tmp481=sin(tmp480);
tmp482=tmp3+(-0.78539816f);
tmp483=cos(tmp482);
tmp484=tmp11+(-0.78539816f);
tmp485=cos(tmp484);
tmp486=sin(tmp140);
tmp487=tmp483*tmp485*tmp486;
tmp488=cos(tmp140);
tmp489=tmp11+(-0.78539816f);
tmp490=sin(tmp489);
tmp491=tmp488*tmp490;
tmp492=tmp487+tmp491;
tmp493=(-1.0f)*tmp481*tmp492;
tmp494=tmp479+tmp493;
tmp495=tmp457*tmp494;
tmp496=tmp0+1.134464f;
tmp497=sin(tmp496);
tmp498=cos(tmp6);
tmp499=tmp3+(-0.78539816f);
tmp500=sin(tmp499);
tmp501=sin(tmp140);
tmp502=tmp498*tmp500*tmp501;
tmp503=sin(tmp6);
tmp504=tmp11+(-0.78539816f);
tmp505=cos(tmp504);
tmp506=cos(tmp140);
tmp507=tmp505*tmp506;
tmp508=tmp3+(-0.78539816f);
tmp509=cos(tmp508);
tmp510=tmp11+(-0.78539816f);
tmp511=sin(tmp510);
tmp512=sin(tmp140);
tmp513=(-1.0f)*tmp509*tmp511*tmp512;
tmp514=tmp507+tmp513;
tmp515=(-1.0f)*tmp503*tmp514;
tmp516=tmp502+tmp515;
tmp517=tmp497*tmp516;
tmp518=tmp495+tmp517;
ans(2,2)=tmp518;
tmp519=sin(tmp140);
tmp520=2.375f*tmp519;
tmp521=tmp3+(-0.78539816f);
tmp522=cos(tmp521);
tmp523=sin(tmp140);
tmp524=3.625f*tmp522*tmp523;
tmp525=tmp3+(-0.78539816f);
tmp526=cos(tmp525);
tmp527=tmp11+(-0.78539816f);
tmp528=cos(tmp527);
tmp529=sin(tmp140);
tmp530=8.5f*tmp526*tmp528*tmp529;
tmp531=cos(tmp140);
tmp532=tmp11+(-0.78539816f);
tmp533=sin(tmp532);
tmp534=8.5f*tmp531*tmp533;
tmp535=tmp20+0.78539816f;
tmp536=cos(tmp535);
tmp537=tmp3+(-0.78539816f);
tmp538=cos(tmp537);
tmp539=tmp11+(-0.78539816f);
tmp540=cos(tmp539);
tmp541=sin(tmp140);
tmp542=tmp538*tmp540*tmp541;
tmp543=cos(tmp140);
tmp544=tmp11+(-0.78539816f);
tmp545=sin(tmp544);
tmp546=tmp543*tmp545;
tmp547=tmp542+tmp546;
tmp548=14.0f*tmp536*tmp547;
tmp549=cos(tmp140);
tmp550=14.0f*tmp549;
tmp551=tmp20+0.78539816f;
tmp552=sin(tmp551);
tmp553=cos(tmp6);
tmp554=tmp11+(-0.78539816f);
tmp555=cos(tmp554);
tmp556=cos(tmp140);
tmp557=tmp555*tmp556;
tmp558=tmp3+(-0.78539816f);
tmp559=cos(tmp558);
tmp560=tmp11+(-0.78539816f);
tmp561=sin(tmp560);
tmp562=sin(tmp140);
tmp563=(-1.0f)*tmp559*tmp561*tmp562;
tmp564=tmp557+tmp563;
tmp565=tmp553*tmp564;
tmp566=tmp3+(-0.78539816f);
tmp567=sin(tmp566);
tmp568=sin(tmp6);
tmp569=sin(tmp140);
tmp570=tmp567*tmp568*tmp569;
tmp571=tmp565+tmp570;
tmp572=14.0f*tmp552*tmp571;
tmp573=tmp520+tmp524+tmp530+tmp534+tmp548+tmp550+tmp572+12.0f;
ans(2,3)=tmp573;
  return ans;
}