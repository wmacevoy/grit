#pragma once

#include <memory>
#include <string>

#include "mat.h"

class Part;
typedef std::shared_ptr < Part > PartSP;
typedef std::map < std::string , PartSP > Parts;
typedef std::map < std::string , E > Symbols;

class Part
{
public:
  Part *parent;
  std::string name;

  Parts parts;
  Symbols symbols;

  Mat frame;
  Mat world() const;
  Part(Part *parent_, const std::string &name_);
  void add(Part *child);
  virtual ~Part();
};
