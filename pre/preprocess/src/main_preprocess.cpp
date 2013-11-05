#include "codegenerator.hpp"
#include "configuration.hpp"
#include "encode_string.hpp"
#include "symbolic.hpp"
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace preprocess;
using namespace utilities;

void list_available_generators(ostream &out)
{
  out << "available generators:";
  CodeGenerator::Registry &registry=*CodeGenerator::registry_get();
  for (CodeGenerator::Registry::iterator i=registry.begin();
       i != registry.end(); ++i) {
    out << " " << i->first;
  }
  out << endl;
}

void usage()
{
  cout << "usage: preprocessor [-h|--help] [-v|--verbose]  <config.ini> [<flag_name>=<flag_value> ...] generator..." << endl;
  cout << "\t" << "see sample.ini for an example .ini file." << endl;
  list_available_generators(cout);
}


int main(int argc, char *argv[])
{
  Configuration configuration;
  bool configured=false;
  bool verbose=false;
  
  if (argc < 2) { usage(); exit(1); }
  for (int argi=1; argi<argc; ++argi) {
    std::string arg=argv[argi];
    if (arg == "-h" || arg == "--help") { usage(); exit(1); }
    if (arg == "-v" || arg == "--verbose") { verbose=true; continue; }

    //
    // use name=value pairs to adjust flags section of configuration,
    // note these adjustments have to be made after a configuration 
    // is loaded.
    //
    if (arg.find("=") != string::npos) {
      string name=arg.substr(0,arg.find("="));
      string value=arg.substr(arg.find("=")+1);
      configuration.flags[name]=value;
      if (verbose) { cout << "set flag " << name << "=" << encode_string(value) << endl; }
      continue;
    }

    // read in .ini files as the current configuration
    if (arg.length() > 4 && arg.substr(arg.length()-4) == ".ini") {
      if (configuration_parse(arg,configuration)) {
	configured=true;
	if (verbose) {
	  cout << "read configuration file: " << encode_string(arg) << endl;
	  cout << configuration << endl;
	}
      } else {
	cerr << "bad configuration file: " << arg << endl;
	exit(1);
      }
      continue;
    }

    //
    // any other arguments should be registered generators.  If not, list the
    // registered generators and exit.
    // 
    //

    CodeGenerator *generator=CodeGenerator::registry_lookup(arg);
    if (configured && generator != 0) {
	generator->configure(&configuration);
	try {
	  generator->generate();
	} catch (preprocess::CodeGenerator::Exception *exception) {
	  cerr << "generator exception: " << exception->message << endl;
	  exit(1);
	} catch (symbolic::Expression::UnsupportedOperation *exception) {
	  cerr << "symbolic exception: " << exception->message << endl;
	  exit(1);
	}
	if (verbose) { cout << "applied generator " << arg << " with current configuration." << endl; }
    }
    if (!configured) {
      cerr << "cannot generate " << encode_string(arg) << " without ini file." << endl;
      exit(1);
    }
    if (generator == 0) {
      cerr << "unknown generator " << encode_string(arg) << " -- did you link in the code generator?" << endl;
      list_available_generators(cerr);
      exit(1);
    }
  }

  return 0;
}
