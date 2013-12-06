#include "configuration.hpp"
#include "utilities.hpp"
#include "symbolic.hpp"
#include "formatter.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <set>
#include <assert.h>

using namespace std;
using namespace symbolic;
using namespace utilities;

namespace preprocess {

class ConfigurationParser
{
public:
  istream &in;
  std::map < std::string , const Expression * > substitutions;
  Configuration &configuration;

  int lineno;
  bool verbose;
  string line;
  string name;
  string value;
  string section;
  string subsection;

  ~ConfigurationParser() {
    // clean up substitution map
    for (std::map < std::string , const Expression * > :: iterator i = substitutions.begin(); i!=substitutions.end(); ++i) { delete i->second; }
  }

  //
  // remove #-comment (if it is outside a quoted string)
  //
  void remove_comment(std::string &data)
  {
    int state=0;
    int pos=0;
    while (pos < data.length()) {
      char c=data[pos];
      if (state == 0) {
	if (c == '\"') { state=1; }
	else if (c == '#') { break; }
      } else if (state == 1) {
	if (c == '\"') { state=0; }
	else if (c == '\\') { state=2; }
      } else {
	state=1;
      }
      ++pos;
    }
    if (pos < data.length()) {
      data=data.substr(0,pos);
    }
  }

  void trim(string &s)
  {
    int lb=0; 
    while (lb < s.length() && (s[lb]==' ' || s[lb]=='\t')) 
      ++lb;

    if (lb > 0) s=s.substr(lb);

    int le=line.length(); 
    while (le > 0 && (s[le-1]==' ' || s[le-1]=='\t' || s[le-1]=='\r' || s[le-1]=='\n')) 
      --le;
    
    if (le < s.length()) s=s.substr(0,le);
  }

  bool readline0() 
  {
    line="";
    name="";
    value="";

    for (;;) {
      if (in.eof()) { line=""; return false; }
      line=""; 
      getline(in,line);
      ++lineno;
      remove_comment(line);
      trim(line);
      if (line.length() > 0) { split(); return true; }
    }
  }

  bool readline()
  {
    bool ans=readline0();
    if (verbose) {
      if (line.find('=') != string::npos) {
	cerr << lineno << ": '" << name << "'='" << value << "'" << endl;
      } else {
	cerr << lineno << ": " << line << endl;
      }
    }
    return ans;
  }

  void split()
  {
    int eq=line.find('=');
    if (eq == -1) { name=line; value=line; }
    else { name = line.substr(0,eq); value=line.substr(eq+1); }
    trim(name);
    trim(value);
  }

  ConfigurationParser(std::istream &_in, Configuration &_configuration)
    : in(_in), configuration(_configuration)
  { lineno=0; verbose=false; }

  void clear()
  {
    configuration.flags.clear();
    configuration.globals.clear();
    configuration.parameters.clear();
    configuration.variables.clear();
    configuration.initializers.clear();
    configuration.equations.clear();
    configuration.updates.clear();
    configuration.tests.clear();
    configuration.runs.clear();
  }

  bool read()
  {
    section="";
    subsection="";
    while (section != "[end]") {
      if (!readline()) break;
      if (line[0]=='[') {
	if ((section == "[tests]" || section == "[runs]") && line == "[case]") {
          subsection="[case]";
	  if (section == "[tests]") {
	    configuration.tests.push_back(Configuration::Case());
	  } else {
	    configuration.runs.push_back(Configuration::Case());
	  }
	} else {
	  subsection="";
	  section=line;
	}
      } else {
	if (section != "[flags]" && name != "name") {
	  apply_substitutions(value);
	}
	if (section == "[flags]") {
	  configuration.flags[name]=decode_string(value);
	} else if (section == "[substitutions]") {
	  Expression *e_sym=0;
	  if (!symbolic::parse(value,e_sym)) {
	    cerr << lineno << ": " << section << " syntax error in expression: " << value << endl;
	    delete e_sym;
	    return false;
	  }
	  substitutions[name]=e_sym;
	} else if (section == "[globals]") {
	  configuration.globals.push_back(name);
	} else if (section == "[parameters]") {
	  configuration.parameters.push_back(name);
	} else if (section == "[variables]") {
	  configuration.variables.push_back(name);
	} else if (section == "[initializers]") {
	  configuration.initializers.push_back(pair<string,string>(name,value));
	} else if (section == "[equations]") {
	  configuration.equations.push_back(pair<string,string>(name,value));
	} else if (section == "[updates]") {
	  configuration.updates.push_back(pair<string,string>(name,value));
	} else if (section == "[tests]" && subsection == "[case]") {
	  if (name == "name") configuration.tests.rbegin()->name=decode_string(value);
	  else configuration.tests.rbegin()->setup.push_back(pair<string,double>(name,evaluate(parse(value))));
	} else if (section == "[runs]" && subsection == "[case]") {
	  if (name == "name") configuration.runs.rbegin()->name=decode_string(value);
	  else configuration.runs.rbegin()->setup.push_back(pair<string,double>(name,evaluate(parse(value))));
	} else {
	  cerr << lineno << ": syntax error." << endl;
	  return false;
	}
      }
    }
    return true;
  }

