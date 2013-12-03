#pragma once

#include <iostream>
#include "Vec3d.h"
#include "Mat3d.h"
#include "Arc3d.h"

#include "Tape.h"

class TapeEditor
{
  Tape *m_tape;
 public:
  typedef std::vector < std::string > Names;
  Names names;

  Tape* tape();
  virtual void tape(Tape *tape_);

  float maxSimTime() const;
  float minSimTime() const;
  size_t size() const;

  float s;

  Tape::Path& paths(const std::string &name);

  void wait(float ds);

  bool parseId(std::istream &in, std::string &id);
  bool parse(std::istream &in, float &value);
  bool parse(std::istream &in, Vec3d &value);
  bool parse(std::istream &in, Mat3d &value);
  bool parse(std::istream &in, Arc3d &value);

  virtual bool parse(const std::string &cmd,std::istream &in);

  TapeEditor();
  ~TapeEditor();
};
