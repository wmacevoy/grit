
// declare globals

global  ik_legepsilon ik_legsteps;

// define function

function ik_legy=ik_legf(ik_legparameters,ik_legx)
  
  // declare globals
  
  global  ik_legepsilon ik_legsteps;
  
    // import globals
  
    epsilon=ik_legepsilon; // epsilon
    steps=ik_legsteps; // steps
  
    // import parameters
  
  knee=ik_legparameters(1);
  femur=ik_legparameters(2);
  hip=ik_legparameters(3);
  chasis=ik_legparameters(4);
  dfemur2knee=ik_legparameters(5);
  dhip2femur=ik_legparameters(6);
  drchasis2hip=ik_legparameters(7);
  dyknee2tip=ik_legparameters(8);
  dzchasis2hip=ik_legparameters(9);
  dzknee2tip=ik_legparameters(10);
  px=ik_legparameters(11);
  py=ik_legparameters(12);
  pz=ik_legparameters(13);
  residual=ik_legparameters(14);
  
  // import variables
  
  _knee=ik_legx(1);
  _femur=ik_legx(2);
  _hip=ik_legx(3);
  residual0=cos(0.0174532925199433*_femur)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))*dfemur2knee+(cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))+(-1)*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip)))*dyknee2tip+((-1)*cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))+(-1)*cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip)))*dzknee2tip+((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))*dhip2femur+(-1)*sin(0.0174532925199433*chasis)*drchasis2hip+(-1)*px;
  residual1=cos(0.0174532925199433*_femur)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))*dfemur2knee+cos(0.0174532925199433*chasis)*drchasis2hip+(cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))+(-1)*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)))*dyknee2tip+(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))*dhip2femur+((-1)*cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))+(-1)*cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)))*dzknee2tip+(-1)*py;
  residual2=sin(0.0174532925199433*_femur)*dfemur2knee+(cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee)+(-1)*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee))*dzknee2tip+(cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)+cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur))*dyknee2tip+(-1)*pz+dzchasis2hip;
  ik_legy=zeros(3,1);
  ik_legy(1)=residual0;
  ik_legy(2)=residual1;
  ik_legy(3)=residual2;
endfunction

