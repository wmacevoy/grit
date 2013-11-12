#pragma once

#include <memory>
#include <vector>

#include "Body.h"
#include "LegsMover.h"
#include "ServoMover.h"
#include "NeckMover.h"
#include "LeftArmMover.h"
#include "RightArmMover.h"

class BodyMover
{
 public:
  LegsMover legs;
  ServoMover waist;
  NeckMover neck;
  LeftArmMover left;
  RightArmMover right;

  BodyMover();
  std::vector<std::vector<double> > bMove(double radius,double x,double y,double z,double step,double direction,double zStep,int repeat=1,float rotation=0.0,float zoffset=0.0);
  std::vector<std::vector<double> > createMove(double radius,double x,double y,double z,double xstep,double ystep,double zAdder,double left,double right,bool narrow=false,int repeat=1);
  bool stepMove(double r,double x,double y,double z,double xstep,double ystep,double stepheight,double left,double right,bool narrow=false,int repeat=1);
  bool blended(double radius,double x,double y,double z,double xstep,double ystep,double zAdder,double left,double right,bool narrow=false,int repeat=1);
  bool bStep(double radius,double x,double y,double z,double step,double directionDegrees,double zStep,int repeat=1,float rotation=0.0,float zoffset=0.0);
  void logPosition(std::vector<std::vector <double> > data);
  void fromTips(std::vector<std::vector <double> > data);  
  void move(Body &body);
  bool load(const std::string &file);
  bool play(const std::string &file);
  ServoMover* getMover(const std::string &name);

  bool done() const;
};

typedef std::shared_ptr < BodyMover > SPBodyMover;
