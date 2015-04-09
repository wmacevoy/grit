#include "symbolic.hpp"
#include "formatter.hpp"

using namespace symbolic;
using namespace std;

int main()
{
  Sum *sum = new Sum();

  {
    Product *product = new Product();
    product->parts.push_back(new Constant(5.2));
    product->parts.push_back(new Power(new Variable("x"),new Product(new Constant(3.14),new Variable("k"))));
    product->parts.push_back(new Power(new Variable("y"),new Constant(0.72)));
    product->parts.push_back(new Variable("z"));
    sum->parts.push_back(product);
  }

  { 
    sum->parts.push_back(new Product(new Variable("x"),new Variable("y")));
  }

  { 
    sum->parts.push_back(new Product(new Variable("y"),new Variable("z")));
  }

  {
    sum->parts.push_back(new Product(new Constant(7.8),new Variable("x")));
  }

  {
    sum->parts.push_back(new Constant(5.0));
  }

  std::map < std::string, double > vars;
  vars["x"]=1.0;
  vars["y"]=2.0;
  vars["z"]=3.0;
  vars["k"]=7.0;
  vars["t"]=8.0;

  cout << "f=" << format_plain(sum) << endl;
  cout << "f(vars)=" << evaluate(sum,vars) << endl;

  for (std::map < std::string , double> :: iterator i=vars.begin(); i!=vars.end(); ++i) {
    string var=i->first;
    if (var == "k") continue;
    Expression *dbydi=differentiate(sum,var);
    cout << "df/d" << var << "=" << format_plain(dbydi) << endl;
    delete dbydi;
  }

  delete sum;
}
