#include "Symbols.h"
#include "Lock.h"

Symbols syms;

const std::string Symbols::STR_DEFAULT;
const float Symbols::NUM_DEFAULT(0.0f);
const Vec3d Symbols::VEC_DEFAULT;
const Mat3d Symbols::MAT_DEFAULT;
const Arc3d Symbols::ARC_DEFAULT;

Symbols::Symbols()
{
}

Symbols::~Symbols()
{
}

Symbols::Symbols(const Symbols &copy)
{
  Lock lock(access);
  {
    std::cout << "copy()" << std::endl;
    Lock lock(copy.access);
    nums=copy.nums;
    vecs=copy.vecs;
    mats=copy.mats;
    arcs=copy.arcs;
  }
}

const Symbols& Symbols::operator=(const Symbols &copy)
{
  if (this != &copy) {
    std::cout << "=()" << std::endl;
    Lock lock(access);
    {
      Lock lock(copy.access);
      nums=copy.nums;
      vecs=copy.vecs;
      mats=copy.mats;
      arcs=copy.arcs;
    }
  }
  return *this;
}

const std::string &Symbols::str(const std::string &name) const
{
  Lock lock(access);
  std::map<std::string,std::string>::const_iterator i=strs.find(name);
  if (i != strs.end()) return i->second;
  else {
    return STR_DEFAULT;
  }
}

void  Symbols::str(const std::string &name, const std::string &value)
{
  Lock lock(access);
  strs[name]=value;
}

float Symbols::num(const std::string &name) const
{
  Lock lock(access);
  float ans=0;
  std::map<std::string,float>::const_iterator i=nums.find(name);
  if (i != nums.end()) ans=i->second;
  else {
    ans=NUM_DEFAULT;
  }

  return ans;
}

void  Symbols::num(const std::string &name, float value)
{
  Lock lock(access);
  nums[name]=value;
}

const Mat3d &Symbols::mat(const std::string &name) const
{
  Lock lock(access);
  std::map<std::string,Mat3d>::const_iterator i=mats.find(name);
  if (i != mats.end()) return i->second;
  else {
    return MAT_DEFAULT;
  }
}
void  Symbols::mat(const std::string &name, const Mat3d &value)
{
  Lock lock(access);
  mats[name]=value;
}

const Vec3d &Symbols::vec(const std::string &name) const
{
  Lock lock(access);
  std::map<std::string,Vec3d>::const_iterator i=vecs.find(name);
  if (i != vecs.end()) return i->second;
  else {
    return VEC_DEFAULT;
  }
}

void  Symbols::vec(const std::string &name, const Vec3d &value)
{
  Lock lock(access);
  vecs[name]=value;
}

const Arc3d &Symbols::arc(const std::string &name) const
{
  Lock lock(access);
  std::map<std::string,Arc3d>::const_iterator i=arcs.find(name);
  if (i != arcs.end()) return i->second;
  else {
    return ARC_DEFAULT;
  }
}

void  Symbols::arc(const std::string &name, const Arc3d &value)
{
  Lock lock(access);
  arcs[name]=value;
}
