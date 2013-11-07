
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
  
  aknee=ik_legparameters(1);
  afemur=ik_legparameters(2);
  ahip=ik_legparameters(3);
  achasis=ik_legparameters(4);
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
  
  aknee_=ik_legx(1);
  afemur_=ik_legx(2);
  ahip_=ik_legx(3);
  residual0=cos(afemur_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))*dfemur2knee+(cos(afemur_)*cos(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))+(-1)*sin(afemur_)*sin(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis)))*dyknee2tip+((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))*dhip2femur+((-1)*cos(afemur_)*sin(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))+(-1)*cos(aknee_)*sin(afemur_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis)))*dzknee2tip+(-1)*sin(achasis)*drchasis2hip+(-1)*px;
  residual1=cos(achasis)*drchasis2hip+cos(afemur_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_))*dfemur2knee+(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_))*dhip2femur+(cos(afemur_)*cos(aknee_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_))+(-1)*sin(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_)))*dyknee2tip+((-1)*cos(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_))+(-1)*cos(aknee_)*sin(afemur_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_)))*dzknee2tip+(-1)*py;
  residual2=sin(afemur_)*dfemur2knee+(cos(afemur_)*cos(aknee_)+(-1)*sin(afemur_)*sin(aknee_))*dzknee2tip+(cos(afemur_)*sin(aknee_)+cos(aknee_)*sin(afemur_))*dyknee2tip+(-1)*pz+dzchasis2hip;
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
  
  aknee=ik_legparameters(1);
  afemur=ik_legparameters(2);
  ahip=ik_legparameters(3);
  achasis=ik_legparameters(4);
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
  
  aknee_=ik_legx(1);
  afemur_=ik_legx(2);
  ahip_=ik_legx(3);
  dresidual0_daknee_=(sin(afemur_)*sin(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))+(-1)*cos(afemur_)*cos(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis)))*dzknee2tip+((-1)*cos(afemur_)*sin(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))+(-1)*cos(aknee_)*sin(afemur_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis)))*dyknee2tip;
  dresidual0_dafemur_=(sin(afemur_)*sin(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))+(-1)*cos(afemur_)*cos(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis)))*dzknee2tip+((-1)*cos(afemur_)*sin(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))+(-1)*cos(aknee_)*sin(afemur_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis)))*dyknee2tip+(-1)*sin(afemur_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))*dfemur2knee;
  dresidual0_dahip_=cos(afemur_)*(sin(achasis)*sin(ahip_)+(-1)*cos(achasis)*cos(ahip_))*dfemur2knee+(cos(afemur_)*cos(aknee_)*(sin(achasis)*sin(ahip_)+(-1)*cos(achasis)*cos(ahip_))+(-1)*sin(afemur_)*sin(aknee_)*(sin(achasis)*sin(ahip_)+(-1)*cos(achasis)*cos(ahip_)))*dyknee2tip+(sin(achasis)*sin(ahip_)+(-1)*cos(achasis)*cos(ahip_))*dhip2femur+((-1)*cos(afemur_)*sin(aknee_)*(sin(achasis)*sin(ahip_)+(-1)*cos(achasis)*cos(ahip_))+(-1)*cos(aknee_)*sin(afemur_)*(sin(achasis)*sin(ahip_)+(-1)*cos(achasis)*cos(ahip_)))*dzknee2tip;
  dresidual1_daknee_=(sin(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_))+(-1)*cos(afemur_)*cos(aknee_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_)))*dzknee2tip+((-1)*cos(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_))+(-1)*cos(aknee_)*sin(afemur_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_)))*dyknee2tip;
  dresidual1_dafemur_=(sin(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_))+(-1)*cos(afemur_)*cos(aknee_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_)))*dzknee2tip+((-1)*cos(afemur_)*sin(aknee_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_))+(-1)*cos(aknee_)*sin(afemur_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_)))*dyknee2tip+(-1)*sin(afemur_)*(cos(achasis)*cos(ahip_)+(-1)*sin(achasis)*sin(ahip_))*dfemur2knee;
  dresidual1_dahip_=cos(afemur_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))*dfemur2knee+(cos(afemur_)*cos(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))+(-1)*sin(afemur_)*sin(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis)))*dyknee2tip+((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))*dhip2femur+((-1)*cos(afemur_)*sin(aknee_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis))+(-1)*cos(aknee_)*sin(afemur_)*((-1)*cos(achasis)*sin(ahip_)+(-1)*cos(ahip_)*sin(achasis)))*dzknee2tip;
  dresidual2_daknee_=(cos(afemur_)*cos(aknee_)+(-1)*sin(afemur_)*sin(aknee_))*dyknee2tip+((-1)*cos(afemur_)*sin(aknee_)+(-1)*cos(aknee_)*sin(afemur_))*dzknee2tip;
  dresidual2_dafemur_=cos(afemur_)*dfemur2knee+(cos(afemur_)*cos(aknee_)+(-1)*sin(afemur_)*sin(aknee_))*dyknee2tip+((-1)*cos(afemur_)*sin(aknee_)+(-1)*cos(aknee_)*sin(afemur_))*dzknee2tip;
  dresidual2_dahip_=0;
  // place derivatives in dy
  ik_legdy=zeros(3,3);
  ik_legdy(1,1)=dresidual0_daknee_;
  ik_legdy(1,2)=dresidual0_dafemur_;
  ik_legdy(1,3)=dresidual0_dahip_;
  ik_legdy(2,1)=dresidual1_daknee_;
  ik_legdy(2,2)=dresidual1_dafemur_;
  ik_legdy(2,3)=dresidual1_dahip_;
  ik_legdy(3,1)=dresidual2_daknee_;
  ik_legdy(3,2)=dresidual2_dafemur_;
  ik_legdy(3,3)=dresidual2_dahip_;
endfunction

function ik_legx=ik_legsolve(ik_legparameters)
  
  // declare globals
  
  global  ik_legepsilon ik_legsteps;
  
    // import globals
  
    epsilon=ik_legepsilon; // epsilon
    steps=ik_legsteps; // steps
  
    // import parameters
  
  aknee=ik_legparameters(1);
  afemur=ik_legparameters(2);
  ahip=ik_legparameters(3);
  achasis=ik_legparameters(4);
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
  aknee_=aknee;
  afemur_=afemur;
  ahip_=ahip;
  
  // setup unknown vector x
  
  ik_legx=zeros(3,1);
  ik_legx(1)=aknee_;
  ik_legx(2)=afemur_;
  ik_legx(3)=ahip_;
  
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
  
  aknee=ik_legparameters(1);
  afemur=ik_legparameters(2);
  ahip=ik_legparameters(3);
  achasis=ik_legparameters(4);
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
  aknee=aknee_;
  afemur=afemur_;
  ahip=ahip_;
  
  // assign to new_parameters
  
  ik_legnew_parameters=zeros(3,1);
  ik_legnew_parameters(1)=aknee;
  ik_legnew_parameters(2)=afemur;
  ik_legnew_parameters(3)=ahip;
  ik_legnew_parameters(4)=achasis;
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
