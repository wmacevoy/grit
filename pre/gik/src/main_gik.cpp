#include "coptgen.hpp"
#include "mat.h"
#include "gsolve.h"
#include "toupper.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <set>
#include <string.h>
#include "coptgen.hpp"
#include "formatter.hpp"

using namespace std;

Mat leg()
{
  E pi=var("%pi");
  //  E leg = var("leg");
  //  E achasis = (num(3)*pi/num(4))-(pi/num(2))*leg;
  E chasis=(pi/num(180))*var("chasis");
  E drchasis2hip = var("drchasis2hip");
  E dzchasis2hip = var("dzchasis2hip");
  E dyknee2tip = var("dyknee2tip");
  E dzknee2tip = var("dzknee2tip");
  E dhip2femur=var("dhip2femur");
  E dfemur2knee=var("dfemur2knee");

  E hip=(pi/num(180))*var("hip");
  E femur=(pi/num(180))*var("femur");
  E knee=(pi/num(180))*var("knee");
  
  Mat m = eye(4,4);

  m = rotate(o,ez,cos(chasis),sin(chasis));
  m = m*translate(vec(0.0,drchasis2hip,dzchasis2hip));
  m = m*rotate(o,ez,cos(hip),sin(hip));
  m = m*translate(vec(0.0,dhip2femur,0.0));
  m = m*rotate(o,ex,cos(femur),sin(femur));
  m = m*translate(vec(0.0,dfemur2knee,0.0));
  m = m*rotate(o,ex,cos(knee),sin(knee));
  m = m*translate(vec(0.0,dyknee2tip,dzknee2tip));

  return m;
}


Mat head()
{
  Mat m = eye(4,4);

  E pi=var("%pi");
  E dbase2waist=12.0;
  E waist=-(pi/180.0)*(var("waist"));
  E dwaist2neck=13.5;
  Vec neck2necklr=vec(0.0,0.0,1.5/2.0+1.75);
  E necklr=(pi/180.0)*var("necklr");
  Vec necklr2neckud=vec(0.0,0.0,1.75);
  E neckud=(pi/180.0)*var("neckud");
  Vec neckud2head=vec(0.0,0.0,1.0+3.0/8.0);

  m=m*translate(0.0,0.0,dbase2waist);
  m=m*rotate(ex,waist);
  m=m*translate(0.0,0.0,dwaist2neck);
  m=m*translate(neck2necklr);
  m=m*rotate(ez,necklr);
  m=m*translate(necklr2neckud);
  m=m*rotate(ex,neckud);
  m=m*translate(neckud2head);
  return m;
}

Mat lidar()
{
  Mat m=head();
  Vec head2lidar=vec(0.0,-1.50,1.75);
  m=m*translate(head2lidar);
  return m;
}

Mat camera()
{
  Mat m=head();
  Vec head2camera=vec(0.0,1.0+5.0/8.0,5.0/8.0);
  m=m*translate(head2camera);
  return m;
}

Mat arm(string side)
{
  string SIDE=utilities::toupper(side);
  E sigma = (side == "left") ? -1 : 1;
  
  Mat m = eye(4,4);

  E pi=var("%pi");
  E dbase2waist=12.0;
  E waist=-(pi/180.0)*(var("waist")); // check
  E dwaist2neck=13.5; // check
  E dxneck2shoulder=(9.0+3.0/4.0)/2.0;
  E dyneck2shoulder=2.0+3.0/8.0;
  E dshoulderio2ud=3.0+5.0/8.0;
  E dshoulderud2elbow=9.0;
  E delbow2palm=14.0;
  E shoulderio=sigma*(pi/180.0)*(var("shoulderio")-45.0);
  E shoulderud=(pi/180.0)*(var("shoulderud")-45.0);
  E bicep=-sigma*(pi/180.0)*var("bicep");
  E elbow=-(pi/180.0)*(var("elbow")-45.0);
  E forearm=-sigma*(pi/180.0)*(var("forearm")-40.0);

  m=m*translate(0.0,0.0,dbase2waist);
  m=m*rotate(ex,waist);
  m=m*translate(0.0,0.0,dwaist2neck);
  m=m*translate(sigma*dxneck2shoulder,dyneck2shoulder,0.0);

  m=m*rotate(ez,shoulderio);
  m=m*translate(0.0,dshoulderio2ud,0.0);
  m=m*rotate(ex,shoulderud);
  m=m*rotate(ey,bicep);
  m=m*translate(0.0,dshoulderud2elbow,0.0);
  m=m*rotate(ex,elbow);
  m=m*rotate(ey,forearm);
  m=m*translate(0.0,delbow2palm,0.0);

  return m;
}


