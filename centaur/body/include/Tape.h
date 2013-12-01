#pragma once

#include <map>
#include <string>
#include "BodyMover.h"

class Tape
{
public:
  typedef std::map < float , float > Path;
  typedef std::map < std::string , Path > Paths;

  std::string name;
  Paths paths;

  void write(BodyMover &mover) const;
  void read(BodyMover &mover);
  float maxSimTime() const;
  float minSimTime() const;

  void clear();
  void same(float s);

  virtual ~Tape();
};
