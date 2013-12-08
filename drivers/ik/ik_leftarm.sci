
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
  
  shoulderio=ik_leftarmparameters(1);
  shoulderud=ik_leftarmparameters(2);
  bicep=ik_leftarmparameters(3);
  elbow=ik_leftarmparameters(4);
  forearm=ik_leftarmparameters(5);
  downx=ik_leftarmparameters(6);
  px=ik_leftarmparameters(7);
  py=ik_leftarmparameters(8);
  pz=ik_leftarmparameters(9);
  waist=ik_leftarmparameters(10);
  residual=ik_leftarmparameters(11);
  
  // import variables
  
  _shoulderio=ik_leftarmx(1);
  _shoulderud=ik_leftarmx(2);
  _bicep=ik_leftarmx(3);
  _elbow=ik_leftarmx(4);
  _forearm=ik_leftarmx(5);
  residual0=(-14)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)+(-9)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)+(-3.625)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)+(-1)*px+14*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_bicep)+cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+(-4.875);
  residual1=(-13.5)*sin((-0.0174532925199433)*waist)+(-9)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist)+(-1)*py+2.375*cos((-0.0174532925199433)*waist)+3.625*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos((-0.0174532925199433)*waist)+9*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist)+14*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist)+(-1)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist))+14*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*waist)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_bicep)+cos(0.0174532925199433*_bicep)*((-1)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos((-0.0174532925199433)*waist)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-1)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist)));
  residual2=(-1)*pz+2.375*sin((-0.0174532925199433)*waist)+3.625*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin((-0.0174532925199433)*waist)+9*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist)+9*cos((-0.0174532925199433)*waist)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+13.5*cos((-0.0174532925199433)*waist)+14*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist)+cos((-0.0174532925199433)*waist)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+14*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos(0.0174532925199433*_bicep)*(cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist)+(-1)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist))+sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin((-0.0174532925199433)*waist)*sin(0.0174532925199433*_bicep))+12;
  residual3=cos(0.0174532925199433*_forearm+(-1.13446401379631))*(cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_bicep)+cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481))+sin(0.0174532925199433*_forearm+(-1.13446401379631))*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_bicep)+(-1)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep))+downx;
  ik_leftarmy=zeros(4,1);
  ik_leftarmy(1)=residual0;
  ik_leftarmy(2)=residual1;
  ik_leftarmy(3)=residual2;
  ik_leftarmy(4)=residual3;
endfunction