Vec parameters(const Vec &eq, const Vec &x)
{
  Vec p;
  set<string> s;

  for (size_t i=0; i != eq.size(); ++i) {
    eq[i]->symbols(s);
  }

  for (size_t i=0; i != x.size(); ++i) {
    s.erase(name(x[i]));
  }

  for (set<string>::const_iterator i=s.begin(); i != s.end(); ++i) {
    p.push_back(var(*i));
  }

  return p;
}

void gfk_head()
{
  string dir=string("../../drivers/ik");

  {
    ofstream out(dir + "/include/fk_head.h");
    out << "#pragma once" << endl;
    out << "#include \"Mat3d.h\"" << endl;
    out << "Mat3d fk_head(float waist, float necklr, float neckud);" << endl;
  }

  {
    ofstream out(dir + "/src/fk_head.cpp");
    out << "#include \"fk_head.h\"" << endl;
    out << "#include <math.h>" << endl;
    out << "Mat3d fk_head(float waist, float necklr, float neckud)" << endl;
    out << "{" << endl;
    out << "  Mat3d ans;" << endl;
    symbolic::COptGen coptgen;
    coptgen.type="float";
    coptgen.format = &symbolic::format_c_single;
    Mat pose=head();
    for (int r=0; r<3; ++r) {
      for (int c=0; c<4; ++c) {
	ostringstream oss;
	oss << "ans(" << r << "," << c << ")";
	coptgen.assign(oss.str(),&*pose[r][c]);
      }
    }
    out << coptgen;
    out << "  return ans;" << endl;
    out << "}";
  }
}

void gsolve_ik_lidar()
{
  string dir=string("../../drivers/ik");
  string inifile=dir + "/ik_lidar.ini";

  Vec p=vec(var("px"),var("py"),var("pz"));

  Mat pose=lidar();

  Vec eq,x;

  Vec ex=vec(pose[0][0],pose[0][1],pose[0][2]);
  Vec ey=vec(pose[1][0],pose[1][1],pose[1][2]);
  Vec ez=vec(pose[2][0],pose[2][1],pose[2][2]);
  Vec o=vec(pose[3][0],pose[3][1],pose[3][2]);

  eq.push_back(dot(ex,p-o));
  eq.push_back(dot(ez,p-o));

  E necklr=var("necklr");
  E neckud=var("neckud");

  x.push_back(necklr);
  x.push_back(neckud);

  Vec _x;
  for (size_t i=0; i != x.size(); ++i) {
    _x.push_back(var(string("_")+name(x[i])));
  }

  Vec parms;
  set<string> s;

  for (size_t i=0; i != eq.size(); ++i) {
    eq[i]->symbols(s);
  }

  for (size_t i=0; i != x.size(); ++i) {
    s.erase(name(x[i]));
    parms.push_back(var(name(x[i])));
  }

  for (set<string>::const_iterator i=s.begin(); i != s.end(); ++i) {
    parms.push_back(var(*i));
  }

  map<string,const symbolic::Expression *> bar;

  for (size_t i=0; i != x.size(); ++i) {
    bar[name(x[i])]=&*_x[i];
  }

  for (size_t i=0; i != eq.size(); ++i) {
    eq[i]=E(substitute(bar,&*eq[i]));
  }

  cout << "generating '" << inifile << "'" << endl;
  ofstream out(inifile.c_str());
  gsolve(out,"lidar",eq,x,parms);
}

