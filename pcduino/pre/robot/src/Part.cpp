#include "Part.h"

Mat Part::world() const
{
  Mat m=frame;
  
  for (const Part *p=parent; p != 0; p = p->parent) {
    m = (p->frame)*m;
  }
  return m;
}

Part::Part(Part *parent_, const std::string &name_)
  : parent(parent_), name(name_)
{
  frame = eye(4,4);
  if (parent != 0) {
    parent->add(this);
  }
}

Part::~Part() 
{
}

void Part::add(Part *child)
{
  child->parent=this;
  Parts::iterator  i=parts.find(child->name);
  if (i != parts.end()) {
    if (&*i->second  == child) return ;
  }
  parts[child->name]=PartSP(child);
}

