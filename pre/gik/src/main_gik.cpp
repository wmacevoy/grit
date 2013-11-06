#include "coptgen.hpp"
#include "mat.h"
#include "gsolve.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <set>
#include <string.h>

using namespace std;

Mat leg()
{
  E pi=var("%pi");
  //  E leg = var("leg");
  //  E achasis = (num(3)*pi/num(4))-(pi/num(2))*leg;
  E achasis=var("achasis");
  E drchasis2hip = var("drchasis2hip");
  E dzchasis2hip = var("dzchasis2hip");
  E dyknee2tip = var("dyknee2tip");
  E dzknee2tip = var("dzknee2tip");
  E dhip2femur=var("dhip2femur");
  E dfemur2knee=var("dfemur2knee");

  E ahip=var("ahip");
  E afemur=var("afemur");
  E aknee=var("aknee");
  
  Mat m = eye(4,4);

  m = rotate(o,ez,cos(achasis),sin(achasis));
  m = m*translate(vec(num(0),drchasis2hip,dzchasis2hip));
  m = m*rotate(o,ez,cos(ahip),sin(ahip));
  m = m*translate(vec(num(0),dhip2femur,num(0)));
  m = m*rotate(o,ex,cos(afemur),sin(afemur));
  m = m*translate(vec(num(0),dfemur2knee,num(0)));
  m = m*rotate(o,ex,cos(aknee),sin(aknee));
  m = m*translate(vec(num(0),dyknee2tip,dzknee2tip));

  return m;
}

Mat arm(string side)
{
  E sigma = num((side == "left") ? -1 : 1);
  
  Mat m = eye(4,4);

  E pi=var("%pi");
  E dxneck2shoulder=num(6.0);
  E dyneck2shoulder=num(2.0+3.0/8.0);
  E dshoulderio2ud=num(3.0+5.0/8.0);
  E dshoulderud2elbow=num(9.0);
  E delbow2palm=num(15.0);
  E shoulderio=var("shoulderio");
  E shoulderud=var("shoulderud");
  E bicep=var("bicep");
  E elbow=var("elbow");
  E forearm=var("forearm");

  m=m*translate(vec(sigma*dxneck2shoulder,dyneck2shoulder,num(0)));
  m=m*rotate(o,ez,cos(shoulderio),-sin(shoulderio));
  m=m*translate(vec(num(0),dshoulderio2ud,num(0)));
  m=m*rotate(o,ex,cos(shoulderud),sigma*sin(shoulderud));
  m=m*rotate(o,ey,cos(bicep),sin(bicep));
  m=m*translate(vec(num(0),dshoulderud2elbow,num(0)));
  m=m*rotate(o,ex,cos(elbow),sigma*sin(elbow));
  m=m*rotate(o,ey,cos(forearm),sin(forearm));
  m=m*translate(vec(num(0),delbow2palm,num(0)));

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

void gsolve_fk_arm(string side)
{
  Mat pose=arm(side);
  string dir=string("../ik");
  string hfile=dir + "/include/fk_" + side + "arm.h";
  string cppfile=dir + "/src/fk_" + side + "arm.cpp";

  {
    cout << "generating '" << hfile << "'" << endl;
    ofstream out(hfile.c_str());
    out << "#pragma once" << endl;
    out << "void fk_" << side << "palm(" << endl;
    out << "float shoulderio," << endl;
    out << "float shoulderud," << endl;
    out << "float bicep," << endl;
    out << "float elbow," << endl;
    out << "float forearm," << endl;
    out << "float pose[4][4]" << endl;
    out << ");" << endl;
  }
  {
    cout << "generating '" << cppfile << "'" << endl;
    ofstream out(cppfile.c_str());
    out << "#include <math.h>" << endl;
    out << "#include \"fk_" << side << "arm.hpp\"" << endl;

    out << endl;
    out << "void fk_" << side << "palm(" << endl;
    out << "float shoulderio," << endl;
    out << "float shoulderud," << endl;
    out << "float bicep," << endl;
    out << "float elbow," << endl;
    out << "float forearm," << endl;
    out << "float pose[4][4]" << endl;
    out << ")" << endl;
    out << "{" << endl;

    symbolic::COptGen coptgen;
    coptgen.type = "float";
    coptgen.format = &symbolic::format_c_single;

    coptgen.assign("shoulderio",&*var("angles[0]"));
    coptgen.assign("shoulderud",&*var("angles[1]"));
    coptgen.assign("bicep",&*var("angles[2]"));
    coptgen.assign("elbow",&*var("angles[3]"));
    coptgen.assign("forearm",&*var("angles[4]"));
    for (int r=0; r<4; ++r) {
      for (int c=0; c<4; ++c) {
	ostringstream oss;
	oss << "pose[" << r << "][" << c << "]";
	coptgen.assign(oss.str(),&*pose[r][c]);
      }
    }
    coptgen.print(out);
    
    out << "}" << endl;
  }
}


void gsolve_ik_arm(string side)
{
  Mat pose=arm(side);
  string dir=string("tmp");
  string inifile=dir + "/ik_" + side + "arm.ini";

  Vec p=vec(var("px"),var("py"),var("pz"));
  Vec n=vec(var("nx"),var("ny"),var("nz"));

  Mat r=arm(side);
  
  Mat r0=r*column(o);
  Mat rz=r*column(ez);

  Vec eq,x;

  eq.push_back(r0[0][0]-p[0]);
  eq.push_back(r0[1][0]-p[1]);
  eq.push_back(r0[2][0]-p[2]);
  eq.push_back(rz[0][0]-n[0]);
  eq.push_back(rz[1][0]-n[1]);

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

  Vec x_;
  for (size_t i=0; i != x.size(); ++i) {
    x_.push_back(var(name(x[i])+"_"));
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
    bar[name(x[i])]=&*x_[i];
  }

  for (size_t i=0; i != eq.size(); ++i) {
    eq[i]=E(substitute(bar,&*eq[i]));
  }

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

  x.push_back(var("aknee"));
  x.push_back(var("afemur"));
  x.push_back(var("ahip"));

  Vec x_;
  for (size_t i=0; i != x.size(); ++i) {
    x_.push_back(var(name(x[i])+"_"));
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
    bar[name(x[i])]=&*x_[i];
  }

  for (size_t i=0; i != eq.size(); ++i) {
    eq[i]=E(substitute(bar,&*eq[i]));
  }

  ofstream out("tmp/ik_leg.ini");
  gsolve(out,"leg",eq,x,p);
}


int main(int argc, char *argv[])
{
  for (int argi=1; argi < argc; ++argi) {
    if (strcmp(argv[argi],"leftarm")==0) {
      gsolve_fk_arm("left");      
      gsolve_ik_arm("left");
      continue;
    }
    if (strcmp(argv[argi],"rightarm")==0) {
      gsolve_fk_arm("right");      
      gsolve_ik_arm("right");
      continue;
    }
    if (strcmp(argv[argi],"leg")==0) {
      gsolve_leg();
      continue;
    }
    cout << "unknown argument '" << argv[argi] << "'" << endl;
  }

  return 0;

}
