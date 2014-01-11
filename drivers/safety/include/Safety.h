#pragma once

#include <memory>

class Safety
{
 public:
  virtual bool stale() const=0;
  virtual bool safe() const=0;
  virtual bool warn() const=0;
  virtual void safe(bool value)=0;
  virtual ~Safety();
};

typedef std::shared_ptr <Safety> SafetySP;
