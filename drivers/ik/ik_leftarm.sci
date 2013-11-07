
// declare globals

global  ik_leftarmepsilon ik_leftarmsteps;

// define function

function ik_leftarmy=ik_leftarmf(ik_leftarmparameters,ik_leftarmx)
  
  // declare globals
  
  global  ik_leftarmepsilon ik_leftarmsteps;
  
    // import globals
  
    epsilon=ik_leftarmepsilon; // epsilon
    steps=ik_leftarmsteps; // steps
  
    // import parameters
  
  LEFT_SHOULDER_IO=ik_leftarmparameters(1);
  LEFT_SHOULDER_UD=ik_leftarmparameters(2);
  LEFT_BICEP_ROTATE=ik_leftarmparameters(3);
  LEFT_ELBOW=ik_leftarmparameters(4);
  LEFT_FOREARM_ROTATE=ik_leftarmparameters(5);
  LEFTARM_BICEP_ROTATE=ik_leftarmparameters(6);
  LEFTARM_ELBOW=ik_leftarmparameters(7);
  LEFTARM_FOREARM_ROTATE=ik_leftarmparameters(8);
  LEFTARM_SHOULDER_IO=ik_leftarmparameters(9);
  LEFTARM_SHOULDER_UD=ik_leftarmparameters(10);
  nx=ik_leftarmparameters(11);
  ny=ik_leftarmparameters(12);
  px=ik_leftarmparameters(13);
  py=ik_leftarmparameters(14);
  pz=ik_leftarmparameters(15);
  residual=ik_leftarmparameters(16);
  
  // import variables
  
  LEFT_SHOULDER_IO_=ik_leftarmx(1);
  LEFT_SHOULDER_UD_=ik_leftarmx(2);
  LEFT_BICEP_ROTATE_=ik_leftarmx(3);
  LEFT_ELBOW_=ik_leftarmx(4);
  LEFT_FOREARM_ROTATE_=ik_leftarmx(5);
  residual0=(-15)*sin(0.0174532925199433*LEFTARM_ELBOW)*(cos(0.0174532925199433*LEFTARM_BICEP_ROTATE)*sin(0.0174532925199433*LEFTARM_SHOULDER_IO)*sin(0.0174532925199433*LEFTARM_SHOULDER_UD)+cos(0.0174532925199433*LEFTARM_SHOULDER_IO)*sin(0.0174532925199433*LEFTARM_BICEP_ROTATE))+(-1)*px+3.625*sin(0.0174532925199433*LEFTARM_SHOULDER_IO)+9*cos(0.0174532925199433*LEFTARM_SHOULDER_UD)*sin(0.0174532925199433*LEFTARM_SHOULDER_IO)+15*cos(0.0174532925199433*LEFTARM_ELBOW)*cos(0.0174532925199433*LEFTARM_SHOULDER_UD)*sin(0.0174532925199433*LEFTARM_SHOULDER_IO)+(-6);
  residual1=(-15)*sin(0.0174532925199433*LEFTARM_ELBOW)*(cos(0.0174532925199433*LEFTARM_BICEP_ROTATE)*cos(0.0174532925199433*LEFTARM_SHOULDER_IO)*sin(0.0174532925199433*LEFTARM_SHOULDER_UD)+(-1)*sin(0.0174532925199433*LEFTARM_BICEP_ROTATE)*sin(0.0174532925199433*LEFTARM_SHOULDER_IO))+(-1)*py+3.625*cos(0.0174532925199433*LEFTARM_SHOULDER_IO)+9*cos(0.0174532925199433*LEFTARM_SHOULDER_IO)*cos(0.0174532925199433*LEFTARM_SHOULDER_UD)+15*cos(0.0174532925199433*LEFTARM_ELBOW)*cos(0.0174532925199433*LEFTARM_SHOULDER_IO)*cos(0.0174532925199433*LEFTARM_SHOULDER_UD)+2.375;
  residual2=(-15)*cos(0.0174532925199433*LEFTARM_BICEP_ROTATE)*cos(0.0174532925199433*LEFTARM_SHOULDER_UD)*sin(0.0174532925199433*LEFTARM_ELBOW)+(-15)*cos(0.0174532925199433*LEFTARM_ELBOW)*sin(0.0174532925199433*LEFTARM_SHOULDER_UD)+(-9)*sin(0.0174532925199433*LEFTARM_SHOULDER_UD)+(-1)*pz;
  residual3=(-1)*cos(0.0174532925199433*LEFTARM_FOREARM_ROTATE)*(cos(0.0174532925199433*LEFTARM_ELBOW)*(cos(0.0174532925199433*LEFTARM_BICEP_ROTATE)*sin(0.0174532925199433*LEFTARM_SHOULDER_IO)*sin(0.0174532925199433*LEFTARM_SHOULDER_UD)+cos(0.0174532925199433*LEFTARM_SHOULDER_IO)*sin(0.0174532925199433*LEFTARM_BICEP_ROTATE))+cos(0.0174532925199433*LEFTARM_SHOULDER_UD)*sin(0.0174532925199433*LEFTARM_ELBOW)*sin(0.0174532925199433*LEFTARM_SHOULDER_IO))+(-1)*sin(0.0174532925199433*LEFTARM_FOREARM_ROTATE)*(cos(0.0174532925199433*LEFTARM_BICEP_ROTATE)*cos(0.0174532925199433*LEFTARM_SHOULDER_IO)+(-1)*sin(0.0174532925199433*LEFTARM_BICEP_ROTATE)*sin(0.0174532925199433*LEFTARM_SHOULDER_IO)*sin(0.0174532925199433*LEFTARM_SHOULDER_UD))+(-1)*nx;
  residual4=(-1)*cos(0.0174532925199433*LEFTARM_FOREARM_ROTATE)*(cos(0.0174532925199433*LEFTARM_ELBOW)*(cos(0.0174532925199433*LEFTARM_BICEP_ROTATE)*cos(0.0174532925199433*LEFTARM_SHOULDER_IO)*sin(0.0174532925199433*LEFTARM_SHOULDER_UD)+(-1)*sin(0.0174532925199433*LEFTARM_BICEP_ROTATE)*sin(0.0174532925199433*LEFTARM_SHOULDER_IO))+cos(0.0174532925199433*LEFTARM_SHOULDER_IO)*cos(0.0174532925199433*LEFTARM_SHOULDER_UD)*sin(0.0174532925199433*LEFTARM_ELBOW))+(-1)*sin(0.0174532925199433*LEFTARM_FOREARM_ROTATE)*((-1)*cos(0.0174532925199433*LEFTARM_BICEP_ROTATE)*sin(0.0174532925199433*LEFTARM_SHOULDER_IO)+(-1)*cos(0.0174532925199433*LEFTARM_SHOULDER_IO)*sin(0.0174532925199433*LEFTARM_BICEP_ROTATE)*sin(0.0174532925199433*LEFTARM_SHOULDER_UD))+(-1)*ny;
  ik_leftarmy=zeros(5,1);
  ik_leftarmy(1)=residual0;
  ik_leftarmy(2)=residual1;
  ik_leftarmy(3)=residual2;
  ik_leftarmy(4)=residual3;
  ik_leftarmy(5)=residual4;