  void apply_substitutions(std::string &e_str)
  {
    Expression *e_sym=0;
    if (symbolic::parse(e_str,e_sym)) {
      Expression *e_sub=symbolic::substitute(substitutions, e_sym);
      Expression *e_sim=symbolic::simplify(e_sub);
      std::ostringstream oss;
      oss << format_ini(e_sim);
      e_str=oss.str();
      delete e_sim;
      delete e_sub;
      delete e_sym;
    }
  }

  bool add_symbols(std::set < std::string > &to , std::list < std::string > & names) {
    bool ok=true;
    for (std::list < std::string > :: iterator i = names.begin(); i!=names.end(); ++i) {
      string name=*i;
      if (to.find(name) != to.end()) {
	cerr << "duplicate symbol: " << name << endl;
	ok=false;
      }
      to.insert(name);
    }
    return ok;
  }

  bool check_expression(const std::string &section,std::string e_str, std::set < std::string > &vars)
  {
    bool ok=true;
    Expression *e_sym=0;
    if (!parse(e_str,e_sym)) {
      cerr << section << ": syntax error in expression: " << e_str << endl;
      delete e_sym;
      return false;
    }

    std::set<std::string> syms;
    if (e_sym) e_sym->symbols(syms);
    for (std::set<std::string>::iterator i=syms.begin(); i!=syms.end(); ++i) {
      if (vars.find(*i) == vars.end()) {
	cerr << section << ": expression " << format_ini(e_sym) << " uses symbol " << *i << ", which is either not allowed or not declared." << endl;
	ok=false;
      }
    }

    if (!ok) {
      cerr << "allowed:";
      for (set<string>::iterator i=vars.begin(); i!=vars.end(); ++i) {
	cerr << " " << *i;
      }
      cerr << endl;
    }

    delete e_sym;

    return ok;
  }

