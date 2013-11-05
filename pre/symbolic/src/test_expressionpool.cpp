#include <iostream>
#include "symbolic.hpp"
#include "formatter.hpp"

using namespace symbolic;
using namespace std;

int main()
{
  //  for (int i=0; i<1000000; ++i) {
  for(;;){
    ExpressionPool ep;
    Expression *z=ep.parse("(x+y)^p*(x-y)^q/log(x^2+y^2)");
    cout << "z=" << format_plain(z) << endl;
    Expression *x=ep.variable("x");
    Expression *y=ep.variable("y");
    Expression *p=ep.variable("p");
    Expression *q=ep.variable("q");
    
    Expression *log_x = ep.log(x);
    Expression *power_x_y = ep.power(x,y);
    Expression *product_p_q = ep.product(p,q);
    Expression *sum_x_y = ep.sum(x,y);
    Expression *sum_x_y2 = ep.sum(sum_x_y,sum_x_y);

    Expression *sum_p_q_x_y = ep.simplify(sum_x_y2);
    
    Expression *dz_dx=ep.differentiate(z,x);
    Expression *dz_dy=ep.differentiate(z,y);
    Expression *dz_dp=ep.differentiate(z,p);
    Expression *dz_dq=ep.differentiate(z,q);

    cout << "pool size = " << ep.size() << endl;
  }
  return 0;
}
