#pragma once

#include <map>
#include <string>
#include "BodyMover.h"

class Tape
{
public:
  typedef std::map < float , float > Path;
  typedef std::map < std::string , Path > Paths;

  Paths paths;

  void write(BodyMover &mover) const;
  void read(BodyMover &mover);
  float maxSimTime() const;
  float minSimTime() const;
  void clear();
  void same(float s);

  void shift(float delta);
  void rescale(float lambda);
  void remap(float new_s0,float new_s1,float old_s0, float old_s1);
  void remap(float new_s0,float new_s1);

  void cluster(float epsilon=1e-4);
  void include(const Tape &inc);
  
  bool save(const std::string &file);
  bool load(const std::string &file);

  virtual ~Tape();
};
