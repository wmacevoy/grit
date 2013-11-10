
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
  
  shoulderio=ik_rightarmparameters(1);
  shoulderud=ik_rightarmparameters(2);
  bicep=ik_rightarmparameters(3);
  elbow=ik_rightarmparameters(4);
  forearm=ik_rightarmparameters(5);
  rightpx=ik_rightarmparameters(6);
  rightpy=ik_rightarmparameters(7);
  rightpz=ik_rightarmparameters(8);
  rightroll=ik_rightarmparameters(9);
  rightyaw=ik_rightarmparameters(10);
  residual=ik_rightarmparameters(11);
  
  // import variables
  
  _shoulderio=ik_rightarmx(1);
  _shoulderud=ik_rightarmx(2);
  _bicep=ik_rightarmx(3);
  _elbow=ik_rightarmx(4);
  _forearm=ik_rightarmx(5);
  residual0=(-1)*rightpx+3.625*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+9*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+15*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+15*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+(-1)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+6;
  residual1=(-1)*rightpy+3.625*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))+9*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))+15*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))+15*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-1)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-1)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep))+2.375;
  residual2=(-1)*rightpz+9*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+15*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+15*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_elbow+0.7853981633974481);
  residual3=cos(0.0174532925199433*_forearm+0.5235987755982989)*(cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+(-1)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+(-1)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481)))+sin(0.0174532925199433*_forearm+0.5235987755982989)*(cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)+sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep))+(-1)*sin(0.0174532925199433*rightroll);
  residual4=cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+(-1)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+(-1)*sin(0.0174532925199433*rightyaw);
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
  
  shoulderio=ik_rightarmparameters(1);
  shoulderud=ik_rightarmparameters(2);
  bicep=ik_rightarmparameters(3);
  elbow=ik_rightarmparameters(4);
  forearm=ik_rightarmparameters(5);
  rightpx=ik_rightarmparameters(6);
  rightpy=ik_rightarmparameters(7);
  rightpz=ik_rightarmparameters(8);
  rightroll=ik_rightarmparameters(9);
  rightyaw=ik_rightarmparameters(10);
  residual=ik_rightarmparameters(11);
  
  // import variables
  
  _shoulderio=ik_rightarmx(1);
  _shoulderud=ik_rightarmx(2);
  _bicep=ik_rightarmx(3);
  _elbow=ik_rightarmx(4);
  _forearm=ik_rightarmx(5);
  dresidual0_d_shoulderio=0.06326818538479446*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))+0.1570796326794897*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))+0.2617993877991495*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))+15*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-0.0174532925199433)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep));
  dresidual0_d_shoulderud=(-0.2617993877991495)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-0.2617993877991495)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+(-0.1570796326794897)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481));
  dresidual0_d_bicep=15*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(0.0174532925199433*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)+0.0174532925199433*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep));
  dresidual0_d_elbow=(-0.2617993877991495)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+(-1)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+0.2617993877991495*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481));
  dresidual0_d_forearm=0;
  dresidual1_d_shoulderio=(-0.2617993877991495)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+(-0.1570796326794897)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+(-0.06326818538479446)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+15*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+0.0174532925199433*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)));
  dresidual1_d_shoulderud=(-0.2617993877991495)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-0.2617993877991495)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_elbow+0.7853981633974481)+(-0.1570796326794897)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481));
  dresidual1_d_bicep=15*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+0.0174532925199433*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep));
  dresidual1_d_elbow=(-0.2617993877991495)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*((-1)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-1)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep))+0.2617993877991495*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481);
  dresidual1_d_forearm=0;
  dresidual2_d_shoulderio=0;
  dresidual2_d_shoulderud=(-0.2617993877991495)*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+0.1570796326794897*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))+0.2617993877991495*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481));
  dresidual2_d_bicep=(-0.2617993877991495)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_bicep);
  dresidual2_d_elbow=(-0.2617993877991495)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)+0.2617993877991495*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481));
  dresidual2_d_forearm=0;
  dresidual3_d_shoulderio=cos(0.0174532925199433*_forearm+0.5235987755982989)*(cos((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-0.0174532925199433)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep))+(-0.0174532925199433)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481))+sin(0.0174532925199433*_forearm+0.5235987755982989)*((-0.0174532925199433)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+0.0174532925199433*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep));
  dresidual3_d_shoulderud=cos(0.0174532925199433*_forearm+0.5235987755982989)*((-0.0174532925199433)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+0.0174532925199433*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+0.0174532925199433*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_forearm+0.5235987755982989)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep);
  dresidual3_d_bicep=cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_forearm+0.5235987755982989)*(0.0174532925199433*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)+0.0174532925199433*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep))+sin(0.0174532925199433*_forearm+0.5235987755982989)*((-0.0174532925199433)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+0.0174532925199433*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)));
  dresidual3_d_elbow=cos(0.0174532925199433*_forearm+0.5235987755982989)*(0.0174532925199433*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))+0.0174532925199433*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+(-1)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))));
  dresidual3_d_forearm=(-0.0174532925199433)*sin(0.0174532925199433*_forearm+0.5235987755982989)*(cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+(-1)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+(-1)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481)))+0.0174532925199433*cos(0.0174532925199433*_forearm+0.5235987755982989)*(cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)+sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep));
  dresidual4_d_shoulderio=sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-0.0174532925199433)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep))+0.0174532925199433*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481));
  dresidual4_d_shoulderud=(-0.0174532925199433)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-0.0174532925199433)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481));
  dresidual4_d_bicep=sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(0.0174532925199433*cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)+0.0174532925199433*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep));
  dresidual4_d_elbow=(-0.0174532925199433)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+(-1)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481))*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+0.0174532925199433*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin(0.0174532925199433*_shoulderio+(-0.7853981633974481));
  dresidual4_d_forearm=0;
  // place derivatives in dy
  ik_rightarmdy=zeros(5,5);
  ik_rightarmdy(1,1)=dresidual0_d_shoulderio;
  ik_rightarmdy(1,2)=dresidual0_d_shoulderud;
  ik_rightarmdy(1,3)=dresidual0_d_bicep;
  ik_rightarmdy(1,4)=dresidual0_d_elbow;
  ik_rightarmdy(1,5)=dresidual0_d_forearm;
  ik_rightarmdy(2,1)=dresidual1_d_shoulderio;
  ik_rightarmdy(2,2)=dresidual1_d_shoulderud;
  ik_rightarmdy(2,3)=dresidual1_d_bicep;
  ik_rightarmdy(2,4)=dresidual1_d_elbow;
  ik_rightarmdy(2,5)=dresidual1_d_forearm;
  ik_rightarmdy(3,1)=dresidual2_d_shoulderio;
  ik_rightarmdy(3,2)=dresidual2_d_shoulderud;
  ik_rightarmdy(3,3)=dresidual2_d_bicep;
  ik_rightarmdy(3,4)=dresidual2_d_elbow;
  ik_rightarmdy(3,5)=dresidual2_d_forearm;
  ik_rightarmdy(4,1)=dresidual3_d_shoulderio;
  ik_rightarmdy(4,2)=dresidual3_d_shoulderud;
  ik_rightarmdy(4,3)=dresidual3_d_bicep;
  ik_rightarmdy(4,4)=dresidual3_d_elbow;
  ik_rightarmdy(4,5)=dresidual3_d_forearm;
  ik_rightarmdy(5,1)=dresidual4_d_shoulderio;
  ik_rightarmdy(5,2)=dresidual4_d_shoulderud;
  ik_rightarmdy(5,3)=dresidual4_d_bicep;
  ik_rightarmdy(5,4)=dresidual4_d_elbow;
  ik_rightarmdy(5,5)=dresidual4_d_forearm;
