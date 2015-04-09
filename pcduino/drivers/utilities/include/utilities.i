%module utilities
%include "std_string.i"
%include "std_shared_ptr.i"
%shared_ptr(Configure)
%{
#define SWIG_FILE_WITH_INIT
#include <iostream>
#include <fstream>
#include <memory>
#include "Configure.h"
#include "now.h"
static std::istream& cin() { return std::cin; }
static std::ostream& cout() { return std::cout; }
%}

namespace std {

class istream {
 public:
  virtual int get()=0;
  virtual ~istream();
};

class ostream {
 public:
  virtual ostream& put(char c)=0;
  virtual ~ostream();
};

class ifstream:public istream {
public:
   ifstream(const char *fn);
   int get();
   ~ifstream();
};

class ofstream:public ostream{
public:
   ofstream(const char *fn);
   ostream& put(char c);
   ~ofstream();
};
} // namespace std

std::istream& cin();
std::ostream& cout();

%include "Configure.h"
%include "now.h"