function ik_leftarmdy=ik_leftarmdf(ik_leftarmparameters,ik_leftarmx)
  
  // declare globals
  
  global  ik_leftarmepsilon ik_leftarmsteps;
  
    // import globals
  
    epsilon=ik_leftarmepsilon; // epsilon
    steps=ik_leftarmsteps; // steps
  
    // import parameters
  
  shoulderio=ik_leftarmparameters(1);
  shoulderud=ik_leftarmparameters(2);
  bicep=ik_leftarmparameters(3);
  elbow=ik_leftarmparameters(4);
  forearm=ik_leftarmparameters(5);
  downx=ik_leftarmparameters(6);
  px=ik_leftarmparameters(7);
  py=ik_leftarmparameters(8);
  pz=ik_leftarmparameters(9);
  waist=ik_leftarmparameters(10);
  residual=ik_leftarmparameters(11);
  
  // import variables
  
  _shoulderio=ik_leftarmx(1);
  _shoulderud=ik_leftarmx(2);
  _bicep=ik_leftarmx(3);
  _elbow=ik_leftarmx(4);
  _forearm=ik_leftarmx(5);
  dresidual0_d_shoulderio=0.06326818538479446*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)+0.1570796326794897*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))+0.2443460952792062*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))+14*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+0.0174532925199433*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_bicep));
  dresidual0_d_shoulderud=0.1570796326794897*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+0.2443460952792062*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+0.2443460952792062*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481);
  dresidual0_d_bicep=14*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+0.0174532925199433*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_bicep));
  dresidual0_d_elbow=(-0.2443460952792062)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_bicep)+cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+(-0.2443460952792062)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481);
  dresidual0_d_forearm=0;
  dresidual1_d_shoulderio=0.06326818538479446*cos((-0.0174532925199433)*waist)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)+0.1570796326794897*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)+0.2443460952792062*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)+14*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos((-0.0174532925199433)*waist)*sin(0.0174532925199433*_bicep)+(-0.0174532925199433)*cos((-0.0174532925199433)*waist)*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)));
  dresidual1_d_shoulderud=(-0.1570796326794897)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos((-0.0174532925199433)*waist)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-0.1570796326794897)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist)+14*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos((-0.0174532925199433)*waist)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-0.0174532925199433)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist))+14*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist)+0.0174532925199433*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist));
  dresidual1_d_bicep=14*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*sin(0.0174532925199433*_bicep)*((-1)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos((-0.0174532925199433)*waist)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-1)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist))+0.0174532925199433*cos((-0.0174532925199433)*waist)*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481));
  dresidual1_d_elbow=(-0.2443460952792062)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*waist)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_bicep)+cos(0.0174532925199433*_bicep)*((-1)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos((-0.0174532925199433)*waist)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+(-1)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist)))+0.2443460952792062*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist)+(-1)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist));
  dresidual1_d_forearm=0;
  dresidual2_d_shoulderio=0.06326818538479446*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin((-0.0174532925199433)*waist)+0.1570796326794897*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin((-0.0174532925199433)*waist)+0.2443460952792062*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin((-0.0174532925199433)*waist)+14*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin((-0.0174532925199433)*waist)*sin(0.0174532925199433*_bicep)+(-0.0174532925199433)*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist));
  dresidual2_d_shoulderud=(-0.1570796326794897)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist)+0.1570796326794897*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist)+14*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist)+0.0174532925199433*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist))+14*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist)+(-0.0174532925199433)*cos((-0.0174532925199433)*waist)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)));
  dresidual2_d_bicep=14*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*sin(0.0174532925199433*_bicep)*(cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist)+(-1)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist))+0.0174532925199433*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin((-0.0174532925199433)*waist));
  dresidual2_d_elbow=(-0.2443460952792062)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos(0.0174532925199433*_bicep)*(cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos((-0.0174532925199433)*waist)+(-1)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist))+sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin((-0.0174532925199433)*waist)*sin(0.0174532925199433*_bicep))+0.2443460952792062*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*waist)+cos((-0.0174532925199433)*waist)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)));
  dresidual2_d_forearm=0;
  dresidual3_d_shoulderio=cos(0.0174532925199433*_forearm+(-1.13446401379631))*(cos((-0.0174532925199433)*_elbow+0.7853981633974481)*((-0.0174532925199433)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_bicep)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+0.0174532925199433*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_bicep))+(-0.0174532925199433)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481))+sin(0.0174532925199433*_forearm+(-1.13446401379631))*(0.0174532925199433*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+0.0174532925199433*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481));
  dresidual3_d_shoulderud=cos(0.0174532925199433*_forearm+(-1.13446401379631))*((-0.0174532925199433)*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))+0.0174532925199433*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481))+(-0.0174532925199433)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_forearm+(-1.13446401379631))*sin(0.0174532925199433*_bicep);
  dresidual3_d_bicep=cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_forearm+(-1.13446401379631))*((-0.0174532925199433)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep)+0.0174532925199433*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_bicep))+sin(0.0174532925199433*_forearm+(-1.13446401379631))*((-0.0174532925199433)*cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_bicep)+(-0.0174532925199433)*cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)));
  dresidual3_d_elbow=cos(0.0174532925199433*_forearm+(-1.13446401379631))*((-0.0174532925199433)*cos((-0.0174532925199433)*_elbow+0.7853981633974481)*cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)+0.0174532925199433*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_bicep)+cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))));
  dresidual3_d_forearm=(-0.0174532925199433)*sin(0.0174532925199433*_forearm+(-1.13446401379631))*(cos((-0.0174532925199433)*_elbow+0.7853981633974481)*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_bicep)+cos(0.0174532925199433*_bicep)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481)))+cos(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin((-0.0174532925199433)*_elbow+0.7853981633974481)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481))+0.0174532925199433*cos(0.0174532925199433*_forearm+(-1.13446401379631))*(cos((-0.0174532925199433)*_shoulderio+0.7853981633974481)*cos(0.0174532925199433*_bicep)+(-1)*sin((-0.0174532925199433)*_shoulderio+0.7853981633974481)*sin(0.0174532925199433*_shoulderud+(-0.7853981633974481))*sin(0.0174532925199433*_bicep));
  // place derivatives in dy
  ik_leftarmdy=zeros(4,5);
  ik_leftarmdy(1,1)=dresidual0_d_shoulderio;
  ik_leftarmdy(1,2)=dresidual0_d_shoulderud;
  ik_leftarmdy(1,3)=dresidual0_d_bicep;
  ik_leftarmdy(1,4)=dresidual0_d_elbow;
  ik_leftarmdy(1,5)=dresidual0_d_forearm;
  ik_leftarmdy(2,1)=dresidual1_d_shoulderio;
  ik_leftarmdy(2,2)=dresidual1_d_shoulderud;
  ik_leftarmdy(2,3)=dresidual1_d_bicep;
  ik_leftarmdy(2,4)=dresidual1_d_elbow;
  ik_leftarmdy(2,5)=dresidual1_d_forearm;
  ik_leftarmdy(3,1)=dresidual2_d_shoulderio;
  ik_leftarmdy(3,2)=dresidual2_d_shoulderud;
  ik_leftarmdy(3,3)=dresidual2_d_bicep;
  ik_leftarmdy(3,4)=dresidual2_d_elbow;
  ik_leftarmdy(3,5)=dresidual2_d_forearm;
  ik_leftarmdy(4,1)=dresidual3_d_shoulderio;
  ik_leftarmdy(4,2)=dresidual3_d_shoulderud;
  ik_leftarmdy(4,3)=dresidual3_d_bicep;
  ik_leftarmdy(4,4)=dresidual3_d_elbow;
  ik_leftarmdy(4,5)=dresidual3_d_forearm;