  bool check()
  {
    bool ok=true;
    
    std::set<std::string> all_symbols;
    std::set<std::string> globals;
    std::set<std::string> parameters;
    std::set<std::string> variables;

    all_symbols.clear();
    if (!add_symbols(all_symbols,configuration.globals)) ok=false;
    if (!add_symbols(all_symbols,configuration.parameters)) ok=false;
    if (!add_symbols(all_symbols,configuration.variables)) ok=false;

    add_symbols(globals,configuration.globals);
    add_symbols(parameters,configuration.parameters);
    add_symbols(variables,configuration.variables);

    {
      // check initializers
      std::set < std::string > uninitialized=variables;
      std::set < std::string > ok_syms;
      ok_syms.insert(globals.begin(),globals.end());
      ok_syms.insert(parameters.begin(),parameters.end());
      
      CONFIGURATION_FOREACH_CONST_INITIALIZER(configuration,i) {
	if (variables.find(i->first) == variables.end()) {
	  cerr << "[initializers]: attempt to initialize undeclared variable " << i->first << endl;
	  ok=false;
	} else if (uninitialized.find(i->first) == uninitialized.end()) {
	  cerr << "[initializers]: attempt to reinitialize variable " << i->first << endl;
	  ok=false;
	}
	uninitialized.erase(i->first);
	
	if (!check_expression("[initializers]",i->second, ok_syms)) ok=false;
	
      }
      if (!uninitialized.empty()) {
	cerr << "[initializers]: uninitialized variables:";
	for (std::set < std::string > :: iterator i = uninitialized.begin(); i!=uninitialized.end(); ++i) {
	  cerr << " " << *i;
	}
	cerr << endl;
	ok=false;
      }
    }

    {
      // check equations
      std::set < std::string > res;
      std::set < std::string > ok_syms;
      ok_syms.insert(globals.begin(),globals.end());
      ok_syms.insert(parameters.begin(),parameters.end());
      ok_syms.insert(variables.begin(),variables.end());
      
      CONFIGURATION_FOREACH_CONST_EQUATION(configuration,i) {
	if (res.find(i->first) != res.end()) {
	  cerr << "[equations]: attempt to redefine residual " << i->first << endl;
	  ok=false;
	} else if (all_symbols.find(i->first) != all_symbols.end()) {
	  cerr << "[equations]: attempt to use global, parameter, or variable as residual: " << i->first << endl;
	  ok=false;
	}
	res.insert(i->first);
	
	if (!check_expression("[equations]",i->second, ok_syms)) ok=false;
      }
      if (res.size() != variables.size()) {
	cerr << "warning [equations]: undetermined or overdetermined system: " << res.size() << " variables in " << variables.size() << " unknowns." << endl;
	//	ok=false;
	}
    }

    // check for formulation of differential
    {
      CONFIGURATION_FOREACH_CONST_EQUATION(configuration,i) {
	Expression *yi=0;
	assert(parse(i->second,yi));
	CONFIGURATION_FOREACH_CONST_VARIABLE(configuration,j) {
	  try {
	    Expression *dyi_dxj=differentiate(yi,*j);
	  } catch (Expression::UnsupportedOperation &exception) {
	    cerr << "[equations]: cannot formulate jacobian because varible " << *j  << " appears as a power in " << i->first << "." << endl;
	    ok=false;
	  }
	}
	delete yi;
      }
    }

    {
      // check updates
      std::set < std::string > unupdated=parameters;
      std::set < std::string > ok_syms;
      ok_syms.insert(globals.begin(),globals.end());
      ok_syms.insert(parameters.begin(),parameters.end());
      ok_syms.insert(variables.begin(),variables.end());
      
      CONFIGURATION_FOREACH_CONST_UPDATE(configuration,i) {
	if (parameters.find(i->first) == parameters.end()) {
	  cerr << "[updates]: attempt to update undeclared parameter " << i->first << endl;
	  ok=false;
	} else if (unupdated.find(i->first) == unupdated.end()) {
	  cerr << "[updates]: attempt to reupdate parameter " << i->first << endl;
	  ok=false;
	}
	unupdated.erase(i->first);
	
	if (!check_expression("[updates]",i->second, ok_syms)) ok=false;
	
      }
    }

    {
      // check tests
      int count=1;

      CONFIGURATION_FOREACH_CONST_TEST(configuration,i) {
	const Configuration::Case &c=*i;

	std::set < std::string > ok_syms;
	ok_syms.insert(globals.begin(),globals.end());
	ok_syms.insert(parameters.begin(),parameters.end());
	ok_syms.insert(variables.begin(),variables.end());
	std::set < std::string > unset=ok_syms;

	for (std::list < std::pair < std::string , double > > :: const_iterator j = c.setup.begin(); j != c.setup.end(); ++j) {
	  if (ok_syms.find(j->first) == ok_syms.end()) {
	    cerr << "[tests][case " << count << "/\"" << c.name << "\"]: " << "attempt to set undefined value " << j->first << endl;
	    ok=false;
	  } else if (unset.find(j->first) == unset.end()) {
	    cerr << "[tests][case " << count << "/\"" << c.name << "\"]: " << "attempt to redefine value " << j->first << endl;
	    ok=false;
	  }
	  unset.erase(j->first);
	}
	
	if (!unset.empty()) {
	  cerr << "[tests][case " << count << "/\"" << c.name << "\"]: " << "unset values:";
	  for (std::set < std::string > :: iterator j = unset.begin(); j!=unset.end(); ++j) {
	    cerr << " " << *j;
	  }
	  cerr << endl;
	  ok=false;
	}
      }
    }

    {
      // check runs
      int count=1;

      CONFIGURATION_FOREACH_CONST_RUN(configuration,i) {
	const Configuration::Case &c=*i;

	std::set < std::string > ok_syms;
	ok_syms.insert(globals.begin(),globals.end());
	ok_syms.insert(parameters.begin(),parameters.end());
	std::set < std::string > unset=ok_syms;

	for (std::list < std::pair < std::string , double > > :: const_iterator j = c.setup.begin(); j != c.setup.end(); ++j) {
	  if (ok_syms.find(j->first) == ok_syms.end()) {
	    cerr << "[runs][case " << count << "/\"" << c.name << "\"]: " << "attempt to set undefined value " << j->first << endl;
	    ok=false;
	  } else if (unset.find(j->first) == unset.end()) {
	    cerr << "[runs][case " << count << "/\"" << c.name << "\"]: " << "attempt to redefine value " << j->first << endl;
	    ok=false;
	  }
	  unset.erase(j->first);
	}
	
	if (!unset.empty()) {
	  cerr << "[runs][case " << count << "/\"" << c.name << "\"]: " << "unset values:";
	  for (std::set < std::string > :: iterator j = unset.begin(); j!=unset.end(); ++j) {
	    cerr << " " << *j;
	  }
	  cerr << endl;
	  ok=false;
	}
      }
    }

    return ok;
  }


};

bool configuration_parse(const std::string &file, Configuration &configuration)
{
  bool ok=false;
  try {

#ifdef SYMBOLIC_OUT_MODE
    int save=symbolic::Expression::outMode;
    symbolic::Expression::outMode=symbolic::Expression::OUT_MODE_INI;  
#endif
    ifstream in(file.c_str());
    if (!in) { 
      cerr << "could not read input file '" << file << "'" << endl;
      return false;
    }

    ConfigurationParser parser(in,configuration);
    parser.clear();
    ok=parser.read();
    if (ok) {
      ok=parser.check();
    }
    if (!ok) parser.clear();
    
#ifdef SYMBOLIC_OUT_MODE
    symbolic::Expression::outMode=save;
#endif
  } catch (Expression::UnsupportedOperation &exception) {
    cerr << "unsupported operation: " << exception.message << endl;
    ok=false;
  }

  return ok;
}


std::ostream& operator<< (std::ostream &out, const Configuration &configuration)
{
  out << "[flags]" << endl;
  CONFIGURATION_FOREACH_CONST_FLAG(configuration,i) {
    out << i->first << "=" << encode_string(i->second) << endl;
  }

  out << "[globals]" << endl;
  CONFIGURATION_FOREACH_CONST_GLOBAL(configuration,i) {
    out << *i << endl;
  }

  out << "[parameters]" << endl;
  CONFIGURATION_FOREACH_CONST_PARAMETER(configuration,i) {
    out << *i << endl;
  }

  out << "[variables]" << endl;
  CONFIGURATION_FOREACH_CONST_VARIABLE(configuration,i) {
    out << *i << endl;
  }

  out << "[initializers]" << endl;
  CONFIGURATION_FOREACH_CONST_INITIALIZER(configuration,i) {
    out << i->first << "=" << i->second << endl;
  }

  out << "[equations]" << endl;
  CONFIGURATION_FOREACH_CONST_EQUATION(configuration,i) {
    out << i->first << "=" << i->second << endl;
  }

  out << "[updates]" << endl;
  CONFIGURATION_FOREACH_CONST_UPDATE(configuration,i) {
    out << i->first << "=" << i->second << endl;
  }

  out << "[tests]" << endl;
  { int count=1; CONFIGURATION_FOREACH_CONST_TEST(configuration,i) {
      const Configuration::Case &c=*i;
      out << "[case] # " << count << endl;
      out << "name=" << encode_string(c.name) << endl;
      for (std::list < std::pair < std::string , double > > :: const_iterator j = c.setup.begin(); j!=c.setup.end(); ++j) {
	out << j->first << "=" << std::setprecision(15) << j->second << endl;
      }
      ++count;
    }}

  out << "[runs]" << endl;
  { int count=1; CONFIGURATION_FOREACH_CONST_RUN(configuration,i) {
      const Configuration::Case &c=*i;
      out << "[case] # " << count << endl;
      out << "name=" << encode_string(c.name) << endl;
      for (std::list < std::pair < std::string , double > > :: const_iterator j = c.setup.begin(); j!=c.setup.end(); ++j) {
	out << j->first << "=" << std::setprecision(15) << j->second << endl;
      }
      ++count;
    }}

  out << "[end]" << endl;

  return out;
}

}
