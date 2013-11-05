#include "symbolic.hpp"
#include "formatter.hpp"
#include <assert.h>
#include <map>
#include <string>

using namespace std;
using namespace symbolic;

int main(int argc, char *argv[])
{
  std::map < std::string , const symbolic::Expression * > subs;
  for (int argi=1; argi<argc-1; ++argi) {
    std::string eqn=argv[argi];
    int eq=eqn.find('=');
    assert(eq != std::string::npos);
    std::string name=eqn.substr(0,eq);
    std::string value=eqn.substr(eq+1);
    subs[name]=symbolic::parse(value);
  }

  symbolic::Expression *e = symbolic::parse(argv[argc-1]);
  symbolic::Expression *f = symbolic::substitute(subs,e);

  cout << format_ini(e) << " -> " << format_ini(f) << endl;

  return 0;
};