void gsolve_ik_arm(string side)
{
  E pi=var("%pi");
  string SIDE=utilities::toupper(side);
  string dir=string("../../drivers/ik");
  string inifile=dir + "/ik_" + side + "arm.ini";

  Vec p=vec(var("px"),var("py"),var("pz"));
  E pointx = var("pointx");
  E downx = var("downx");

  Mat pose=arm(side);

  Vec eq,x;

  eq.push_back(pose[0][3]-p[0]);
  eq.push_back(pose[1][3]-p[1]);
  eq.push_back(pose[2][3]-p[2]);

  eq.push_back(pose[0][1]-pointx);
  eq.push_back(pose[0][2]+downx);

  E shoulderio=var("shoulderio");
  E shoulderud=var("shoulderud");
  E bicep=var("bicep");
  E elbow=var("elbow");
  E forearm=var("forearm");

  x.push_back(shoulderio);
  x.push_back(shoulderud);
  x.push_back(bicep);
  x.push_back(elbow);
  x.push_back(forearm);

  Vec _x;
  for (size_t i=0; i != x.size(); ++i) {
    _x.push_back(var(string("_")+name(x[i])));
  }

  Vec parms;
  set<string> s;

  for (size_t i=0; i != eq.size(); ++i) {
    eq[i]->symbols(s);
  }

  for (size_t i=0; i != x.size(); ++i) {
    s.erase(name(x[i]));
    parms.push_back(var(name(x[i])));
  }

  for (set<string>::const_iterator i=s.begin(); i != s.end(); ++i) {
    parms.push_back(var(*i));
  }

  map<string,const symbolic::Expression *> bar;

  for (size_t i=0; i != x.size(); ++i) {
    bar[name(x[i])]=&*_x[i];
  }

  for (size_t i=0; i != eq.size(); ++i) {
    eq[i]=E(substitute(bar,&*eq[i]));
  }

  cout << "generating '" << inifile << "'" << endl;
  ofstream out(inifile.c_str());
  gsolve(out,side+"arm",eq,x,parms);
}

void gsolve_leg()
{
  E px=var("px");
  E py=var("py");
  E pz=var("pz");
  Mat r=leg()*column(o)-column(vec(px,py,pz));

  Vec eq;
  Vec x;

  eq.push_back(r[0][0]);
  eq.push_back(r[1][0]);
  eq.push_back(r[2][0]);

  x.push_back(var("knee"));
  x.push_back(var("femur"));
  x.push_back(var("hip"));

  Vec _x;
  for (size_t i=0; i != x.size(); ++i) {
    _x.push_back(var(string("_")+name(x[i])));
  }

  Vec p;
  set<string> s;

  for (size_t i=0; i != eq.size(); ++i) {
    eq[i]->symbols(s);
  }

  for (size_t i=0; i != x.size(); ++i) {
    s.erase(name(x[i]));
    p.push_back(var(name(x[i])));
  }

  for (set<string>::const_iterator i=s.begin(); i != s.end(); ++i) {
    p.push_back(var(*i));
  }

  map<string,const symbolic::Expression *> bar;

  for (size_t i=0; i != x.size(); ++i) {
    bar[name(x[i])]=&*_x[i];
  }

  for (size_t i=0; i != eq.size(); ++i) {
    eq[i]=E(substitute(bar,&*eq[i]));
  }

  string ini="../../drivers/ik/ik_leg.ini";
  cout << "generating '" << ini << "'" << endl;
  ofstream out(ini.c_str());
  gsolve(out,"leg",eq,x,p);
}


int main(int argc, char *argv[])
{
  for (int argi=1; argi < argc; ++argi) {
    if (strcmp(argv[argi],"ik_leftarm")==0) {
      gsolve_ik_arm("left");
      continue;
    }
    if (strcmp(argv[argi],"ik_rightarm")==0) {
      gsolve_ik_arm("right");
      continue;
    }
    if (strcmp(argv[argi],"fk_head")==0) {
      gfk_head();
      continue;
    }
    if (strcmp(argv[argi],"ik_lidar")==0) {
      gsolve_ik_lidar();
      continue;
    }
    cout << "unknown argument '" << argv[argi] << "'" << endl;
  }

  return 0;

}