endfunction

function ik_leftarmdy=ik_leftarmdf(ik_leftarmparameters,ik_leftarmx)
  
  // declare globals
  
  global  ik_leftarmepsilon ik_leftarmsteps;
  
    // import globals
  
    epsilon=ik_leftarmepsilon; // epsilon
    steps=ik_leftarmsteps; // steps
  
    // import parameters
  
  LEFT_SHOULDER_IO=ik_leftarmparameters(1);
  LEFT_SHOULDER_UD=ik_leftarmparameters(2);
  LEFT_BICEP_ROTATE=ik_leftarmparameters(3);
  LEFT_ELBOW=ik_leftarmparameters(4);
  LEFT_FOREARM_ROTATE=ik_leftarmparameters(5);
  LEFTARM_BICEP_ROTATE=ik_leftarmparameters(6);
  LEFTARM_ELBOW=ik_leftarmparameters(7);
  LEFTARM_FOREARM_ROTATE=ik_leftarmparameters(8);
  LEFTARM_SHOULDER_IO=ik_leftarmparameters(9);
  LEFTARM_SHOULDER_UD=ik_leftarmparameters(10);
  nx=ik_leftarmparameters(11);
  ny=ik_leftarmparameters(12);
  px=ik_leftarmparameters(13);
  py=ik_leftarmparameters(14);
  pz=ik_leftarmparameters(15);
  residual=ik_leftarmparameters(16);
  
  // import variables
  
  LEFT_SHOULDER_IO_=ik_leftarmx(1);
  LEFT_SHOULDER_UD_=ik_leftarmx(2);
  LEFT_BICEP_ROTATE_=ik_leftarmx(3);
  LEFT_ELBOW_=ik_leftarmx(4);
  LEFT_FOREARM_ROTATE_=ik_leftarmx(5);
  dresidual0_dLEFT_SHOULDER_IO_=0;
  dresidual0_dLEFT_SHOULDER_UD_=0;
  dresidual0_dLEFT_BICEP_ROTATE_=0;
  dresidual0_dLEFT_ELBOW_=0;
  dresidual0_dLEFT_FOREARM_ROTATE_=0;
  dresidual1_dLEFT_SHOULDER_IO_=0;
  dresidual1_dLEFT_SHOULDER_UD_=0;
  dresidual1_dLEFT_BICEP_ROTATE_=0;
  dresidual1_dLEFT_ELBOW_=0;
  dresidual1_dLEFT_FOREARM_ROTATE_=0;
  dresidual2_dLEFT_SHOULDER_IO_=0;
  dresidual2_dLEFT_SHOULDER_UD_=0;
  dresidual2_dLEFT_BICEP_ROTATE_=0;
  dresidual2_dLEFT_ELBOW_=0;
  dresidual2_dLEFT_FOREARM_ROTATE_=0;
  dresidual3_dLEFT_SHOULDER_IO_=0;
  dresidual3_dLEFT_SHOULDER_UD_=0;
  dresidual3_dLEFT_BICEP_ROTATE_=0;
  dresidual3_dLEFT_ELBOW_=0;
  dresidual3_dLEFT_FOREARM_ROTATE_=0;
  dresidual4_dLEFT_SHOULDER_IO_=0;
  dresidual4_dLEFT_SHOULDER_UD_=0;
  dresidual4_dLEFT_BICEP_ROTATE_=0;
  dresidual4_dLEFT_ELBOW_=0;
  dresidual4_dLEFT_FOREARM_ROTATE_=0;
  // place derivatives in dy
  ik_leftarmdy=zeros(5,5);
  ik_leftarmdy(1,1)=dresidual0_dLEFT_SHOULDER_IO_;
  ik_leftarmdy(1,2)=dresidual0_dLEFT_SHOULDER_UD_;
  ik_leftarmdy(1,3)=dresidual0_dLEFT_BICEP_ROTATE_;
  ik_leftarmdy(1,4)=dresidual0_dLEFT_ELBOW_;
  ik_leftarmdy(1,5)=dresidual0_dLEFT_FOREARM_ROTATE_;
  ik_leftarmdy(2,1)=dresidual1_dLEFT_SHOULDER_IO_;
  ik_leftarmdy(2,2)=dresidual1_dLEFT_SHOULDER_UD_;
  ik_leftarmdy(2,3)=dresidual1_dLEFT_BICEP_ROTATE_;
  ik_leftarmdy(2,4)=dresidual1_dLEFT_ELBOW_;
  ik_leftarmdy(2,5)=dresidual1_dLEFT_FOREARM_ROTATE_;
  ik_leftarmdy(3,1)=dresidual2_dLEFT_SHOULDER_IO_;
  ik_leftarmdy(3,2)=dresidual2_dLEFT_SHOULDER_UD_;
  ik_leftarmdy(3,3)=dresidual2_dLEFT_BICEP_ROTATE_;
  ik_leftarmdy(3,4)=dresidual2_dLEFT_ELBOW_;
  ik_leftarmdy(3,5)=dresidual2_dLEFT_FOREARM_ROTATE_;
  ik_leftarmdy(4,1)=dresidual3_dLEFT_SHOULDER_IO_;
  ik_leftarmdy(4,2)=dresidual3_dLEFT_SHOULDER_UD_;
  ik_leftarmdy(4,3)=dresidual3_dLEFT_BICEP_ROTATE_;
  ik_leftarmdy(4,4)=dresidual3_dLEFT_ELBOW_;
  ik_leftarmdy(4,5)=dresidual3_dLEFT_FOREARM_ROTATE_;
  ik_leftarmdy(5,1)=dresidual4_dLEFT_SHOULDER_IO_;
  ik_leftarmdy(5,2)=dresidual4_dLEFT_SHOULDER_UD_;
  ik_leftarmdy(5,3)=dresidual4_dLEFT_BICEP_ROTATE_;
  ik_leftarmdy(5,4)=dresidual4_dLEFT_ELBOW_;
  ik_leftarmdy(5,5)=dresidual4_dLEFT_FOREARM_ROTATE_;
