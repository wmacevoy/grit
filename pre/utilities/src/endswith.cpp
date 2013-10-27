#include "endswith.hpp"

namespace utilities {

  bool endswith(const std::string &s, const std::string &suffix)
  {
    int n=suffix.length();
    int n0=s.length();
    if (n0 < n) return false;
    n0 -= n;
    for (int i=0; i<n; ++i) {
      if (s[n0+i] != suffix[i]) return false;
    }
    return true;
  }
}