function ik_legdy=ik_legdf(ik_legparameters,ik_legx)
  
  // declare globals
  
  global  ik_legepsilon ik_legsteps;
  
    // import globals
  
    epsilon=ik_legepsilon; // epsilon
    steps=ik_legsteps; // steps
  
    // import parameters
  
  knee=ik_legparameters(1);
  femur=ik_legparameters(2);
  hip=ik_legparameters(3);
  chasis=ik_legparameters(4);
  dfemur2knee=ik_legparameters(5);
  dhip2femur=ik_legparameters(6);
  drchasis2hip=ik_legparameters(7);
  dyknee2tip=ik_legparameters(8);
  dzchasis2hip=ik_legparameters(9);
  dzknee2tip=ik_legparameters(10);
  px=ik_legparameters(11);
  py=ik_legparameters(12);
  pz=ik_legparameters(13);
  residual=ik_legparameters(14);
  
  // import variables
  
  _knee=ik_legx(1);
  _femur=ik_legx(2);
  _hip=ik_legx(3);
  dresidual0_d_knee=((-0.0174532925199433)*cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))+0.0174532925199433*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip)))*dzknee2tip+((-0.0174532925199433)*cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))+(-0.0174532925199433)*cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip)))*dyknee2tip;
  dresidual0_d_femur=((-0.0174532925199433)*cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))+0.0174532925199433*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip)))*dzknee2tip+((-0.0174532925199433)*cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))+(-0.0174532925199433)*cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip)))*dyknee2tip+(-0.0174532925199433)*sin(0.0174532925199433*_femur)*((-1)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-1)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))*dfemur2knee;
  dresidual0_d_hip=cos(0.0174532925199433*_femur)*((-0.0174532925199433)*cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+0.0174532925199433*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))*dfemur2knee+(cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee)*((-0.0174532925199433)*cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+0.0174532925199433*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))+(-1)*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*((-0.0174532925199433)*cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+0.0174532925199433*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)))*dyknee2tip+((-1)*cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*((-0.0174532925199433)*cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+0.0174532925199433*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))+(-1)*cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur)*((-0.0174532925199433)*cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+0.0174532925199433*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)))*dzknee2tip+((-0.0174532925199433)*cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+0.0174532925199433*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))*dhip2femur;
  dresidual1_d_knee=((-0.0174532925199433)*cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))+0.0174532925199433*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)))*dzknee2tip+((-0.0174532925199433)*cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))+(-0.0174532925199433)*cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)))*dyknee2tip;
  dresidual1_d_femur=((-0.0174532925199433)*cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))+0.0174532925199433*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)))*dzknee2tip+((-0.0174532925199433)*cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))+(-0.0174532925199433)*cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)))*dyknee2tip+(-0.0174532925199433)*sin(0.0174532925199433*_femur)*(cos(0.0174532925199433*_hip)*cos(0.0174532925199433*chasis)+(-1)*sin(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis))*dfemur2knee;
  dresidual1_d_hip=cos(0.0174532925199433*_femur)*((-0.0174532925199433)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-0.0174532925199433)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))*dfemur2knee+(cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee)*((-0.0174532925199433)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-0.0174532925199433)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))+(-1)*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*((-0.0174532925199433)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-0.0174532925199433)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip)))*dyknee2tip+((-1)*cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)*((-0.0174532925199433)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-0.0174532925199433)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))+(-1)*cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur)*((-0.0174532925199433)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-0.0174532925199433)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip)))*dzknee2tip+((-0.0174532925199433)*cos(0.0174532925199433*_hip)*sin(0.0174532925199433*chasis)+(-0.0174532925199433)*cos(0.0174532925199433*chasis)*sin(0.0174532925199433*_hip))*dhip2femur;
  dresidual2_d_knee=((-0.0174532925199433)*cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)+(-0.0174532925199433)*cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur))*dzknee2tip+((-0.0174532925199433)*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)+0.0174532925199433*cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee))*dyknee2tip;
  dresidual2_d_femur=((-0.0174532925199433)*cos(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)+(-0.0174532925199433)*cos(0.0174532925199433*_knee)*sin(0.0174532925199433*_femur))*dzknee2tip+((-0.0174532925199433)*sin(0.0174532925199433*_femur)*sin(0.0174532925199433*_knee)+0.0174532925199433*cos(0.0174532925199433*_femur)*cos(0.0174532925199433*_knee))*dyknee2tip+0.0174532925199433*cos(0.0174532925199433*_femur)*dfemur2knee;
  dresidual2_d_hip=0;
  // place derivatives in dy
  ik_legdy=zeros(3,3);
  ik_legdy(1,1)=dresidual0_d_knee;
  ik_legdy(1,2)=dresidual0_d_femur;
  ik_legdy(1,3)=dresidual0_d_hip;
  ik_legdy(2,1)=dresidual1_d_knee;
  ik_legdy(2,2)=dresidual1_d_femur;
  ik_legdy(2,3)=dresidual1_d_hip;
  ik_legdy(3,1)=dresidual2_d_knee;
  ik_legdy(3,2)=dresidual2_d_femur;
  ik_legdy(3,3)=dresidual2_d_hip;
endfunction

