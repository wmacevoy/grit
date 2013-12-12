#pragma once

#include <mutex>
#include <map>
#include <string>

#include "Vec3d.h"
#include "Mat3d.h"
#include "Arc3d.h"

class Symbols
{
 private:
  mutable std::mutex access;
  std::map < std::string , std::string > strs;
  static const std::string STR_DEFAULT;
  std::map < std::string , float > nums;
  static const float NUM_DEFAULT;
  std::map < std::string , Vec3d > vecs;
  static const Vec3d VEC_DEFAULT;
  std::map < std::string , Mat3d > mats;
  static const Mat3d MAT_DEFAULT;
  std::map < std::string , Arc3d > arcs;
  static const Arc3d ARC_DEFAULT;

 public:
  Symbols();
  ~Symbols();
  Symbols(const Symbols &copy);
  const Symbols& operator=(const Symbols &copy);
  const std::string &str(const std::string &name) const;
  void  str(const std::string &name, const std::string &value);


  float num(const std::string &name) const;
  void  num(const std::string &name, float value);

  const Mat3d &mat(const std::string &name) const;
  void  mat(const std::string &name, const Mat3d &value);

  const Vec3d &vec(const std::string &name) const;
  void  vec(const std::string &name, const Vec3d &value);

  const Arc3d &arc(const std::string &name) const;
  void  arc(const std::string &name, const Arc3d &value);

};

extern Symbols syms;
