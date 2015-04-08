#include "ceildiv.hpp"

namespace utilities {

  int ceildiv(int a,int b)
  {
    return (a/b)+(((a >= 0 && b >= 0) || (a <= 0 && b <= 0)) && ((a % b) != 0));
  }

}
