#include "Body.h"

#include "BodyGlobals.h"

using namespace std;

void Body::init()
{
  legs.init();
  neck.init();
  waist = servo("WAIST");
  left.init();
  right.init();
}

void Body::report(std::ostream &out) const
{
  right.report(out);
  out << endl;
}
