#include "tolower.hpp"

namespace utilities {

  std::string tolower(const std::string &s)
  {
    std::string ans=s;
    for (size_t i=0; i<ans.length(); ++i) {
      if (ans[i] >= 'A' && ans[i] <= 'Z') ans[i] += 'a'-'A';
    }
    return ans;
  }
}
