#pragma once

#include <memory>
#include <vector>

#include "Body.h"
#include "LegsMover.h"
#include "ServoMover.h"
#include "NeckMover.h"
#include "LeftArmMover.h"
#include "RightArmMover.h"

class WalkParameters {
  public:
    double radius;
    double x1,y1,z1;
    double x2,y2,z2;
    double x3,y3,z3;
    double x4,y4,z4;
    double z;
    double step;
    double direction;
    double zStep;
    int repeat;
    double rotation;
    double zOffset;
    WalkParameters(double radius,double x,double y,double z,double step,double direction,double zStep);
};

class BodyMover
{
 public:
  LegsMover legs;
  ServoMover waist;
  NeckMover neck;
  LeftArmMover left;
  RightArmMover right;

  BodyMover();
//  std::vector<std::vector<double> > bMove(double radius,double x,double y,double z,double step,double direction,double zStep,int repeat=1,float rotation=0.0,float zoffset=0.0);
  std::vector<std::vector<double> > bMove(WalkParameters wp);
  std::vector<std::vector<double> > createMove(double radius,double x,double y,double z,double xstep,double ystep,double zAdder,double left,double right,bool narrow=false,int repeat=1);
  bool stepMove(double r,double x,double y,double z,double xstep,double ystep,double stepheight,double left,double right,bool narrow=false,int repeat=1);
  bool blended(double radius,double x,double y,double z,double xstep,double ystep,double zAdder,double left,double right,bool narrow=false,int repeat=1);
//  bool bStep(double radius,double x,double y,double z,double step,double directionDegrees,double zStep,int repeat=1,float rotation=0.0,float zoffset=0.0);
  bool bStep(WalkParameters wp);
  std::vector<std::vector<double> > stepMerge(std::vector<std::vector <double> > start,std::vector<std::vector <double> > end);
  void changeZ(WalkParameters start,double newZ);
  void stepMerge(WalkParameters start,WalkParameters end);
  void logPosition(std::vector<std::vector <double> > data);
  void fromTips(std::vector<std::vector <double> > data);  
  void move(Body &body);
  bool load(const std::string &file);
  bool play(const std::string &file);
  ServoMover* getMover(const std::string &name);

  bool done() const;
};

typedef std::shared_ptr < BodyMover > SPBodyMover;
