#include <math.h>
#include "TurnArmTapeEditor.h"

TurnArmTapeEditor::TurnArmTapeEditor(const std::string &side_, 
				     const Arc3d &arc_,
				     float dr_,
				     float speed_)
  : ArmTapeEditor(side_), arc(arc_), dr(dr_), speed(speed_)
{
}


void TurnArmTapeEditor::start()
{
  Arc3d arc1(arc);
  arc1.radius = arc.radius + dr;
  open();
  s += 5.0; pose(arc1.pose(0));
}

void TurnArmTapeEditor::grasp()
{
  float s0=s;
  float s1=s0+1.0+dr/speed;
  Arc3d arc1(arc);
  int n=4;
  for (int i=1; i<=n; ++i) {
    float u=float(i)/float(n);
    arc1.radius = arc.radius + (1-u)*dr;
    s=s0+(s1-s0)*u; pose(arc1.pose(1));
  }
  close();
}

void TurnArmTapeEditor::turn()
{
  float s0=s;
  float s1=s0+arc.radius*(M_PI/180)*fabs(arc.theta1-arc.theta0)/speed;

  // 1 per inch
  int n1 = arc.radius*(M_PI/180)*fabs(arc.theta1-arc.theta0)/1.0;
  // 1 per second
  int n2 = (s1-s0)/1.0;

  int n=4;
  if (n1 > n) n=n1;
  if (n2 > n) n=n2;

  for (int i=1; i <= n; ++i) {
    float u=float(i)/float(n);
    s=s0+(s1-s0)*u; pose(arc.pose(u));
  }
}

void TurnArmTapeEditor::release()
{
  float s0=s;
  float s1=s0+1.0+dr/speed;
  Arc3d arc1(arc);
  open();
  int n=4;
  for (int i=1; i<=n; ++i) {
    float u=float(i)/float(n);
    arc1.radius = arc.radius + u*dr;
    s=s0+(s1-s0)*u; pose(arc1.pose(1));
  }
}

void TurnArmTapeEditor::restart()
{
  Arc3d arc1(arc);
  arc1.radius = arc.radius + dr;

  float s0=s;
  float s1=s0+arc1.radius*(M_PI/180)*fabs(arc1.theta1-arc1.theta0)/speed;
  
  // 1 per inch
  int n1 = arc1.radius*(M_PI/180)*fabs(arc.theta1-arc.theta0)/1.0;
  // 1 per second
  int n2 = (s1-s0)/1.0;
  
  int n=4;
  if (n < n1) n=n1;
  if (n < n2) n=n2;
	  
  for (int i=1; i <= n; ++i) {
    float u=float(i)/float(n);
    s=s0+(s1-s0)*(1-u); pose(arc1.pose(1-u));
  }
}

void TurnArmTapeEditor::cycle()
{
  grasp();
  turn();
  release();
  restart();
}

void TurnArmTapeEditor::cycles(int n)
{
  for (int i=0; i<n; ++i) cycle();
}

      bool TurnArmTapeEditor::parse(const std::string &cmd, std::istream &in)
      {
	if (ArmTapeEditor::parse(cmd,in)) return true;
	if (cmd == "arc") {
	  Arc3d arc_;
	  if (parse(in,arc_)) {
	    arc=arc_;
	    return true;
	  }
	}
	if (cmd == "dr") {
	  float dr_;
	  if (parse(in,dr_)) {
	    dr=dr_;
	    return true;
	  }
	}
	if (cmd == "speed") {
	  float speed_;
	  if (parse(in,speed_)) {
	    speed=speed_;
	    return true;
	  }
	}
	if (cmd == "start") { start(); return true; }
	if (cmd == "grasp") { grasp(); return true; }
	if (cmd == "turn")  { turn(); return true; }
	if (cmd == "release") { release(); return true; }
	if (cmd == "restart") { restart(); return true; }
        if (cmd == "cycle") { cycle(); return true; }
	if (cmd == "cycles") {
	  float n;
	  if (parse(in,n) && n == floor(n)) {
	    cycles(n);
	    return true;
	  }
	}
	return false;
      }

