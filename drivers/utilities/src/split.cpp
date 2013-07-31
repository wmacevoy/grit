#include <sstream>
#include "split.h"
#include "trim.h"

using namespace std;

void split(const std::string &together, 
	   std::vector<std::string> &parts, char sep, bool dotrim)
{
  istringstream  lineStream(together);
  string cell;

  parts.clear();
  while(!!getline(lineStream,cell,sep)) {
    if (dotrim) trim(cell);
    parts.push_back(cell);
  }
}
