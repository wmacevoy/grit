#include "mat.h"
#include "gsolve.h"
#include <iostream>

using namespace std;

class Component;
typedef std::shared_ptr < Component > ComponentSP;

class Component
{
public:
  Component *parent;
  std::string name;
  Mat frame;
  E mass;
  Mat moments;
  std::string mesh;

  std::map < std::string , ComponentSP > parts;
  std::map < std::string , double > parameters;
  std::map < std::string , E > variables;

  void sdl(std::ostream &out)
  {

  }

  Mat world()
  {
    Mat m=frame;

    for (Component *p=parent; p != 0; p = p->parent) {
      m = (p->frame)*m;
    }
    return m;
  }

  Component(Component *parent_, const std::string &name_, )
  {
    frame = eye(4,4);
    parent=parent_;
    name=name_;
    if (parent != 0) {
      parent->add(this);
    }
  }

  void parameter(const std::string &parameterName, double value)
  {
    parameters[parameterName] = num(value);
  }

  double parameter(const std::string &parameterName) const
  {
    std::map < std::string , E > i = parameters.find(parameterName);
    if (i == parameters.end()) {
      Expression::UnsupportedOperation err;
      err.message = name + " does not have parameter " + parameterName;
      throw err;
    }
    return i->second->evaluate(parameters);
  }

  void add(Component *child)
  {
    for (size_t i=0; i < parts.size(); ++i) {
      if (*parts[i] == child) return;
    }
    parts.push_back(new ComponentSP(child));
  }
};

class Box : public Component
{
  Box(const Vec &w, const E &mass) {
    parameters["wx"]=clone(w[0]);
    parameters["wy"]=clone(w[1]);
    parameters["wz"]=clone(w[2]);
    parameters["mass"]=mass;
    parameters["cx"]=num(0);
    parameters["cy"]=num(0);
    parameters["cz"]=num(0);
    parameters["volume"]=w[0]*w[1]*w[2];
    parameters["density"]=parameters["mass"]/parameters["volume"];
    parameters["ixx"]=(num(1.0/12.0)*parameters["mass"]*(pow(w[1],num(2))+pow(w[2],2)));
    parameters["ixy"]=num(0);
    parameters["ixz"]=num(0);
    parameters["iyy"]=(num(1.0/12.0)*parameters["mass"]*(pow(w[0],num(2))+pow(w[2],2)));
    parameters["iyz"]=0;
    parameters["izz"]=(num(1.0/12.0)*parameters["mass"]*(pow(w[0],num(2))+pow(w[1],2)));
  }
};

class Cap : public Component
{

};

class Rod : public Component
{


};

class Ball : pulic Component
{

};

class Body : public Component
{
public:
  Body() : Component(0,"body") 
  { 
  }
};

ComponentSP centaur()
{
  ComponentSP body = new Component(0,"centaur");
  for (int leg=1; leg <= 4; ++leg) {
    ostringstream name;
    name << "leg" << leg;
    ComponentSP leg = new Component(body,name.str());
    leg.parameters["achasis"]=num(5.0*M_PI/4.0-M_PI/2*leg);
    leg.parameters["drchasis2hip"] = num(14.0);
    leg.parameters["dzchasis2hip"] = num(4.0);
    leg.parameters["dyknee2tip"] = num(12.0);
    leg.parameters["dzknee2tip"] = num(1.5);
    leg.parameters["dhip2femur"] = num(2.0);
    leg.parameters["dfemur2hip"] = num(12.0);
  }
  E ahip=var("ahip");
  E afemur=var("afemur");
  E aknee=var("aknee");

    

  ComponentSP leg1 = new Component(body,"leg1");
  ComponentSP leg1 = new Component(body,"leg1");

  centaur = new 

}


class Body : public Component
{
  Body()
  {
    parts["legs"]=Legs(this);
    parts["torso"]=Torso(this);
  }

}

class Leg
{


};

class Centaur
{
  


};

Mat hand()
{
  

}

Mat tip()
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

void gsolve_tip()
{
  E px=var("px");
  E py=var("py");
  E pz=var("pz");
  Mat r=tip()*column(o)-column(vec(px,py,pz));

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

  gsolve("tip",eq,x,p);
}



void gik_arm(string side)
{

}

int main(int argc, char *argv[])
{
  gsolve_tip();

#if 0
  for (int argi=1; argi<argc; ++argi) {
    if (strcmp(argv[argi],"leg") == 0) {
      int number = atoi(argv[++argi]); // 1 - 4
      gik_leg(number);
      continue;
    }
    if (strcmp(argv[argi],"arm") == 0) {
      string side = atoi(argv[++argi]); // left or right
      gik_arm(side);
      continue;
    }
  }
#endif
}
