
// declare globals

global  ik_lidarepsilon ik_lidarsteps;

// define function

function ik_lidary=ik_lidarf(ik_lidarparameters,ik_lidarx)
  
  // declare globals
  
  global  ik_lidarepsilon ik_lidarsteps;
  
    // import globals
  
    epsilon=ik_lidarepsilon; // epsilon
    steps=ik_lidarsteps; // steps
  
    // import parameters
  
  necklr=ik_lidarparameters(1);
  neckud=ik_lidarparameters(2);
  px=ik_lidarparameters(3);
  py=ik_lidarparameters(4);
  pz=ik_lidarparameters(5);
  waist=ik_lidarparameters(6);
  residual=ik_lidarparameters(7);
  
  // import variables
  
  _necklr=ik_lidarx(1);
  _neckud=ik_lidarx(2);
  residual0=cos(0.0174532925199433*_necklr)*px+sin((-0.0174532925199433)*_neckud)*sin(0.0174532925199433*_necklr)*pz+(-1)*cos((-0.0174532925199433)*_neckud)*sin(0.0174532925199433*_necklr)*py;
  residual1=sin((-0.0174532925199433)*waist)*sin(0.0174532925199433*_necklr)*px+(cos((-0.0174532925199433)*_neckud)*cos((-0.0174532925199433)*waist)+(-1)*cos(0.0174532925199433*_necklr)*sin((-0.0174532925199433)*_neckud)*sin((-0.0174532925199433)*waist))*pz+(cos((-0.0174532925199433)*_neckud)*cos(0.0174532925199433*_necklr)*sin((-0.0174532925199433)*waist)+cos((-0.0174532925199433)*waist)*sin((-0.0174532925199433)*_neckud))*py;
  ik_lidary=zeros(2,1);
  ik_lidary(1)=residual0;
  ik_lidary(2)=residual1;
endfunction

function ik_lidardy=ik_lidardf(ik_lidarparameters,ik_lidarx)
  
  // declare globals
  
  global  ik_lidarepsilon ik_lidarsteps;
  
    // import globals
  
    epsilon=ik_lidarepsilon; // epsilon
    steps=ik_lidarsteps; // steps
  
    // import parameters
  
  necklr=ik_lidarparameters(1);
  neckud=ik_lidarparameters(2);
  px=ik_lidarparameters(3);
  py=ik_lidarparameters(4);
  pz=ik_lidarparameters(5);
  waist=ik_lidarparameters(6);
  residual=ik_lidarparameters(7);
  
  // import variables
  
  _necklr=ik_lidarx(1);
  _neckud=ik_lidarx(2);
  dresidual0_d_necklr=(-0.0174532925199433)*cos((-0.0174532925199433)*_neckud)*cos(0.0174532925199433*_necklr)*py+(-0.0174532925199433)*sin(0.0174532925199433*_necklr)*px+0.0174532925199433*cos(0.0174532925199433*_necklr)*sin((-0.0174532925199433)*_neckud)*pz;
  dresidual0_d_neckud=(-0.0174532925199433)*cos((-0.0174532925199433)*_neckud)*sin(0.0174532925199433*_necklr)*pz+(-0.0174532925199433)*sin((-0.0174532925199433)*_neckud)*sin(0.0174532925199433*_necklr)*py;
  dresidual1_d_necklr=(-0.0174532925199433)*cos((-0.0174532925199433)*_neckud)*sin((-0.0174532925199433)*waist)*sin(0.0174532925199433*_necklr)*py+0.0174532925199433*cos(0.0174532925199433*_necklr)*sin((-0.0174532925199433)*waist)*px+0.0174532925199433*sin((-0.0174532925199433)*_neckud)*sin((-0.0174532925199433)*waist)*sin(0.0174532925199433*_necklr)*pz;
  dresidual1_d_neckud=((-0.0174532925199433)*cos((-0.0174532925199433)*_neckud)*cos((-0.0174532925199433)*waist)+0.0174532925199433*cos(0.0174532925199433*_necklr)*sin((-0.0174532925199433)*_neckud)*sin((-0.0174532925199433)*waist))*py+(0.0174532925199433*cos((-0.0174532925199433)*_neckud)*cos(0.0174532925199433*_necklr)*sin((-0.0174532925199433)*waist)+0.0174532925199433*cos((-0.0174532925199433)*waist)*sin((-0.0174532925199433)*_neckud))*pz;
  // place derivatives in dy
  ik_lidardy=zeros(2,2);
  ik_lidardy(1,1)=dresidual0_d_necklr;
  ik_lidardy(1,2)=dresidual0_d_neckud;
  ik_lidardy(2,1)=dresidual1_d_necklr;
  ik_lidardy(2,2)=dresidual1_d_neckud;
