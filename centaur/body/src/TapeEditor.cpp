#include "TapeEditor.h"
#include "BodyGlobals.h"

#include <math.h>

using namespace std;

Tape::Path& TapeEditor::paths(const std::string &name) 
{
  return tape()->paths[name];
}

size_t TapeEditor::size() const
{
  return names.size();
}

float TapeEditor::maxSimTime() const
{
  float ans = 0;
  if (m_tape != 0) {
    float found=false;
    const Tape::Paths &paths=m_tape->paths;
    for (size_t j=0; j != names.size(); ++j) {
      Tape::Paths::const_iterator i=paths.find(names[j]);
      if (i != paths.end() && i->second.size() != 0) {
	float s=i->second.rbegin()->first;
	if (found) {
	  ans = fmax(ans,s);
	} else {
	  found = true;
	  ans = s;
	}
      }
    }
  }
  return ans;
}

float TapeEditor::minSimTime() const
{
  float ans = 0;
  if (m_tape != 0) {
    float found=false;
    const Tape::Paths &paths=m_tape->paths;
    for (size_t j=0; j != names.size(); ++j) {
      Tape::Paths::const_iterator i=paths.find(names[j]);
      if (i != paths.end() && i->second.size() != 0) {
	float s=i->second.begin()->first;
	if (found) {
	  ans = fmin(ans,s);
	} else {
	  found = true;
	  ans = s;
	}
      }
    }
  }
  return ans;
}

void TapeEditor::tape(Tape *tape_) { 
  m_tape = tape_; 
  s=maxSimTime();
}

Tape* TapeEditor::tape() { 
  return m_tape; 
}

void TapeEditor::wait(float ds)
{
  s += ds;
}

bool TapeEditor::parseId(std::istream &in, std::string &id)
{
  while (isblank(in.peek())) in.get();
  int ch = in.peek();
  if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == '_')) {
    id.clear();
    while ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == '_') || (ch >= '0' && ch <= '9')) {
      id.push_back(ch);
      in.get();
      ch=in.peek();
    }
    return true;
  }
  return false;
}

bool TapeEditor::parse(std::istream &in, float &value)
{
  while (isblank(in.peek())) in.get();
  int ch = in.peek();
  if (ch >= '0' && ch <= '9' || ch == '-') {
    if (in >> value) return true;
  } else {
    std::string name;
    if (parseId(in,name)) {
      if (nums.find(name) != nums.end()) {
	value = nums[name];
	return true;
      }
    }
  }
  return false;
}

bool TapeEditor::parse(std::istream &in, Vec3d &value)
{
  while (isblank(in.peek())) in.get();
  int ch = in.peek();
  if (ch == '[') {
    if (in >> value) return true;
  } else {
    std::string name;
    if (in >> name) {
      if (vecs.find(name) != vecs.end()) {
	value = vecs[name];
	return true;
      }
    }
  }
  return false;
}

bool TapeEditor::parse(std::istream &in, Mat3d &value)
{
  string name;
  if (parseId(in,name)) {
    if (mats.find(name) != mats.end()) {
      value = mats[name];
      return true;
    }
  }
  return false;
}

bool TapeEditor::parse(std::istream &in, Arc3d &arc)
{
  string name;
  if (parseId(in,name)) {
    if (name == "ed") {
      while (isblank(in.peek())) in.get();
      if (in.peek() == '(') {
	in.get();
	Arc3d arc_;
	if (parse(in,arc_)) {
	  for (;;) {
	    while (isblank(in.peek())) in.get();
	    if (in.peek() == ')') { arc=arc_; return true; }
	    if (in.peek() != ',') { return false; }
	    in.get();

	    string field;
	    if (parseId(in,field)) {
	      while (isblank(in.peek())) in.get();
	      if (in.peek() != '=') { return false; }
	      in.get();
	      if ((field == "radius" && parse(in,arc_.radius)) ||
		  (field == "theta0" && parse(in,arc_.theta0)) ||
		  (field == "theta1" && parse(in,arc_.theta1)) ||
		  (field == "frame" && parse(in,arc_.frame))) {
	      } else {
		return false;
	      }
	    }
	  }
	}
      }
    }
    if (name == "fromPoints" || name == "frompoints") {
      while (isblank(in.peek())) in.get();
      if (in.peek() == '(') {
	in.get();
	Vec3d p1;
	if (parse(in,p1)) {
	  while (isblank(in.peek())) in.get();
	  if (in.peek() == ',') {
	    in.get();
	    Vec3d p2;
	    if (parse(in,p2)) {
	      while (isblank(in.peek())) in.get();
	      if (in.peek() == ',') {
		in.get();
		Vec3d p3;
		if (parse(in,p3)) {
		  while (isblank(in.peek())) in.get();
		  if (in.peek() == ')') {
		    in.get();
		    arc.fromPoints(p1,p2,p3);
		    return true;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
    if (arcs.find(name) != arcs.end()) {
      arc = arcs[name];
      return true;
    }
  }
  return false;
}

bool TapeEditor::parse(const std::string &cmd, std::istream &in)
{
  if (cmd == "wait") {
    float ds;
    if (parse(in,ds)) {
      wait(ds);
      return true;
    }
  }
  return false;
}

TapeEditor::TapeEditor() 
{
  m_tape = 0;
  s=0;
  // cannot virtualize in constructor
}

TapeEditor::~TapeEditor() {
}
