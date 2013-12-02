#include "Tape.h"

#include <math.h>
#include <iostream>

using namespace std;

void Tape::write(BodyMover &mover) const
{
  for (Paths::const_iterator i = paths.begin(); i != paths.end(); ++i) {
    const string &name = i->first;
    const Path &path = i->second;
    ServoMover *servoMover = mover.getMover(name);
    if (servoMover != 0) {
      float simTime0 = path.size() > 0 ? path.begin()->first : 0.0;
      float simTime1 = path.size() > 0 ? path.rbegin()->first : 0.0;
      std::map < float , float > path0;
      for (Path::const_iterator j = path.begin(); j != path.end(); ++j) {
	path0[j->first-simTime0]=j->second;
      }
      servoMover->setup(path0,simTime0,simTime1);
      cout << "tape " << name << ": ";
      cout << "s=[" << simTime0 << "," << simTime1 << "]";
      for (Path::const_iterator j=path.begin(); j != path.end(); ++j) {
	cout << " " << j->first << "->" << j->second;
      }
      cout << endl;
    }
  }
}

void Tape::read(BodyMover &mover)
{
  for (Paths::iterator i = paths.begin(); i != paths.end(); ++i) {
    const string &name = i->first;
    Path &path = i->second;
    path.clear();
    ServoMover *servoMover = mover.getMover(name);
    if (servoMover != 0) {
      Curve::Knots &knots = servoMover->angles.knots;
      for (Curve::Knots::iterator i = knots.begin(); i != knots.end(); ++i) {
	path[i->x]=i->y;
      }
    }
  }
}

float Tape::minSimTime() const
{
  bool found = false;
  float ans=0;
  for (Paths::const_iterator i = paths.begin(); i != paths.end(); ++i) {
    if (!i->second.empty()) { 
      float t=i->second.begin()->first;
      if (found) {
	ans=fmin(ans,t);
      } else {
	found  = true;
	ans = t;
      }
    }
  }
  return ans;
}

float Tape::maxSimTime() const
{
  bool found = false;
  float ans=0;
  for (Paths::const_iterator i = paths.begin(); i != paths.end(); ++i) {
    if (!i->second.empty()) { 
      float t=i->second.begin()->first;
      if (found) {
	ans=fmax(ans,t);
      } else {
	found  = true;
	ans = t;
      }
    }
  }
  return ans;
}

void Tape::clear()
{
  for (Paths::iterator i = paths.begin(); i != paths.end(); ++i) {
    Path &path = i->second;
    path.clear();
  }
}

void Tape::same(float s)
{
  for (Paths::iterator i = paths.begin(); i != paths.end(); ++i) {
    Path &path = i->second;
    float at=path.rbegin()->second;
    path[s]=at;
  }
}

Tape::~Tape() {}