endfunction

function ik_lidarx=ik_lidarsolve(ik_lidarparameters)
  
  // declare globals
  
  global  ik_lidarepsilon ik_lidarsteps;
  
    // import globals
  
    epsilon=ik_lidarepsilon; // epsilon
    steps=ik_lidarsteps; // steps
  
    // import parameters
  
  necklr=ik_lidarparameters(1);
  neckud=ik_lidarparameters(2);
  px=ik_lidarparameters(3);
  py=ik_lidarparameters(4);
  pz=ik_lidarparameters(5);
  waist=ik_lidarparameters(6);
  residual=ik_lidarparameters(7);
  ik_lidarresidual=0;
  
  // initialize unknowns from parameters
  
  ik_lidarx=zeros(2,1);
  _necklr=necklr;
  _neckud=neckud;
  
  // setup unknown vector x
  
  ik_lidarx=zeros(2,1);
  ik_lidarx(1)=_necklr;
  ik_lidarx(2)=_neckud;
  
  // newton iteration
  for step=1:steps
    ik_lidary=ik_lidarf(ik_lidarparameters,ik_lidarx);
    ik_lidardy=ik_lidardf(ik_lidarparameters,ik_lidarx);
    ik_lidarx=ik_lidarx-ik_lidardy\ik_lidary;
    ik_lidarresidual=norm(ik_lidary);
    if (ik_lidarresidual <= epsilon) then
      break;
    end
  end
  ik_lidarparameters(7)=ik_lidarresidual;
endfunction

function ik_lidarnew_parameters=ik_lidarupdate(ik_lidarparameters)
  
  // declare globals
  
  global  ik_lidarepsilon ik_lidarsteps;
  
    // import globals
  
    epsilon=ik_lidarepsilon; // epsilon
    steps=ik_lidarsteps; // steps
  
    // import parameters
  
  necklr=ik_lidarparameters(1);
  neckud=ik_lidarparameters(2);
  px=ik_lidarparameters(3);
  py=ik_lidarparameters(4);
  pz=ik_lidarparameters(5);
  waist=ik_lidarparameters(6);
  residual=ik_lidarparameters(7);
  ik_lidarx=ik_lidarsolve(ik_lidarparameters);
  
  // import variables
  
  _necklr=ik_lidarx(1);
  _neckud=ik_lidarx(2);
  necklr=_necklr;
  neckud=_neckud;
  
  // assign to new_parameters
  
  ik_lidarnew_parameters=zeros(7,1);
  ik_lidarnew_parameters(1)=necklr;
  ik_lidarnew_parameters(2)=neckud;
  ik_lidarnew_parameters(3)=px;
  ik_lidarnew_parameters(4)=py;
  ik_lidarnew_parameters(5)=pz;
  ik_lidarnew_parameters(6)=waist;
  ik_lidarnew_parameters(7)=residual;
endfunction

function ik_lidartests(ik_lidarname)
  
  // declare globals
  
  global  ik_lidarepsilon ik_lidarsteps;
  ik_lidarcase_name='';
  ik_lidarcase_count=1;
  ik_lidarmax_x_error_name="none";
  ik_lidarmax_x_error_count=0;
  ik_lidarmax_x_error=0;
  ik_lidarx_error=0;
  ik_lidarmax_y_error_name="none";
  ik_lidarmax_y_error_count=0;
  ik_lidarmax_y_error=0;
  ik_lidary_error=0;
  if (ik_lidarname == "all") then
    printf("test summary: max_x_error=%1.15g from %d/""%s"", max_y_error=%1.15g from %d/""%s""\n",ik_lidarmax_x_error,ik_lidarmax_x_error_count,ik_lidarmax_x_error_name,ik_lidarmax_y_error,ik_lidarmax_y_error_count,ik_lidarmax_y_error_name);
  end
endfunction
function ik_lidarruns(name)
  
  // declare globals
  
  global  ik_lidarepsilon ik_lidarsteps;
  ik_lidarcase_count=1;
endfunction
