#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <map>
#include <string>
#include <iostream>
#include <assert.h>
#include "configuration.hpp"
#include "symbolic.hpp"
#include "formatter.hpp"

namespace preprocess {

class CodeGenerator 
{
public:
  typedef std::map < std::string , CodeGenerator* > Registry;
private:
  static Registry *registry;
public:
  static void registry_add(const std::string &name, CodeGenerator *generator);
  static CodeGenerator* registry_lookup(const std::string &name);
  static Registry *registry_get();
  
  std::ostream *out;
  int indent;
  Configuration *configuration;
  virtual void configure(Configuration *_configuration);
  virtual std::ostream &o();
  virtual void include(std::istream &in);
  virtual void include(const std::string &file);
  virtual void generate()=0;

  const symbolic::Format *default_format;
  std::string format(const std::string &s) const;
  symbolic::FormatResult format(const symbolic::Expression *e) const;
  class Exception { 
  public:
    std::string message; 
    Exception();
    Exception(const std::string &_message);
  };

  class NoDefaultFormatException : public Exception {
  public:
    NoDefaultFormatException();
    NoDefaultFormatException(const std::string &_message);
  };

  class NoSuchValueException : public Exception {
  public:
    NoSuchValueException();
    NoSuchValueException(const std::string &_message);
  };

  CodeGenerator();
  virtual ~CodeGenerator();

  int unchecked_global_index_of(const std::string &name);
  int global_index_of(const std::string &name);
  int unchecked_parameter_index_of(const std::string &name);
  int parameter_index_of(const std::string &name);
  int unchecked_variable_index_of(const std::string &name);
  int variable_index_of(const std::string &name);

  size_t nflags();
  size_t nglobals();
  size_t nparameters();
  size_t nvariables();
  size_t ninitializers();
  size_t nequations();
  size_t nupdates();
  size_t ntests();
  size_t nruns();

  const std::string &flag(const std::string &flag, const std::string &default_value="");
};

//
// handy defines when implementing a code generator
//
#define FOREACH_FLAG(INDEX) CONFIGURATION_FOREACH_FLAG(*(this->configuration),INDEX)
#define FOREACH_GLOBAL(INDEX) CONFIGURATION_FOREACH_GLOBAL(*(this->configuration),INDEX)
#define FOREACH_PARAMETER(INDEX) CONFIGURATION_FOREACH_PARAMETER(*(this->configuration),INDEX)
#define FOREACH_VARIABLE(INDEX) CONFIGURATION_FOREACH_VARIABLE(*(this->configuration),INDEX)
#define FOREACH_INITIALIZER(INDEX) CONFIGURATION_FOREACH_INITIALIZER(*(this->configuration),INDEX)
#define FOREACH_EQUATION(INDEX) CONFIGURATION_FOREACH_EQUATION(*(this->configuration),INDEX)
#define FOREACH_UPDATE(INDEX) CONFIGURATION_FOREACH_UPDATE(*(this->configuration),INDEX)
#define FOREACH_TEST(INDEX) CONFIGURATION_FOREACH_TEST(*(this->configuration),INDEX)
#define FOREACH_RUN(INDEX) CONFIGURATION_FOREACH_RUN(*(this->configuration),INDEX)

}

#endif
