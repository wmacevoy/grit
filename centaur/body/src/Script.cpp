#include "Script.h"
#include "StdCapture.h"
#include "CSVSplit.h"
#include <Python.h>
#include <iostream>
#include <vector>
#include <string>

Script::Error::Error(const std::string &message_)
  : message(message_)
{
}

Script::Error::Error()
{
  StdCapture capture;

  capture.BeginCapture();
  PyErr_PrintEx(0);
  capture.EndCapture();
  message=capture.GetCapture();
}

std::ostream& operator<< (std::ostream &out, const Script::Error &e)
{
  out << "script error: " << e.message << std::endl;
  return out;
}

std::ostream& operator<< (std::ostream &out, const Script::Object &o)
{
  StdCapture capture;

  capture.BeginCapture();
  PyObject_Print(o.py,stdout,0);
  capture.EndCapture();
  out << capture.GetCapture();
  return out;
}

Script::Object::Object(PyObject *py_)
  : py(py_)
{
}

Script::Object::Object(const char *s)
  : py(PyString_FromString(s))
{
}

Script::Object::Object(const std::string &s)
  : py(PyString_FromString(s.c_str()))
{
}

Script::Object::Object(const Script::Object &copy)
  : py(copy.py)
{
  if (py != NULL) Py_INCREF(py);
}

void Script::Object::own()
{
  if (py != NULL) Py_INCREF(py);
}

PyObject *Script::Object::steal()
{
  PyObject *ans = py;
  py = NULL;
  return ans;
}

Script::Object::operator bool() 
{ 
  return py != NULL; 
}

void Script::Object::print() const
{
  PyObject_Print(py,stdout,0);
}

Script::Object::~Object()
{
  if (py != NULL) Py_DECREF(py);
}

void Script::addPaths(const std::string &directories,char sep)
{
  std::vector < std::string > path;
  CSVSplit(directories,path,sep);
  for (size_t i=0; i<path.size(); ++i) {
    addPath(path[i]);
  }
}

void Script::addPath(const std::string &directory)
{
  Object path(PySys_GetObject("path")); path.own();
  Object pyDirectory(directory);
  PyList_Insert(path.py,0,pyDirectory.py);
}

Script::Object Script::module(const std::string &name)
{
  Object pName(name);
  Object ans(PyModule_GetDict(pName.py));
  if (!ans) {
    throw Error("module '"+name +"' missing.");
  }
  ans.own();
  return ans;
}


Script::Object Script::import(const std::string &name)
{
  Object ans(PyImport_Import(Object(name).py));
  if (!ans) {
    throw Error("import '"+name +"' failed.");
  }
  return ans;
}

Script::Script(int argc, char **argv)
{
  Py_Initialize();
  Py_SetProgramName(argv[0]);
  globals = import("__main__");
  locals = Object(PyDict_New());
}

Script::Object Script::compile(const std::string &e)
{
  return Object(Py_CompileString(e.c_str(),"script",Py_eval_input));
}

Script::Object Script::eval(Script::Object compiled)
{
  return Object(PyEval_EvalCode((PyCodeObject*)compiled.py,globals.py,locals.py));
}

Script::Object Script::eval(const std::string &e)
{
  return eval(compile(e));
}

void Script::run(const std::string &s)
{
  PyRun_SimpleString(s.c_str());
}


#if 0
Script::Object Script::set()
{
  if (pModule != NULL) {
    pFunc = PyObject_GetAttrString(pModule, argv[2]);
    /* pFunc is a new reference */

    if (pFunc && PyCallable_Check(pFunc)) {
      pArgs = PyTuple_New(argc - 3);
      for (i = 0; i < argc - 3; ++i) {
	pValue = PyInt_FromLong(atoi(argv[i + 3]));
	if (!pValue) {
	  Py_DECREF(pArgs);
	  Py_DECREF(pModule);
	  fprintf(stderr, "Cannot convert argument\n");
	  return 1;
	}
	/* pValue reference stolen here: */
	PyTuple_SetItem(pArgs, i, pValue);
      }
      pValue = PyObject_CallObject(pFunc, pArgs);
      Py_DECREF(pArgs);
      if (pValue != NULL) {
	printf("Result of call: %ld\n", PyInt_AsLong(pValue));
	Py_DECREF(pValue);
      }
      else {
	Py_DECREF(pFunc);
	Py_DECREF(pModule);
	PyErr_Print();
	fprintf(stderr,"Call failed\n");
	return 1;
      }
    }
    else {
      if (PyErr_Occurred())
	PyErr_Print();
      fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
    }
    Py_XDECREF(pFunc);
    Py_DECREF(pModule);
  }
  else {
    PyErr_Print();
    fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
    return 1;
  }
  return 0;
}

#endif


Script::~Script()
{
  Py_Finalize();
}
