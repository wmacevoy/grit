#include "codegenerator.hpp"
#include <fstream>
#include <sstream>

using namespace std;

namespace preprocess {

  CodeGenerator::Exception::Exception() {}
  CodeGenerator::Exception::Exception(const std::string &_message) : message(_message) {};

  CodeGenerator::NoDefaultFormatException::NoDefaultFormatException() {}
  CodeGenerator::NoDefaultFormatException::NoDefaultFormatException(const std::string &_message) : Exception(_message) {};

  CodeGenerator::NoSuchValueException::NoSuchValueException() {}
  CodeGenerator::NoSuchValueException::NoSuchValueException(const std::string &_message) : Exception(_message) {};

CodeGenerator::Registry *CodeGenerator::registry=0;

void CodeGenerator::registry_add(const std::string &name, CodeGenerator *generator)
{
  registry_get();
  (*registry)[name]=generator;
}

CodeGenerator* CodeGenerator::registry_lookup(const std::string &name)
{
  registry_get();
  Registry :: iterator i=registry->find(name);
  return (i != registry->end()) ? i->second : 0;
}

CodeGenerator::Registry *CodeGenerator::registry_get()
{
  if (registry == 0) {
    registry = new Registry();
  }
  return registry;
}

int CodeGenerator::unchecked_global_index_of(const std::string &name) {
  int index=-1;
  int count=0;
  FOREACH_GLOBAL(i) { 
    if (*i == name) { index=count; }
    ++count;
  }
  return index;
}

int CodeGenerator::global_index_of(const std::string &name) {
  int ans=unchecked_global_index_of(name);
  if (ans == -1) {
    NoSuchValueException *exception = new NoSuchValueException();
    std::ostringstream oss;
    oss << __FILE__ << " " << __LINE__ << ": " << "global " << name << " not found.";
    exception->message=oss.str();
    throw exception;
  }
  return ans;
}

int CodeGenerator::unchecked_parameter_index_of(const std::string &name) {
  int index=-1;
  int count=0;
  FOREACH_PARAMETER(i) { 
    if (*i == name) { index=count; }
    ++count;
  }
  return index;
}

int CodeGenerator::parameter_index_of(const std::string &name) {
  int ans=unchecked_parameter_index_of(name);
  if (ans == -1) {
    NoSuchValueException *exception = new NoSuchValueException();
    std::ostringstream oss;
    oss << __FILE__ << " " << __LINE__ << ": " << "parameter " << name << " not found.";
    exception->message=oss.str();
    throw exception;
  }
  return ans;
}

int CodeGenerator::unchecked_variable_index_of(const std::string &name) {
  int index=-1;
  int count=0;
  FOREACH_VARIABLE(i) { 

    if (*i == name) { index=count; }
    ++count;
  }
  return index;
}

int CodeGenerator::variable_index_of(const std::string &name) {
  int ans=unchecked_variable_index_of(name);
  if (ans == -1) {
    NoSuchValueException *exception = new NoSuchValueException();
    std::ostringstream oss;
    oss << __FILE__ << " " << __LINE__ << ": " << "variable " << name << " not found.";
    exception->message=oss.str();
    throw exception;
  }
  return ans;
}

CodeGenerator::CodeGenerator() 
{ 
  out=0; 
  configuration=0; 
  indent=0;
  default_format = 0;
}
CodeGenerator::~CodeGenerator() {}
void CodeGenerator::configure(Configuration *_configuration) {
  configuration=_configuration;
  if (default_format == 0) {
    std::string precision=flag("precision","double");
    if (precision=="double") {
      default_format = &symbolic::format_c_double;
    } else if (precision=="single") {
      default_format = &symbolic::format_c_single;
    }
  }
}

ostream& CodeGenerator::o() { 
  for (int i=0; i<indent; ++i) { (*out) << " "; }
  return (*out);
}

void CodeGenerator::include(istream &in)
{
  string line;
  while (!in.eof()) {
    getline(in,line);
    int length=line.length();
    while (length > 0) {
      int ch=line[length-1];
      if (!(ch == '\r' || ch == '\n')) break;
      --length;
    }

    int pos=0;
    for (;;) {
      if (pos >= length) break;
      if (line[pos] == '$' && ( pos+1 < length && line[pos+1]=='(')) {
	int begin=pos+2;
	int parens=1;
	pos += 2;
	while (pos < length && parens > 0) {
	  if (line[pos] == '(') ++parens;
	  if (line[pos] == ')') --parens;
	  ++pos;
	}
	(*out) << configuration->flags[line.substr(begin,pos-1)];
      } else if (line[pos] == '$' && ( pos+1 < length && line[pos+1]=='$')) {
	(*out) << "$"; ++pos;
      } else {
	(*out) << line[pos];
      }
      ++pos;
    }
    (*out) << endl;
  }
}

void CodeGenerator::include(const string &file)
{
  ifstream in(file.c_str());
  if (!!in) include(in);
}

  
std::string CodeGenerator::format(const std::string &s) const
{
  std::ostringstream oss;
  symbolic::Expression *e=symbolic::parse(s);
  oss << format(e);
  delete e;
  return oss.str();
}

symbolic::FormatResult CodeGenerator::format(const symbolic::Expression *e) const
{
  return default_format(e);
}

size_t CodeGenerator::nflags()
{
  return configuration->flags.size();
}

size_t CodeGenerator::nglobals()
{
  return configuration->globals.size();
}

size_t CodeGenerator::nparameters()
{
  return configuration->parameters.size();
}

size_t CodeGenerator::nvariables()
{
  return configuration->variables.size();
}

size_t CodeGenerator::ninitializers()
{
  return configuration->initializers.size();
}

size_t CodeGenerator::nequations()
{
  return configuration->equations.size();
}

size_t CodeGenerator::nupdates()
{
  return configuration->updates.size();
}

size_t CodeGenerator::ntests()
{
  return configuration->tests.size();
}

size_t CodeGenerator::nruns()
{
  return configuration->runs.size();
}

const std::string &CodeGenerator::flag(const std::string &flag, const std::string &default_value) 
{
  std::map < std::string , std::string > :: iterator i=
    configuration->flags.find(flag);

  if (i != configuration->flags.end()) { return i->second; }
  else return default_value;
}



}
