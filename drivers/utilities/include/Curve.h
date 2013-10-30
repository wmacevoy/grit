#pragma once

#include <map>
#include <vector>
#include <memory>

class Curve
{
 public:
  struct Knot {
    float x,y;
    bool  sharp;
  };
  typedef std::vector < Knot > Knots;

  std::size_t n;
  Knots knots;
  std::vector<std::size_t> at;

  // x->y map
  void setup(const std::map < float , float > &points);

  // use estimates to pick corners with unitless cutoff:
  // abs(dy/dx(before)-dy/dx(after)/(dy/dx(max)) > cutoff
  // are considered sharp points
  void sharpen(float cutoff=1);

  // explicit x->(y,sharp) map
  void setup(const std::map < float , std::pair<float,bool> > &points);
  void interval(float x, int &k0, int &k1);
  void expand(float x,float &x0, float &x1,float c[3]);
};  

typedef std::shared_ptr < Curve > CurveSP;
