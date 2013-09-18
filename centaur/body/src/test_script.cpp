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
      s.addPaths(getenv("LD_LIBRARY_PATH"));
      s.import("body");
      string e = argv[1];
      Script::Object ans(s.eval(e));
      cout << e << "=" << ans << endl;
    }
  } catch (const Script::Error &e) {
    cout << e << endl;
    assert(false);
  }
  cout << "ok" << endl;
  return 0;
}