endfunction

function ik_leftarmx=ik_leftarmsolve(ik_leftarmparameters)
  
  // declare globals
  
  global  ik_leftarmepsilon ik_leftarmsteps;
  
    // import globals
  
    epsilon=ik_leftarmepsilon; // epsilon
    steps=ik_leftarmsteps; // steps
  
    // import parameters
  
  LEFT_SHOULDER_IO=ik_leftarmparameters(1);
  LEFT_SHOULDER_UD=ik_leftarmparameters(2);
  LEFT_BICEP_ROTATE=ik_leftarmparameters(3);
  LEFT_ELBOW=ik_leftarmparameters(4);
  LEFT_FOREARM_ROTATE=ik_leftarmparameters(5);
  LEFTARM_BICEP_ROTATE=ik_leftarmparameters(6);
  LEFTARM_ELBOW=ik_leftarmparameters(7);
  LEFTARM_FOREARM_ROTATE=ik_leftarmparameters(8);
  LEFTARM_SHOULDER_IO=ik_leftarmparameters(9);
  LEFTARM_SHOULDER_UD=ik_leftarmparameters(10);
  nx=ik_leftarmparameters(11);
  ny=ik_leftarmparameters(12);
  px=ik_leftarmparameters(13);
  py=ik_leftarmparameters(14);
  pz=ik_leftarmparameters(15);
  residual=ik_leftarmparameters(16);
  ik_leftarmresidual=0;
  
  // initialize unknowns from parameters
  
  ik_leftarmx=zeros(5,1);
  LEFT_SHOULDER_IO_=LEFT_SHOULDER_IO;
  LEFT_SHOULDER_UD_=LEFT_SHOULDER_UD;
  LEFT_BICEP_ROTATE_=LEFT_BICEP_ROTATE;
  LEFT_ELBOW_=LEFT_ELBOW;
  LEFT_FOREARM_ROTATE_=LEFT_FOREARM_ROTATE;
  
  // setup unknown vector x
  
  ik_leftarmx=zeros(5,1);
  ik_leftarmx(1)=LEFT_SHOULDER_IO_;
  ik_leftarmx(2)=LEFT_SHOULDER_UD_;
  ik_leftarmx(3)=LEFT_BICEP_ROTATE_;
  ik_leftarmx(4)=LEFT_ELBOW_;
  ik_leftarmx(5)=LEFT_FOREARM_ROTATE_;
  
  // newton iteration
  for step=1:steps
    ik_leftarmy=ik_leftarmf(ik_leftarmparameters,ik_leftarmx);
    ik_leftarmdy=ik_leftarmdf(ik_leftarmparameters,ik_leftarmx);
    ik_leftarmx=ik_leftarmx-ik_leftarmdy\ik_leftarmy;
    ik_leftarmresidual=norm(ik_leftarmy);
    if (ik_leftarmresidual <= epsilon) then
      break;
    end
  end
  ik_leftarmparameters(16)=ik_leftarmresidual;
