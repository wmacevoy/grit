#include <iostream>
#include <string>
#include "symbolic.hpp"
#include "formatter.hpp"

using namespace std;
using namespace symbolic;

const char *tests[] =
  {
    "0",
    "1",
    "1.1",
    ".1"
    "1e10",
    "-1e10",
    "+1e10",
    "1e-10",
    "-1e-10",
    "+1e-10",
    "1e+10",
    "-1e+10",
    "+1e+10",
    "x",
    "x+y",
    "x+y+z",
    "x*y",
    "x*y*z",
    "x*y*z+x+y+z",
    "x*y*0+3+5+7",
    "3*x^(k)*y^(0.78)-x*y+5",
    0,
  };

int main()
{
  for (int i=0; tests[i]!=0; ++i)
  {
    string e_str=tests[i];
    Expression *e_sym=0;
    bool ok=symbolic::parse(e_str,e_sym);
    cout << (ok ? "ok" : "fail") 
	 << ": parsed '" << e_str 
	 << "' as " << format_plain(e_sym) << endl;
  }

  return 0;
}
