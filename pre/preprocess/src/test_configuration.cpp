#include <iostream>
#include "configuration.hpp"

using namespace std;
using namespace preprocess;


int main(int argc, char *argv[])
{
  for (int i=1; i<argc; ++i) {
    Configuration configuration;
    if (configuration_parse(argv[i],configuration)) {
      cout << "read ini file '" << argv[i] << "' as" << endl;
      cout << configuration << endl;
    } else {
      cout << "could not read ini file '" << argv[i] << "'" << endl;
    }
  }
  return 0;
}
