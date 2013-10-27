#include "mat.h"
#include "gsolve.h"
#include <iostream>
#include <tr1/memory>
using namespace std;

class Component;
typedef std::tr1::shared_ptr < Component > ComponentSP;

class Component
{
public:
  Component *parent;
  std::string name;
  Mat frame;
  std::map < std::string , ComponentSP > parts;
  std::map < std::string , E > parameters;

  Mat world()
  {
    Mat m=frame;

    for (Component *p=parent; p != 0; p = p->parent) {
      m = (p->frame)*m;
    }
    return m;
  }

  Component() {
    parent=0;
    name="";
  }


  void name(const std::string &name_) {
    if (this->parent != 0) {
      // keep ref count above 1 while adjusting name...
      ComponentSP me=parent->parts[name];
      parent->remove(this);
      name=_name;
      parent->add(this);
    } else {
      name=_name;
    }
  }

  void init(Component *parent_, const std::string &name_)
  {
    parent=parent_;
    frame = eye(4,4);
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
    parts[child->name]=child;
  }

  void remove(Component *part)
  {
    std:map < std::string , ComponentSP > :: iterator i = 
      parts.find(name);
    if (i != parts.end()) {
      parts.erase(i);
    }
  }

};

ComponentSP box(Component *parent, const std::string name, const Vec &w, const E &mass)
{
  ComponentSP ans = ComponentSP(new Component());
  ans->init(parent,name);
  ans->parameters["wx"]=clone(w[0]);
  ans->parameters["wy"]=clone(w[1]);
  ans->parameters["wz"]=clone(w[2]);
  ans->parameters["mass"]=mass;
  ans->parameters["cx"]=num(0);
  ans->parameters["cy"]=num(0);
  ans->parameters["cz"]=num(0);
  ans->parameters["volume"]=w[0]*w[1]*w[2];
  ans->parameters["density"]=ans->parameters["mass"]/ans->parameters["volume"];
  ans->parameters["ixx"]=(num(1.0/12.0)*ans->parameters["mass"]*(pow(w[1],num(2))+pow(w[2],num(2))));
  ans->parameters["ixy"]=num(0);
  ans->parameters["ixz"]=num(0);
  ans->parameters["iyy"]=(num(1.0/12.0)*ans->parameters["mass"]*(pow(w[0],num(2))+pow(w[2],num(2))));
  ans->parameters["iyz"]=0;
  ans->parameters["izz"]=(num(1.0/12.0)*ans->parameters["mass"]*(pow(w[0],num(2))+pow(w[1],num(2))));
};

class Box : public Component
{
  void init(Component *parent, const std::string name, const Vec &w, const E &mass)
    : Component(parent,name) {
  }
};

class Body;
class Legs;
class Torso;
class Head;
class Arms;
class Arm;
class Hand;
class Finger;


class Foot : public Component
{
  void init();
};

class Lower
class Body : public Box
{
  Torso *torso;
  Leg *legs[4];
  void init();
};

class Torso : public Component
{
  Torso *torso;
};

class 

// inches and pounds for dimension units here
class Body : public Box
{
public:
  void init() {
    Box::init(0,"body",11.0,11.0,6.0, 10.0);
    frame = transpose(vec(num(0),num(0),parameter["wz"]/num(2)));
    parts["legs"] = new Legs(this);
    parts["torso"] = new Torso(this);
  }
};

body->legs->leg[k]->hip->femur->knee->foot->toe
    ->torso->necklr->neckud->head
           ->arm[k]->shoulderlr->shoulderud->bicep->elbow->forearm->hand

class Hip : public Box
{
  Leg *leg;
  void init(Leg *leg_)
  {
    E wx=num(2.0);
    E wy=num(11.0);
    E wz=num(2.0);
    Box::init(leg_,"hip",vec(wx,wy,wz));
    frame=rotate(
    leg=leg_;
    
    frame = 
  }
}
class Leg : public Component
{
  Legs *legs;

  Hip *hip;
  Femur *femur;
  Knee *knee;
  Foot *foot;

  std::string name(int number) {
  }

  double angle(int number)
  {
    return 3*M_PI/4-M_PI/2*number;
  }

  double distance(int number)
  {
    return sqrt(pow(11.0,2)+pow(11.0,2));
  }

  void init(Legs *legs_, int id_)
  {
    legs=legs_;

    std::ostreamstream oss;
    oss << "leg" << (id_);

    Component::init(legs,oss.str());

    E pi=var("%pi");
    E id=num(id_);
    E a=num(5)*pi/num(4)-pi/num(2)*id;
    E wx = legs->body->parameter["wx"];
    E wy = legs->body->parameter["wy"];
    E wz = legs->body->parameter["wz"];
    E dr = 2.0;
    E d = pow(pow(wx,2)+pow(wy,2),0.5)+dr;
    
    parameters["id"]=id;
    parameters["a"]=angle;
    parameters["d"]=d;

    frame = rotate(o,ez,cos(a),sin(a))
      *translate(vec(num(0),d,wz/num(2)));

    hip = new Hip();
    hip->init(this);

    femur = new Femur();
    femur->init(hip);

    knee = new Knee();
    knee->init(femur);

    foot = new Foot();
    foot->init(knee);
  }
};

class Legs : public Component
{
public:
  Body *body;
  Leg legs[4];

  void init(Body *body_) {
    body=body_;
    Component::init(body,"body");
    frame=translate(vec(0,0,body->parameter("wz")/num(2)));
    for (int i=0; i<4; ++i) {
      legs[i] = new Leg(this,i);
    }
  }
};


