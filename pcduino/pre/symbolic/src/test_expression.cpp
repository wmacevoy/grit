#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <assert.h>
#include <map>
#include <math.h>
#include "symbolic.hpp"
#include "coptgen.hpp"
#include "formatter.hpp"

using namespace std;
using namespace symbolic;

int main(int argc, const char *argv[])
{
  Expression *e_sym=0;
  map < string , double > variables;

  for (int argi=1; argi<argc; ++argi)
  {
    try {
      string arg=argv[argi];
      if (arg.find('=') != -1) {
	string name=arg.substr(0,arg.find('='));
	string value=arg.substr(arg.find('=')+1);
	variables[name]=atof(value.c_str());
	continue;
      }

      if (arg == "format_c_double") {
	cout << format_c_double(e_sym) << endl;
	continue;
      }

      if (arg == "format_c_single") {
	cout << format_c_single(e_sym) << endl;
	continue;
      }

      if (arg == "format_scilab") {
	cout << format_scilab(e_sym) << endl;
	continue;
      }

      if (arg == "format_ini") {
	cout << format_ini(e_sym) << endl;
	continue;
      }

      if (arg == "format_latex") {
	cout << format_latex(e_sym) << endl;
	continue;
      }

#ifdef SYMBOLIC_OUT_MODE
      if (arg == "mode") {
	string mode=argv[++argi];
	if (mode == "csingle") Expression::outMode=Expression::OUT_MODE_C_SINGLE;
	else if (mode == "cdouble") Expression::outMode=Expression::OUT_MODE_C_DOUBLE;
	else if (mode == "sci") Expression::outMode=Expression::OUT_MODE_SCI;
	else if (mode == "latex") Expression::outMode=Expression::OUT_MODE_LATEX;
	else if (mode == "ini") Expression::outMode=Expression::OUT_MODE_INI;
	else {
	  cout << "unknown mode" << endl;
	  exit(1);
	}
        continue;
      }
#endif

      if (arg == "simplify") {
	Expression *new_e_sym=simplify(e_sym);
	delete e_sym;
	e_sym = new_e_sym;
	continue;
      }
      if (arg == "diff") {
	string var=argv[++argi];
	Expression *de=differentiate(e_sym,var);
	delete e_sym;
	e_sym=de;
	continue;
      }
#ifdef SYMBOLIC_PRINT
      if (arg == "print") {
	cout << e_sym << endl;
	continue;
      }
#endif
      if (arg == "coptgen") {
	COptGen gen;
	gen.assign("ans",e_sym);
	gen.print(std::cout);
	std::cout << std::endl;

	continue;
      }
      if (arg == "eval") {
	cout << format_plain(e_sym) << " at [";
	for (map<string,double>::iterator i=variables.begin(); i!=variables.end(); ++i) {
	  if (i != variables.begin()) { cout << ","; }
	  cout << i->first << "=" << setprecision(15) << i->second;
	}
	cout << "]=" << setprecision(15) << evaluate(e_sym,variables) << endl;
	continue;
      }
      if (arg == "expect") {
	double sym_value=evaluate(e_sym,variables);
	double exp_value=atof(argv[++argi]);
	if (fabs(sym_value-exp_value) >= 1e-10) {
	  cout << "expected " << setprecision(15) << exp_value << ", but got " << setprecision(15) << sym_value << endl;
	  assert(false);
	}
	continue;
      }
      if (arg == "compare") {
	Expression *exp1=e_sym;
	Expression *exp2=0;
	int ans=0;
	assert(symbolic::parse(argv[++argi],exp2));
	ans=compare(exp1,exp2);
	if (0!=ans) {
	  cout << format_ini(exp1) << " is not compare=0 to " << format_ini(exp2) << endl;
	  cout << "compare = " << ans << endl;
	  assert(false);
	}
	delete exp2;
	continue;
      }
      if (arg == "--help") {
	cout << "usage: " << argv[0] << " [--help] [<expr>|mode <sci,ini,csingle,cdouble>|diff var|name=value|eval|expect <num>|print|simplify]..." << endl;
	cout << "  mode changes the output syntax format." << endl;
	cout << "  diff replaces the currrent expresion with its derivative." << endl;
	cout << "  eval evaluates the current expression with the current variable assignment." << endl;
	cout << "  expect is like eval, but with a known value given to 1e-10 tolerance." << endl;
	cout << "  print prints the current (symbolic) expression." << endl;
      }
      delete e_sym;
      variables.clear();
      e_sym=0;
      assert(symbolic::parse(arg,e_sym));
    } catch (symbolic::Expression::UnsupportedOperation &exception) {
      cout << "exception: " << exception.message << endl;
      exit(1);
    }
  }

  return 0;
}
