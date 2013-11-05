#include "symbolic.hpp"
#include "coptgen.hpp"
#include "formatter.hpp"

#include <stdlib.h>

using namespace std;
using namespace symbolic;

int main(int argc, char *argv[])
{
  COptGen coptgen;
  for (int argi=1; argi<argc; ++argi) {
    std::string arg=argv[argi];
    int eq=arg.find("=");
    if (eq != std::string::npos) {
      std::string lhs=arg.substr(0,eq);
      std::string rhs=arg.substr(eq+1);
      coptgen.assign(lhs,rhs);
      continue;
    }

    if (arg == "-format_c_double") {
      coptgen.format = &symbolic::format_c_double;
      continue;
    }

    if (arg == "-format_c_single") {
      coptgen.format = &symbolic::format_c_single;
      continue;
    }

    if (arg == "-format_scilab") {
      coptgen.format = &symbolic::format_scilab;
      continue;
    }

    std::cerr << "unknown arg " << arg << endl;
    exit(1);
  }

  coptgen.print(std::cout);
  
  return 0;
}
