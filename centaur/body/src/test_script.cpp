#include "Script.h"
#include "CSVSplit.h"

#include <iostream>
#include <assert.h>

using namespace std;

int main(int argc, char *argv[])
{
  Script s(argv[0]);
  try {
    if (argc >= 2) {
      for (int argi=1; argi<argc; ++argi) {
	s.run(argv[argi]);
      }
    }
  } catch (const Script::Error &e) {
    cout << e << endl;
    assert(false);
  }
  cout << "ok" << endl;
  return 0;
}
