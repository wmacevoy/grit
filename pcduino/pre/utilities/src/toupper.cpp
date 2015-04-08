#include "toupper.hpp"

namespace utilities {

  std::string toupper(const std::string &s)
  {
    std::string ans=s;
    for (size_t i=0; i<ans.length(); ++i) {
      if (ans[i] >= 'a' && ans[i] <= 'z') ans[i] += 'A'-'a';
    }
    return ans;
  }
}
