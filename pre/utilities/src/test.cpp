#include <assert.h>
#include <math.h>
#include <unistd.h>
#include "utilities.hpp"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, const char *argv[])
{
  assert(utilities::beginswith(string(""),string(""))==true);
  assert(utilities::beginswith(string("bob"),string(""))==true);
  assert(utilities::beginswith(string(""),string("bob"))==false);
  assert(utilities::beginswith(string("bob"),string("bo"))==true);
  assert(utilities::beginswith(string("bob"),string("bob"))==true);
  assert(utilities::beginswith(string("bob"),string("bobby"))==false);

  assert(utilities::endswith(string(""),string(""))==true);
  assert(utilities::endswith(string("bob"),string(""))==true);
  assert(utilities::endswith(string(""),string("bob"))==false);
  assert(utilities::endswith(string("bob"),string("ob"))==true);
  assert(utilities::endswith(string("bob"),string("bob"))==true);
  assert(utilities::endswith(string("bob"),string("bobby"))==false);

  assert(utilities::md5("bob") == "9f9d51bc70ef21ca5c14f307980a29d8");

  assert(utilities::tolower("Bob")=="bob");
  assert(utilities::toupper("Bob")=="BOB");

  assert(utilities::encode_string("bob's \"country bunker\"") == "\"bob\\\'s \\\"country bunker\\\"\"");

  for (int a=-5; a<=5; ++a) {
    for (int b=-5; b<=5; ++b) {
      if (b != 0) {
	assert(utilities::ceildiv(a,b) == ceil(double(a)/double(b)));
      }
    }
  }

  utilities::Stopwatch sw;
  usleep(int(0.1*1e6));
  assert(fabs(sw.time()-0.1) < 0.0100);
  sw.stop();
  usleep(int(0.2*1e6));
  sw.start();
  usleep(int(0.3*1e6));
  assert(fabs(sw.time()-0.4) < 0.0100);

  cout << "ok" << endl;
  return 0;
}
