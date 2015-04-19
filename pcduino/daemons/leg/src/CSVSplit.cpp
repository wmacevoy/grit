#include "CSVSplit.h"

using namespace std;

void CSVSplit(const std::string &line, std::vector < std::string > &vals, char sep)
{
  int state = 0;
  string val;
  vals.clear();
  for (size_t i=0; i <= line.size(); ++i) {
    char ch = i < line.size() ? line[i] : sep;
    switch(state) {
    case 0: 
      if (ch == sep) { vals.push_back(""); }
      else if (ch != '"') { state = 1; val.clear(); val.push_back(ch); }
      else { state = 2; val.clear(); }
      break;
    case 1:
      if (ch != sep) { val.push_back(ch); }
      else { state = 0; vals.push_back(val); }
      break;
    case 2:
      if (ch == '"') { state = 3; }
      else { val.push_back(ch); }
      break;
    case 3: 
      if (ch != sep) { state = 2; val.push_back(ch); }
      else { vals.push_back(val); state = 0;  }
      break;
    }
  }
  if (state != 0) {
    vals.push_back(val);
  }
}