function ik_legx=ik_legsolve(ik_legparameters)
  
  // declare globals
  
  global  ik_legepsilon ik_legsteps;
  
    // import globals
  
    epsilon=ik_legepsilon; // epsilon
    steps=ik_legsteps; // steps
  
    // import parameters
  
  knee=ik_legparameters(1);
  femur=ik_legparameters(2);
  hip=ik_legparameters(3);
  chasis=ik_legparameters(4);
  dfemur2knee=ik_legparameters(5);
  dhip2femur=ik_legparameters(6);
  drchasis2hip=ik_legparameters(7);
  dyknee2tip=ik_legparameters(8);
  dzchasis2hip=ik_legparameters(9);
  dzknee2tip=ik_legparameters(10);
  px=ik_legparameters(11);
  py=ik_legparameters(12);
  pz=ik_legparameters(13);
  residual=ik_legparameters(14);
  ik_legresidual=0;
  
  // initialize unknowns from parameters
  
  ik_legx=zeros(3,1);
  _knee=knee;
  _femur=femur;
  _hip=hip;
  
  // setup unknown vector x
  
  ik_legx=zeros(3,1);
  ik_legx(1)=_knee;
  ik_legx(2)=_femur;
  ik_legx(3)=_hip;
  
  // newton iteration
  for step=1:steps
    ik_legy=ik_legf(ik_legparameters,ik_legx);
    ik_legdy=ik_legdf(ik_legparameters,ik_legx);
    ik_legx=ik_legx-ik_legdy\ik_legy;
    ik_legresidual=norm(ik_legy);
    if (ik_legresidual <= epsilon) then
      break;
    end
  end
  ik_legparameters(14)=ik_legresidual;
endfunction

function ik_legnew_parameters=ik_legupdate(ik_legparameters)
  
  // declare globals
  
  global  ik_legepsilon ik_legsteps;
  
    // import globals
  
    epsilon=ik_legepsilon; // epsilon
    steps=ik_legsteps; // steps
  
    // import parameters
  
  knee=ik_legparameters(1);
  femur=ik_legparameters(2);
  hip=ik_legparameters(3);
  chasis=ik_legparameters(4);
  dfemur2knee=ik_legparameters(5);
  dhip2femur=ik_legparameters(6);
  drchasis2hip=ik_legparameters(7);
  dyknee2tip=ik_legparameters(8);
  dzchasis2hip=ik_legparameters(9);
  dzknee2tip=ik_legparameters(10);
  px=ik_legparameters(11);
  py=ik_legparameters(12);
  pz=ik_legparameters(13);
  residual=ik_legparameters(14);
  ik_legx=ik_legsolve(ik_legparameters);
  
  // import variables
  
  _knee=ik_legx(1);
  _femur=ik_legx(2);
  _hip=ik_legx(3);
  knee=_knee;
  femur=_femur;
  hip=_hip;
  
  // assign to new_parameters
  
  ik_legnew_parameters=zeros(14,1);
  ik_legnew_parameters(1)=knee;
  ik_legnew_parameters(2)=femur;
  ik_legnew_parameters(3)=hip;
  ik_legnew_parameters(4)=chasis;
  ik_legnew_parameters(5)=dfemur2knee;
  ik_legnew_parameters(6)=dhip2femur;
  ik_legnew_parameters(7)=drchasis2hip;
  ik_legnew_parameters(8)=dyknee2tip;
  ik_legnew_parameters(9)=dzchasis2hip;
  ik_legnew_parameters(10)=dzknee2tip;
  ik_legnew_parameters(11)=px;
  ik_legnew_parameters(12)=py;
  ik_legnew_parameters(13)=pz;
  ik_legnew_parameters(14)=residual;
endfunction

function ik_legtests(ik_legname)
  
  // declare globals
  
  global  ik_legepsilon ik_legsteps;
  ik_legcase_name='';
  ik_legcase_count=1;
  ik_legmax_x_error_name="none";
  ik_legmax_x_error_count=0;
  ik_legmax_x_error=0;
  ik_legx_error=0;
  ik_legmax_y_error_name="none";
  ik_legmax_y_error_count=0;
  ik_legmax_y_error=0;
  ik_legy_error=0;
  if (ik_legname == "all") then
    printf("test summary: max_x_error=%1.15g from %d/""%s"", max_y_error=%1.15g from %d/""%s""\n",ik_legmax_x_error,ik_legmax_x_error_count,ik_legmax_x_error_name,ik_legmax_y_error,ik_legmax_y_error_count,ik_legmax_y_error_name);
  end
endfunction
function ik_legruns(name)
  
  // declare globals
  
  global  ik_legepsilon ik_legsteps;
  ik_legcase_count=1;
endfunction
