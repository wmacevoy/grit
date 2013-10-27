#include "symbolic.hpp"
#include "formatter.hpp"
#include "preprocess.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <map>
#include <list>
#include <set>
#include <string>
#include <fstream>
#include <math.h>
#include <typeinfo>

using namespace std;
using namespace symbolic;

namespace preprocess {

  class LogTransform : public CodeGenerator 
  {
  public:

    map<string,string> vars2logs;
    map<string,string> logs2vars;
  
    void configure(Configuration *_configuration) 
    {
      CodeGenerator::configure(_configuration);
    
      string log_ini_file = flag("log_ini_file");
      if (log_ini_file == "") {
	log_ini_file=flag("file");
	if (log_ini_file.rfind('.') != -1) {
	  log_ini_file=log_ini_file.substr(0,log_ini_file.rfind('.'));
	}
	log_ini_file += "_log.ini";
      }
    }
  
    void subs(Expression **e)
    {
      if (*e == 0) return;

      if (typeid(**e) == typeid(NaturalLog)) {
	NaturalLog *ln = (NaturalLog*) *e;
	subs(&(ln->arg));
      } else if (typeid(**e) == typeid(Product)) {
	Product *p = (Product*) *e;
	for (std::vector<Expression*> :: iterator i = p->parts.begin(); i != p->parts.end(); ++i) {
	  subs(&(*i));
	}
      } else if (typeid(**e) == typeid(Sum)) {
	Sum *s = (Sum*) *e;
	for (std::vector<Expression*> :: iterator i = s->parts.begin(); i != s->parts.end(); ++i) {
	  subs(&(*i));
	}
      } else if (typeid(**e) == typeid(Power)) {
	Power *p = (Power*) *e;
	subs(&(p->base));
	subs(&(p->power));
      } else if (typeid(**e) == typeid(Variable)) {
	Variable *v = (Variable*) *e;
	if (is_variable(v->name)) {
	  *e = new Power(new Variable("%e"),new Variable(vars2logs[v->name]));
	  delete v;    
	}
      } else if (typeid(**e) == typeid(Constant)) {
	/* null */
      } else {
	std::ostringstream oss;
	oss << "cannot log-transform " << format_ini(*e);
	Expression::UnsupportedEvaluation exception;
	exception.message = oss.str();
	throw exception;
      }
    }

    Expression *translate(Expression *e)
    {
      Expression *unsimplified_ans = clone(e);
      if (unsimplified_ans != 0) {
	subs(&unsimplified_ans);
      }
      Expression *ans = simplify(unsimplified_ans);
      delete unsimplified_ans;
      return ans;
    }

    void transform_expression(string &e_str)
    {
      Expression *e_sym = symbolic::parse(e_str);
      Expression *x_sym = translate(e_sym);
  
      std::ostringstream oss;
      oss << format_ini(x_sym);
      e_str = oss.str();
      delete e_sym;
      delete x_sym;
    }

    string tmp;


    const std::string &log_var(const string &var)
    {
      if (var.length() > 0 && var[0] == '{') {
	tmp="{log(";
	tmp += var.substr(1,var.length()-2);
	tmp += ")}";
      } else {
	tmp = "log_";
	tmp += var;
      }

      return tmp;
    }


    bool is_variable(const std::string &x)
    {
      return (vars2logs.find(x) != vars2logs.end());
    }

    const std::string& log(const std::string &x)
    {
      tmp="log(";
      tmp += x;
      tmp += ")";

      return tmp;
    }

    void transform_initializer(std::string &var, std::string &rhs)
    {
      var=vars2logs[var];
      rhs=log(rhs);
    }

    void transform_equation(std::string &lhs, std::string &rhs)
    {
      transform_expression(rhs);
    }

    void transform_update(std::string &lhs, std::string &rhs)
    {
      transform_expression(rhs);
    }


    void transform()
    {
      vars2logs.clear();
      logs2vars.clear();
      FOREACH_VARIABLE(k) 
      { 
	string ln_var=log_var(*k);
	vars2logs[*k]=ln_var;
	logs2vars[ln_var]=*k;
      }
  
      FOREACH_VARIABLE(k) *k = vars2logs[*k];
      FOREACH_INITIALIZER(k) transform_initializer(k->first,k->second);
      FOREACH_EQUATION(k) transform_equation(k->first,k->second); 
      FOREACH_UPDATE(k) transform_update(k->first,k->second);

      FOREACH_TEST(k) 
      {
	Configuration::Case &c = *k;
	for (std::list < std::pair < std::string , double > > :: iterator i=c.setup.begin(); i!=c.setup.end(); ++i) {
	  if (is_variable(i->first)) {
	    i->first=log_var(i->first);
	    i->second = ::log(i->second);
	  }

	}
      }

    }

    void generate()
    {
#ifdef SYMBOLIC_OUT_MODE
      Expression::outMode=Expression::OUT_MODE_INI;
#endif
      transform();
      if (flag("ini_log_file") != "") {
	ofstream ini_log_file(flag("ini_log_file").c_str());
	ini_log_file << *configuration;
      }
    }

  };

  class LogTransformRegister {
  public:
    LogTransformRegister()
    {
      LogTransform *generator = new LogTransform();
      CodeGenerator::registry_add("log",generator);
    }
  };

  static const LogTransformRegister LOG_TRANSFORM_REGISTER;


}

