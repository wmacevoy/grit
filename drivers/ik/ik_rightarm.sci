
// declare globals

global  ik_rightarmepsilon ik_rightarmsteps;

// define function

function ik_rightarmy=ik_rightarmf(ik_rightarmparameters,ik_rightarmx)
  
  // declare globals
  
  global  ik_rightarmepsilon ik_rightarmsteps;
  
    // import globals
  
    epsilon=ik_rightarmepsilon; // epsilon
    steps=ik_rightarmsteps; // steps
  
    // import parameters
  
  RIGHT_SHOULDER_IO=ik_rightarmparameters(1);
  RIGHT_SHOULDER_UD=ik_rightarmparameters(2);
  RIGHT_BICEP_ROTATE=ik_rightarmparameters(3);
  RIGHT_ELBOW=ik_rightarmparameters(4);
  RIGHT_FOREARM_ROTATE=ik_rightarmparameters(5);
  RIGHTARM_BICEP_ROTATE=ik_rightarmparameters(6);
  RIGHTARM_ELBOW=ik_rightarmparameters(7);
  RIGHTARM_FOREARM_ROTATE=ik_rightarmparameters(8);
  RIGHTARM_SHOULDER_IO=ik_rightarmparameters(9);
  RIGHTARM_SHOULDER_UD=ik_rightarmparameters(10);
  nx=ik_rightarmparameters(11);
  ny=ik_rightarmparameters(12);
  px=ik_rightarmparameters(13);
  py=ik_rightarmparameters(14);
  pz=ik_rightarmparameters(15);
  residual=ik_rightarmparameters(16);
  
  // import variables
  
  RIGHT_SHOULDER_IO_=ik_rightarmx(1);
  RIGHT_SHOULDER_UD_=ik_rightarmx(2);
  RIGHT_BICEP_ROTATE_=ik_rightarmx(3);
  RIGHT_ELBOW_=ik_rightarmx(4);
  RIGHT_FOREARM_ROTATE_=ik_rightarmx(5);
  residual0=(-1)*px+3.625*sin(0.0174532925199433*RIGHTARM_SHOULDER_IO)+9*cos(0.0174532925199433*RIGHTARM_SHOULDER_UD)*sin(0.0174532925199433*RIGHTARM_SHOULDER_IO)+15*cos(0.0174532925199433*RIGHTARM_ELBOW)*cos(0.0174532925199433*RIGHTARM_SHOULDER_UD)*sin(0.0174532925199433*RIGHTARM_SHOULDER_IO)+15*sin(0.0174532925199433*RIGHTARM_ELBOW)*(cos(0.0174532925199433*RIGHTARM_SHOULDER_IO)*sin(0.0174532925199433*RIGHTARM_BICEP_ROTATE)+(-1)*cos(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*sin(0.0174532925199433*RIGHTARM_SHOULDER_IO)*sin(0.0174532925199433*RIGHTARM_SHOULDER_UD))+6;
  residual1=(-1)*py+3.625*cos(0.0174532925199433*RIGHTARM_SHOULDER_IO)+9*cos(0.0174532925199433*RIGHTARM_SHOULDER_IO)*cos(0.0174532925199433*RIGHTARM_SHOULDER_UD)+15*cos(0.0174532925199433*RIGHTARM_ELBOW)*cos(0.0174532925199433*RIGHTARM_SHOULDER_IO)*cos(0.0174532925199433*RIGHTARM_SHOULDER_UD)+15*sin(0.0174532925199433*RIGHTARM_ELBOW)*((-1)*cos(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*cos(0.0174532925199433*RIGHTARM_SHOULDER_IO)*sin(0.0174532925199433*RIGHTARM_SHOULDER_UD)+(-1)*sin(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*sin(0.0174532925199433*RIGHTARM_SHOULDER_IO))+2.375;
  residual2=(-1)*pz+9*sin(0.0174532925199433*RIGHTARM_SHOULDER_UD)+15*cos(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*cos(0.0174532925199433*RIGHTARM_SHOULDER_UD)*sin(0.0174532925199433*RIGHTARM_ELBOW)+15*cos(0.0174532925199433*RIGHTARM_ELBOW)*sin(0.0174532925199433*RIGHTARM_SHOULDER_UD);
  residual3=(-1)*cos(0.0174532925199433*RIGHTARM_FOREARM_ROTATE)*(cos(0.0174532925199433*RIGHTARM_ELBOW)*(cos(0.0174532925199433*RIGHTARM_SHOULDER_IO)*sin(0.0174532925199433*RIGHTARM_BICEP_ROTATE)+(-1)*cos(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*sin(0.0174532925199433*RIGHTARM_SHOULDER_IO)*sin(0.0174532925199433*RIGHTARM_SHOULDER_UD))+(-1)*cos(0.0174532925199433*RIGHTARM_SHOULDER_UD)*sin(0.0174532925199433*RIGHTARM_ELBOW)*sin(0.0174532925199433*RIGHTARM_SHOULDER_IO))+(-1)*sin(0.0174532925199433*RIGHTARM_FOREARM_ROTATE)*(cos(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*cos(0.0174532925199433*RIGHTARM_SHOULDER_IO)+sin(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*sin(0.0174532925199433*RIGHTARM_SHOULDER_IO)*sin(0.0174532925199433*RIGHTARM_SHOULDER_UD))+(-1)*nx;
  residual4=(-1)*cos(0.0174532925199433*RIGHTARM_FOREARM_ROTATE)*(cos(0.0174532925199433*RIGHTARM_ELBOW)*((-1)*cos(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*cos(0.0174532925199433*RIGHTARM_SHOULDER_IO)*sin(0.0174532925199433*RIGHTARM_SHOULDER_UD)+(-1)*sin(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*sin(0.0174532925199433*RIGHTARM_SHOULDER_IO))+(-1)*cos(0.0174532925199433*RIGHTARM_SHOULDER_IO)*cos(0.0174532925199433*RIGHTARM_SHOULDER_UD)*sin(0.0174532925199433*RIGHTARM_ELBOW))+(-1)*sin(0.0174532925199433*RIGHTARM_FOREARM_ROTATE)*(cos(0.0174532925199433*RIGHTARM_SHOULDER_IO)*sin(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*sin(0.0174532925199433*RIGHTARM_SHOULDER_UD)+(-1)*cos(0.0174532925199433*RIGHTARM_BICEP_ROTATE)*sin(0.0174532925199433*RIGHTARM_SHOULDER_IO))+(-1)*ny;
  ik_rightarmy=zeros(5,1);
  ik_rightarmy(1)=residual0;
  ik_rightarmy(2)=residual1;
  ik_rightarmy(3)=residual2;
  ik_rightarmy(4)=residual3;
  ik_rightarmy(5)=residual4;
endfunction

function ik_rightarmdy=ik_rightarmdf(ik_rightarmparameters,ik_rightarmx)
  
  // declare globals
  
  global  ik_rightarmepsilon ik_rightarmsteps;
  
    // import globals
  
    epsilon=ik_rightarmepsilon; // epsilon
    steps=ik_rightarmsteps; // steps
  
    // import parameters
  
  RIGHT_SHOULDER_IO=ik_rightarmparameters(1);
  RIGHT_SHOULDER_UD=ik_rightarmparameters(2);
  RIGHT_BICEP_ROTATE=ik_rightarmparameters(3);
  RIGHT_ELBOW=ik_rightarmparameters(4);
  RIGHT_FOREARM_ROTATE=ik_rightarmparameters(5);
  RIGHTARM_BICEP_ROTATE=ik_rightarmparameters(6);
  RIGHTARM_ELBOW=ik_rightarmparameters(7);
  RIGHTARM_FOREARM_ROTATE=ik_rightarmparameters(8);
  RIGHTARM_SHOULDER_IO=ik_rightarmparameters(9);
  RIGHTARM_SHOULDER_UD=ik_rightarmparameters(10);
  nx=ik_rightarmparameters(11);
  ny=ik_rightarmparameters(12);
  px=ik_rightarmparameters(13);
  py=ik_rightarmparameters(14);
  pz=ik_rightarmparameters(15);
  residual=ik_rightarmparameters(16);
  
  // import variables
  
  RIGHT_SHOULDER_IO_=ik_rightarmx(1);
  RIGHT_SHOULDER_UD_=ik_rightarmx(2);
  RIGHT_BICEP_ROTATE_=ik_rightarmx(3);
  RIGHT_ELBOW_=ik_rightarmx(4);
  RIGHT_FOREARM_ROTATE_=ik_rightarmx(5);
  dresidual0_dRIGHT_SHOULDER_IO_=0;
  dresidual0_dRIGHT_SHOULDER_UD_=0;
  dresidual0_dRIGHT_BICEP_ROTATE_=0;
  dresidual0_dRIGHT_ELBOW_=0;
  dresidual0_dRIGHT_FOREARM_ROTATE_=0;
  dresidual1_dRIGHT_SHOULDER_IO_=0;
  dresidual1_dRIGHT_SHOULDER_UD_=0;
  dresidual1_dRIGHT_BICEP_ROTATE_=0;
  dresidual1_dRIGHT_ELBOW_=0;
  dresidual1_dRIGHT_FOREARM_ROTATE_=0;
  dresidual2_dRIGHT_SHOULDER_IO_=0;
  dresidual2_dRIGHT_SHOULDER_UD_=0;
  dresidual2_dRIGHT_BICEP_ROTATE_=0;
  dresidual2_dRIGHT_ELBOW_=0;
  dresidual2_dRIGHT_FOREARM_ROTATE_=0;
  dresidual3_dRIGHT_SHOULDER_IO_=0;
  dresidual3_dRIGHT_SHOULDER_UD_=0;
  dresidual3_dRIGHT_BICEP_ROTATE_=0;
  dresidual3_dRIGHT_ELBOW_=0;
  dresidual3_dRIGHT_FOREARM_ROTATE_=0;
  dresidual4_dRIGHT_SHOULDER_IO_=0;
  dresidual4_dRIGHT_SHOULDER_UD_=0;
  dresidual4_dRIGHT_BICEP_ROTATE_=0;
  dresidual4_dRIGHT_ELBOW_=0;
  dresidual4_dRIGHT_FOREARM_ROTATE_=0;
  // place derivatives in dy
  ik_rightarmdy=zeros(5,5);
  ik_rightarmdy(1,1)=dresidual0_dRIGHT_SHOULDER_IO_;
  ik_rightarmdy(1,2)=dresidual0_dRIGHT_SHOULDER_UD_;
  ik_rightarmdy(1,3)=dresidual0_dRIGHT_BICEP_ROTATE_;
  ik_rightarmdy(1,4)=dresidual0_dRIGHT_ELBOW_;
  ik_rightarmdy(1,5)=dresidual0_dRIGHT_FOREARM_ROTATE_;
  ik_rightarmdy(2,1)=dresidual1_dRIGHT_SHOULDER_IO_;
  ik_rightarmdy(2,2)=dresidual1_dRIGHT_SHOULDER_UD_;
  ik_rightarmdy(2,3)=dresidual1_dRIGHT_BICEP_ROTATE_;
  ik_rightarmdy(2,4)=dresidual1_dRIGHT_ELBOW_;
  ik_rightarmdy(2,5)=dresidual1_dRIGHT_FOREARM_ROTATE_;
  ik_rightarmdy(3,1)=dresidual2_dRIGHT_SHOULDER_IO_;
  ik_rightarmdy(3,2)=dresidual2_dRIGHT_SHOULDER_UD_;
  ik_rightarmdy(3,3)=dresidual2_dRIGHT_BICEP_ROTATE_;
  ik_rightarmdy(3,4)=dresidual2_dRIGHT_ELBOW_;
  ik_rightarmdy(3,5)=dresidual2_dRIGHT_FOREARM_ROTATE_;
  ik_rightarmdy(4,1)=dresidual3_dRIGHT_SHOULDER_IO_;
  ik_rightarmdy(4,2)=dresidual3_dRIGHT_SHOULDER_UD_;
  ik_rightarmdy(4,3)=dresidual3_dRIGHT_BICEP_ROTATE_;
  ik_rightarmdy(4,4)=dresidual3_dRIGHT_ELBOW_;
  ik_rightarmdy(4,5)=dresidual3_dRIGHT_FOREARM_ROTATE_;
  ik_rightarmdy(5,1)=dresidual4_dRIGHT_SHOULDER_IO_;
  ik_rightarmdy(5,2)=dresidual4_dRIGHT_SHOULDER_UD_;
  ik_rightarmdy(5,3)=dresidual4_dRIGHT_BICEP_ROTATE_;
  ik_rightarmdy(5,4)=dresidual4_dRIGHT_ELBOW_;
  ik_rightarmdy(5,5)=dresidual4_dRIGHT_FOREARM_ROTATE_;
endfunction

function ik_rightarmx=ik_rightarmsolve(ik_rightarmparameters)
  
  // declare globals
  
  global  ik_rightarmepsilon ik_rightarmsteps;
  
    // import globals
  
    epsilon=ik_rightarmepsilon; // epsilon
    steps=ik_rightarmsteps; // steps
  
    // import parameters
  
  RIGHT_SHOULDER_IO=ik_rightarmparameters(1);
  RIGHT_SHOULDER_UD=ik_rightarmparameters(2);
  RIGHT_BICEP_ROTATE=ik_rightarmparameters(3);
  RIGHT_ELBOW=ik_rightarmparameters(4);
  RIGHT_FOREARM_ROTATE=ik_rightarmparameters(5);
  RIGHTARM_BICEP_ROTATE=ik_rightarmparameters(6);
  RIGHTARM_ELBOW=ik_rightarmparameters(7);
  RIGHTARM_FOREARM_ROTATE=ik_rightarmparameters(8);
  RIGHTARM_SHOULDER_IO=ik_rightarmparameters(9);
  RIGHTARM_SHOULDER_UD=ik_rightarmparameters(10);
  nx=ik_rightarmparameters(11);
  ny=ik_rightarmparameters(12);
  px=ik_rightarmparameters(13);
  py=ik_rightarmparameters(14);
  pz=ik_rightarmparameters(15);
  residual=ik_rightarmparameters(16);
  ik_rightarmresidual=0;
  
  // initialize unknowns from parameters
  
  ik_rightarmx=zeros(5,1);
  RIGHT_SHOULDER_IO_=RIGHT_SHOULDER_IO;
  RIGHT_SHOULDER_UD_=RIGHT_SHOULDER_UD;
  RIGHT_BICEP_ROTATE_=RIGHT_BICEP_ROTATE;
  RIGHT_ELBOW_=RIGHT_ELBOW;
  RIGHT_FOREARM_ROTATE_=RIGHT_FOREARM_ROTATE;
  
  // setup unknown vector x
  
  ik_rightarmx=zeros(5,1);
  ik_rightarmx(1)=RIGHT_SHOULDER_IO_;
  ik_rightarmx(2)=RIGHT_SHOULDER_UD_;
  ik_rightarmx(3)=RIGHT_BICEP_ROTATE_;
  ik_rightarmx(4)=RIGHT_ELBOW_;
  ik_rightarmx(5)=RIGHT_FOREARM_ROTATE_;
  
  // newton iteration
  for step=1:steps
    ik_rightarmy=ik_rightarmf(ik_rightarmparameters,ik_rightarmx);
    ik_rightarmdy=ik_rightarmdf(ik_rightarmparameters,ik_rightarmx);
    ik_rightarmx=ik_rightarmx-ik_rightarmdy\ik_rightarmy;
    ik_rightarmresidual=norm(ik_rightarmy);
    if (ik_rightarmresidual <= epsilon) then
      break;
    end
  end
  ik_rightarmparameters(16)=ik_rightarmresidual;
endfunction

function ik_rightarmnew_parameters=ik_rightarmupdate(ik_rightarmparameters)
  
  // declare globals
  
  global  ik_rightarmepsilon ik_rightarmsteps;
  
    // import globals
  
    epsilon=ik_rightarmepsilon; // epsilon
    steps=ik_rightarmsteps; // steps
  
    // import parameters
  
  RIGHT_SHOULDER_IO=ik_rightarmparameters(1);
  RIGHT_SHOULDER_UD=ik_rightarmparameters(2);
  RIGHT_BICEP_ROTATE=ik_rightarmparameters(3);
  RIGHT_ELBOW=ik_rightarmparameters(4);
  RIGHT_FOREARM_ROTATE=ik_rightarmparameters(5);
  RIGHTARM_BICEP_ROTATE=ik_rightarmparameters(6);
  RIGHTARM_ELBOW=ik_rightarmparameters(7);
  RIGHTARM_FOREARM_ROTATE=ik_rightarmparameters(8);
  RIGHTARM_SHOULDER_IO=ik_rightarmparameters(9);
  RIGHTARM_SHOULDER_UD=ik_rightarmparameters(10);
  nx=ik_rightarmparameters(11);
  ny=ik_rightarmparameters(12);
  px=ik_rightarmparameters(13);
  py=ik_rightarmparameters(14);
  pz=ik_rightarmparameters(15);
  residual=ik_rightarmparameters(16);
  ik_rightarmx=ik_rightarmsolve(ik_rightarmparameters);
  RIGHT_SHOULDER_IO=RIGHT_SHOULDER_IO_;
  RIGHT_SHOULDER_UD=RIGHT_SHOULDER_UD_;
  RIGHT_BICEP_ROTATE=RIGHT_BICEP_ROTATE_;
  RIGHT_ELBOW=RIGHT_ELBOW_;
  RIGHT_FOREARM_ROTATE=RIGHT_FOREARM_ROTATE_;
  
  // assign to new_parameters
  
  ik_rightarmnew_parameters=zeros(5,1);
  ik_rightarmnew_parameters(1)=RIGHT_SHOULDER_IO;
  ik_rightarmnew_parameters(2)=RIGHT_SHOULDER_UD;
  ik_rightarmnew_parameters(3)=RIGHT_BICEP_ROTATE;
  ik_rightarmnew_parameters(4)=RIGHT_ELBOW;
  ik_rightarmnew_parameters(5)=RIGHT_FOREARM_ROTATE;
  ik_rightarmnew_parameters(6)=RIGHTARM_BICEP_ROTATE;
  ik_rightarmnew_parameters(7)=RIGHTARM_ELBOW;
  ik_rightarmnew_parameters(8)=RIGHTARM_FOREARM_ROTATE;
  ik_rightarmnew_parameters(9)=RIGHTARM_SHOULDER_IO;
  ik_rightarmnew_parameters(10)=RIGHTARM_SHOULDER_UD;
  ik_rightarmnew_parameters(11)=nx;
  ik_rightarmnew_parameters(12)=ny;
  ik_rightarmnew_parameters(13)=px;
  ik_rightarmnew_parameters(14)=py;
  ik_rightarmnew_parameters(15)=pz;
  ik_rightarmnew_parameters(16)=residual;
endfunction

function ik_rightarmtests(ik_rightarmname)
  
  // declare globals
  
  global  ik_rightarmepsilon ik_rightarmsteps;
  ik_rightarmcase_name='';
  ik_rightarmcase_count=1;
  ik_rightarmmax_x_error_name="none";
  ik_rightarmmax_x_error_count=0;
  ik_rightarmmax_x_error=0;
  ik_rightarmx_error=0;
  ik_rightarmmax_y_error_name="none";
  ik_rightarmmax_y_error_count=0;
  ik_rightarmmax_y_error=0;
  ik_rightarmy_error=0;
  if (ik_rightarmname == "all") then
    printf("test summary: max_x_error=%1.15g from %d/""%s"", max_y_error=%1.15g from %d/""%s""\n",ik_rightarmmax_x_error,ik_rightarmmax_x_error_count,ik_rightarmmax_x_error_name,ik_rightarmmax_y_error,ik_rightarmmax_y_error_count,ik_rightarmmax_y_error_name);
  end
endfunction
function ik_rightarmruns(name)
  
  // declare globals
  
  global  ik_rightarmepsilon ik_rightarmsteps;
  ik_rightarmcase_count=1;
endfunction
