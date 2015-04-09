#include <iostream>
#include <string>
#include <string.h>

#include "Configure.h"

Configure cfg;

int main(int argc,char **argv) {
  bool nopath = true;
  bool noload = true;


  cfg.args("config.",argv);
  if (argc == 1) cfg.load("config.csv");

  for (int argi = 1; argi < argc; ++argi) {
    if (strcmp(argv[argi],"--path") == 0) {
	++argi;
	if (strcmp(argv[argi],"--") != 0) {
	  cfg.path(argv[argi]);
	}
	nopath=false;
	continue;
    }
    if (strcmp(argv[argi],"--load") == 0) {
      ++argi;
      if (nopath) {
	cfg.path("../../setup");
	nopath=false;
      }
      cfg.load(argv[argi]);
      noload=false;
      continue;
    }
    if (nopath) {
      cfg.path("../../setup");
      nopath=false;
    }
    if (noload) {
      cfg.load("config.csv");
      noload=false;
    }
    std::string arg=argv[argi];
    std::cout << cfg.substitute(arg) << std::endl;
  }

  return 0;
}