endfunction

function ik_leftarmx=ik_leftarmsolve(ik_leftarmparameters)
  
  // declare globals
  
  global  ik_leftarmepsilon ik_leftarmsteps;
  
    // import globals
  
    epsilon=ik_leftarmepsilon; // epsilon
    steps=ik_leftarmsteps; // steps
  
    // import parameters
  
  shoulderio=ik_leftarmparameters(1);
  shoulderud=ik_leftarmparameters(2);
  bicep=ik_leftarmparameters(3);
  elbow=ik_leftarmparameters(4);
  forearm=ik_leftarmparameters(5);
  downx=ik_leftarmparameters(6);
  px=ik_leftarmparameters(7);
  py=ik_leftarmparameters(8);
  pz=ik_leftarmparameters(9);
  waist=ik_leftarmparameters(10);
  residual=ik_leftarmparameters(11);
  ik_leftarmresidual=0;
  
  // initialize unknowns from parameters
  
  ik_leftarmx=zeros(5,1);
  _shoulderio=shoulderio;
  _shoulderud=shoulderud;
  _bicep=bicep;
  _elbow=elbow;
  _forearm=forearm;
  
  // setup unknown vector x
  
  ik_leftarmx=zeros(5,1);
  ik_leftarmx(1)=_shoulderio;
  ik_leftarmx(2)=_shoulderud;
  ik_leftarmx(3)=_bicep;
  ik_leftarmx(4)=_elbow;
  ik_leftarmx(5)=_forearm;
  
  // newton iteration
  for step=1:steps
    ik_leftarmy=ik_leftarmf(ik_leftarmparameters,ik_leftarmx);
    ik_leftarmdy=ik_leftarmdf(ik_leftarmparameters,ik_leftarmx);
    ik_leftarmdyt=ik_leftarmdy'
    ik_leftarmdydyt=ik_leftarmdy*ik_leftarmdyt;
    ik_leftarmx=ik_leftarmx-ik_leftarmdyt*(ik_leftarmdydyt\ik_leftarmy);  
    ik_leftarmresidual=norm(ik_leftarmy);
    if (ik_leftarmresidual <= epsilon) then
      break;
    end
  end
  ik_leftarmparameters(11)=ik_leftarmresidual;
endfunction

function ik_leftarmnew_parameters=ik_leftarmupdate(ik_leftarmparameters)
  
  // declare globals
  
  global  ik_leftarmepsilon ik_leftarmsteps;
  
    // import globals
  
    epsilon=ik_leftarmepsilon; // epsilon
    steps=ik_leftarmsteps; // steps
  
    // import parameters
  
  shoulderio=ik_leftarmparameters(1);
  shoulderud=ik_leftarmparameters(2);
  bicep=ik_leftarmparameters(3);
  elbow=ik_leftarmparameters(4);
  forearm=ik_leftarmparameters(5);
  downx=ik_leftarmparameters(6);
  px=ik_leftarmparameters(7);
  py=ik_leftarmparameters(8);
  pz=ik_leftarmparameters(9);
  waist=ik_leftarmparameters(10);
  residual=ik_leftarmparameters(11);
  ik_leftarmx=ik_leftarmsolve(ik_leftarmparameters);
  
  // import variables
  
  _shoulderio=ik_leftarmx(1);
  _shoulderud=ik_leftarmx(2);
  _bicep=ik_leftarmx(3);
  _elbow=ik_leftarmx(4);
  _forearm=ik_leftarmx(5);
  shoulderio=_shoulderio;
  shoulderud=_shoulderud;
  bicep=_bicep;
  elbow=_elbow;
  forearm=_forearm;
  
  // assign to new_parameters
  
  ik_leftarmnew_parameters=zeros(11,1);
  ik_leftarmnew_parameters(1)=shoulderio;
  ik_leftarmnew_parameters(2)=shoulderud;
  ik_leftarmnew_parameters(3)=bicep;
  ik_leftarmnew_parameters(4)=elbow;
  ik_leftarmnew_parameters(5)=forearm;
  ik_leftarmnew_parameters(6)=downx;
  ik_leftarmnew_parameters(7)=px;
  ik_leftarmnew_parameters(8)=py;
  ik_leftarmnew_parameters(9)=pz;
  ik_leftarmnew_parameters(10)=waist;
  ik_leftarmnew_parameters(11)=residual;
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
