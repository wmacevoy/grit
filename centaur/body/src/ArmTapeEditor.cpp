#include "ArmTapeEditor.h"

#include <assert.h>

ArmTapeEditor::ArmTapeEditor(const std::string &side_)
  : HandTapeEditor(side_)
{
  if (side == "LEFTARM") {
    geometry = std::shared_ptr < ArmGeometry > (new LeftArmGeometry());
  } else if (side == "RIGHTARM") {
    geometry = std::shared_ptr < ArmGeometry > (new RightArmGeometry());
  } else {
    assert(false);
  }

  shoulderio=side + "_SHOULDER_IO";
  shoulderud=side + "_SHOULDER_UD";
  bicep=side="_BICEP_ROTATE";
  elbow=side="_ELBOW";
  forearm=side+"_FOREARM_ROTATE";

  names.push_back(shoulderio);
  names.push_back(shoulderud);
  names.push_back(bicep);
  names.push_back(elbow);
  names.push_back(forearm);
}

void ArmTapeEditor::pose(const Mat3d &frame)
{
  Vec3d at=frame.o();
  Vec3d up=frame.ez();
  Vec3d point=frame.ey();


  if (up.z() < 0) { up = -up; }
  if (point.y() < 0) { point = -point; }

  std::cout << "ArmTapeEditor::pose(s=" << s << ",at=" << at << ",up=" << up << ",point=" << point << ")" << std::endl;

  geometry->compute(at.x(),at.y(),at.z(),/* point.x(), */ -up.x(),
		    paths(shoulderio)[s],
		    paths(shoulderud)[s],
		    paths(bicep)[s],
		    paths(elbow)[s],
		    paths(forearm)[s]);
}

void ArmTapeEditor::line(const Mat3d &f0, const Mat3d &f1, float speed, bool first, bool last)
{
  float d=dist(f0.o(),f1.o());
  float s0=s;
  float s1=s0+d/speed;
  int n1=d/1.0; // one point per inch
  int n2=(s1-s0)/1.0; // one per second
  int n=4;
  if (n < n1) n = n1;
  if (n < n2) n = n2;
  for (int i=0; i<=n; ++i) {
    if (i == 0 && !first) continue;
    if (i == n && !last)  continue;
    float u=float(i)/float(i);
    Mat3d f=(1-u)*f0+u*f1;
    s=(1-u)*s0+u*s1;
    pose(f);
  }
}

bool ArmTapeEditor::parse(const std::string &cmd, std::istream &in)
{
  if (cmd == "pose") {
    Mat3d m;
    if (parse(in,m)) {
      pose(m);
      return true;
    }
  }
  if (cmd == "line") {
    char b;
    in >> b;
    if (b == '(' || b == '[') {
      Mat3d f0;
      if (parse(in,f0)) {
	while (isblank(in.peek()) || in.peek() == ',') in.get();
	Mat3d f1;
	if (parse(in,f1)) {
	  char e;
	  in >> e;
	  if (e == ')' || e == ']') {
	    float speed;
	    if (!parse(in,speed)) speed = 4.0;
	    line(f0,f1,speed,b=='[',e==']');
	    return true;
	  }
	}
      }
    }
  }
  return false;
}