endfunction

function ik_leftarmnew_parameters=ik_leftarmupdate(ik_leftarmparameters)
  
  // declare globals
  
  global  ik_leftarmepsilon ik_leftarmsteps;
  
    // import globals
  
    epsilon=ik_leftarmepsilon; // epsilon
    steps=ik_leftarmsteps; // steps
  
    // import parameters
  
  LEFT_SHOULDER_IO=ik_leftarmparameters(1);
  LEFT_SHOULDER_UD=ik_leftarmparameters(2);
  LEFT_BICEP_ROTATE=ik_leftarmparameters(3);
  LEFT_ELBOW=ik_leftarmparameters(4);
  LEFT_FOREARM_ROTATE=ik_leftarmparameters(5);
  LEFTARM_BICEP_ROTATE=ik_leftarmparameters(6);
  LEFTARM_ELBOW=ik_leftarmparameters(7);
  LEFTARM_FOREARM_ROTATE=ik_leftarmparameters(8);
  LEFTARM_SHOULDER_IO=ik_leftarmparameters(9);
  LEFTARM_SHOULDER_UD=ik_leftarmparameters(10);
  nx=ik_leftarmparameters(11);
  ny=ik_leftarmparameters(12);
  px=ik_leftarmparameters(13);
  py=ik_leftarmparameters(14);
  pz=ik_leftarmparameters(15);
  residual=ik_leftarmparameters(16);
  ik_leftarmx=ik_leftarmsolve(ik_leftarmparameters);
  LEFT_SHOULDER_IO=LEFT_SHOULDER_IO_;
  LEFT_SHOULDER_UD=LEFT_SHOULDER_UD_;
  LEFT_BICEP_ROTATE=LEFT_BICEP_ROTATE_;
  LEFT_ELBOW=LEFT_ELBOW_;
  LEFT_FOREARM_ROTATE=LEFT_FOREARM_ROTATE_;
  
  // assign to new_parameters
  
  ik_leftarmnew_parameters=zeros(5,1);
  ik_leftarmnew_parameters(1)=LEFT_SHOULDER_IO;
  ik_leftarmnew_parameters(2)=LEFT_SHOULDER_UD;
  ik_leftarmnew_parameters(3)=LEFT_BICEP_ROTATE;
  ik_leftarmnew_parameters(4)=LEFT_ELBOW;
  ik_leftarmnew_parameters(5)=LEFT_FOREARM_ROTATE;
  ik_leftarmnew_parameters(6)=LEFTARM_BICEP_ROTATE;
  ik_leftarmnew_parameters(7)=LEFTARM_ELBOW;
  ik_leftarmnew_parameters(8)=LEFTARM_FOREARM_ROTATE;
  ik_leftarmnew_parameters(9)=LEFTARM_SHOULDER_IO;
  ik_leftarmnew_parameters(10)=LEFTARM_SHOULDER_UD;
  ik_leftarmnew_parameters(11)=nx;
  ik_leftarmnew_parameters(12)=ny;
  ik_leftarmnew_parameters(13)=px;
  ik_leftarmnew_parameters(14)=py;
  ik_leftarmnew_parameters(15)=pz;
  ik_leftarmnew_parameters(16)=residual;
endfunction

function ik_leftarmtests(ik_leftarmname)
  
  // declare globals
  
  global  ik_leftarmepsilon ik_leftarmsteps;
  ik_leftarmcase_name='';
  ik_leftarmcase_count=1;
  ik_leftarmmax_x_error_name="none";
  ik_leftarmmax_x_error_count=0;
  ik_leftarmmax_x_error=0;
  ik_leftarmx_error=0;
  ik_leftarmmax_y_error_name="none";
  ik_leftarmmax_y_error_count=0;
  ik_leftarmmax_y_error=0;
  ik_leftarmy_error=0;
  if (ik_leftarmname == "all") then
    printf("test summary: max_x_error=%1.15g from %d/""%s"", max_y_error=%1.15g from %d/""%s""\n",ik_leftarmmax_x_error,ik_leftarmmax_x_error_count,ik_leftarmmax_x_error_name,ik_leftarmmax_y_error,ik_leftarmmax_y_error_count,ik_leftarmmax_y_error_name);
  end
endfunction
function ik_leftarmruns(name)
  
  // declare globals
  
  global  ik_leftarmepsilon ik_leftarmsteps;
  ik_leftarmcase_count=1;
endfunction