endfunction

function ik_rightarmx=ik_rightarmsolve(ik_rightarmparameters)
  
  // declare globals
  
  global  ik_rightarmepsilon ik_rightarmsteps;
  
    // import globals
  
    epsilon=ik_rightarmepsilon; // epsilon
    steps=ik_rightarmsteps; // steps
  
    // import parameters
  
  shoulderio=ik_rightarmparameters(1);
  shoulderud=ik_rightarmparameters(2);
  bicep=ik_rightarmparameters(3);
  elbow=ik_rightarmparameters(4);
  forearm=ik_rightarmparameters(5);
  rightpx=ik_rightarmparameters(6);
  rightpy=ik_rightarmparameters(7);
  rightpz=ik_rightarmparameters(8);
  rightroll=ik_rightarmparameters(9);
  rightyaw=ik_rightarmparameters(10);
  residual=ik_rightarmparameters(11);
  ik_rightarmresidual=0;
  
  // initialize unknowns from parameters
  
  ik_rightarmx=zeros(5,1);
  _shoulderio=shoulderio;
  _shoulderud=shoulderud;
  _bicep=bicep;
  _elbow=elbow;
  _forearm=forearm;
  
  // setup unknown vector x
  
  ik_rightarmx=zeros(5,1);
  ik_rightarmx(1)=_shoulderio;
  ik_rightarmx(2)=_shoulderud;
  ik_rightarmx(3)=_bicep;
  ik_rightarmx(4)=_elbow;
  ik_rightarmx(5)=_forearm;
  
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
  ik_rightarmparameters(11)=ik_rightarmresidual;
endfunction

function ik_rightarmnew_parameters=ik_rightarmupdate(ik_rightarmparameters)
  
  // declare globals
  
  global  ik_rightarmepsilon ik_rightarmsteps;
  
    // import globals
  
    epsilon=ik_rightarmepsilon; // epsilon
    steps=ik_rightarmsteps; // steps
  
    // import parameters
  
  shoulderio=ik_rightarmparameters(1);
  shoulderud=ik_rightarmparameters(2);
  bicep=ik_rightarmparameters(3);
  elbow=ik_rightarmparameters(4);
  forearm=ik_rightarmparameters(5);
  rightpx=ik_rightarmparameters(6);
  rightpy=ik_rightarmparameters(7);
  rightpz=ik_rightarmparameters(8);
  rightroll=ik_rightarmparameters(9);
  rightyaw=ik_rightarmparameters(10);
  residual=ik_rightarmparameters(11);
  ik_rightarmx=ik_rightarmsolve(ik_rightarmparameters);
  
  // import variables
  
  _shoulderio=ik_rightarmx(1);
  _shoulderud=ik_rightarmx(2);
  _bicep=ik_rightarmx(3);
  _elbow=ik_rightarmx(4);
  _forearm=ik_rightarmx(5);
  shoulderio=_shoulderio;
  shoulderud=_shoulderud;
  bicep=_bicep;
  elbow=_elbow;
  forearm=_forearm;
  
  // assign to new_parameters
  
  ik_rightarmnew_parameters=zeros(11,1);
  ik_rightarmnew_parameters(1)=shoulderio;
  ik_rightarmnew_parameters(2)=shoulderud;
  ik_rightarmnew_parameters(3)=bicep;
  ik_rightarmnew_parameters(4)=elbow;
  ik_rightarmnew_parameters(5)=forearm;
  ik_rightarmnew_parameters(6)=rightpx;
  ik_rightarmnew_parameters(7)=rightpy;
  ik_rightarmnew_parameters(8)=rightpz;
  ik_rightarmnew_parameters(9)=rightroll;
  ik_rightarmnew_parameters(10)=rightyaw;
  ik_rightarmnew_parameters(11)=residual;
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
