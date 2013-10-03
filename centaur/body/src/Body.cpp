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

void Body::temp_report(std::ostream &out, std::string part) const
{
  if (part == "right")
    right.temp_report(out);
  else if (part == "left")
    left.temp_report(out);
  else if (part == "waist")
    waist->temp_report(out);
  else if (part == "neck")
    {
    neck.upDown->temp_report(out);
    neck.leftRight->temp_report(out);
    }
  else if (part == "legs")
    for (int i = 0; i < 4; i++)
      {
	legs[i].knee->temp_report(out);
	legs[i].femur->temp_report(out);
	legs[i].hip->temp_report(out);
      }
  out << endl;
}
