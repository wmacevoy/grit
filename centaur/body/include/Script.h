#pragma once

#include <string>
#include <Python.h>


class Script
{
 public:
  class Error {
  public:
    std::string message;
    Error();
    Error(const std::string &message_);
  };

  class Object {
  public:
    PyObject *py;
    Object(PyObject *py_=NULL);
    Object(const char *s);
    Object(const std::string &s);
    Object(const Object &copy);
    void own(); // own a borrowed reference
    PyObject* steal(); // steal (the reference is stolen and no longer tracked)
    ~Object();
    operator bool();
    void print() const;
  };

  
  Object import(const std::string &name);
  Object module(const std::string &name);
  void addPath(const std::string &directory);
  void addPaths(const std::string &directories, char sep=':');
  Object compile(const std::string &e);
  Object eval(Object compiled);
  Object eval(const std::string &e);
  
  Script(int argc, char **argv);
  ~Script();

  Object globals;
  Object locals;
};

std::ostream& operator<< (std::ostream &out, const Script::Object &o);
std::ostream& operator<< (std::ostream &out, const Script::Error &e);
