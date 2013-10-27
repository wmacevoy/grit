#include "endswith.hpp"

namespace utilities {

  bool beginswith(const std::string &s, const std::string &prefix)
  {
    int n=prefix.length();
    int n0=s.length();
    if (n0 < n) return false;

    for (int i=0; i<n; ++i) {
      if (s[i] != prefix[i]) return false;
    }
    return true;
  }

}
