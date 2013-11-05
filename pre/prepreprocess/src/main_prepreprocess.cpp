#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "prepreprocess.hpp"

using namespace preprocess;
using namespace std;

int main(int argc, char *argv[])
{
  Prepreprocess ppp;

  for (int argi=1; argi<argc; ++argi) {
    string arg=argv[argi];
    if (arg == "-prefix") { ppp.prefix=argv[++argi]; continue; }
    if (arg == "-suffix") { ppp.suffix=argv[++argi]; continue; }
    if (arg == "-separator") { ppp.separator=argv[++argi]; continue; }
    if (arg == "-out") {
      string file=argv[++argi];
      if (ppp.out != &cout) delete ppp.out;
      ppp.out = (file == "-") ? &cout : new ofstream(file.c_str());
      if (!(*ppp.out)) { 
	cerr << "could not open output file " << file << endl;
	exit(1);
      }
      continue;
    }

    if (arg == "-in") {
      string file=argv[++argi];
      if (ppp.in != &cin) delete ppp.in;
      ppp.in=(file == "-") ? &cin : new ifstream(file.c_str());
      if (!(*ppp.in)) { 
	cerr << "could not open input file " << file << endl;
	exit(1);
      }

      if (file != "-") {
	(*ppp.out) << "#line 1 \"" << file << "\"" << endl;
      }
      ppp.process();
      continue;
    }

    if (arg != "-h") {
      cerr << "unknown argument " << arg << endl;
    }

    cerr << "usage: " << argv[0] << " [-h] [-prefix \"std::cout\"] [-suffix \"std::endl\"] [-separator \"<<\"] [-out \"-\"] [-in \"-\"]" << endl;
    cerr << "A file of \"-\" means using stdin/stdout." << endl;
    exit(1);
  }

  return 0;
}
