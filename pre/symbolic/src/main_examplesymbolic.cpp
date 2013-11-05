#include <iostream>
#include "symbolic.hpp"
#include "formatter.hpp"

using namespace symbolic;
using namespace std;

//
// convenience operator to write out sets of strings
//
std::ostream &operator<<(std::ostream &out, const std::set<std::string> &strings)
{
  out << "[";
  for (std::set<std::string>::const_iterator i=strings.begin(); i!=strings.end(); ++i) {
    if (i != strings.begin()) { out << ","; }
    out << "\"" << *i << "\"";
  }
  out << "]";
  return out;
}

//
// convenience operator to write out sets of assignments
//
std::ostream &operator<<(std::ostream &out, const std::map<std::string,double> &vars)
{
  out << "{";
  for (std::map<std::string,double>::const_iterator i=vars.begin(); i!=vars.end(); ++i) {
    if (i != vars.begin()) { out << ","; }
    out << i->first << "=" << i->second;
  }
  out << "}";
  return out;
}

int main()
{
  try {

    //
    // parse may throw an 
    //
    //   Expression::UnsupportedSyntax
    //
    // exception.  Expressions may have 
    //
    //   unary and binary "+" and "-"
    //   binary "*" for multiplication
    //   binary "^" for exponentiation
    //   () for grouping
    //   double precision constants
    //   variables (letters and underscores
    //     followed by letters underscores and digits)
    //
    Expression *z=parse("(2*x^p*y^q-x*y+1)*(x-y)");

    cout << "z=" << format_plain(z) << endl;

    //
    // parse errors throw a syntax exception:
    //
    try {
      Expression *q = parse("x+1**2");
      delete q;
    } catch (Expression::UnsupportedSyntax &exception) {
      cout << "exception: " << exception.message << endl;
    }


    //
    // differentiate may throw an
    //
    //   Expression::UnsupportedDifferentiation
    //
    // exception.  Because the result is not a
    // multinomial, differentiation against parameters
    // in an exponent is not supported.
    //
    Expression *dz_dx=differentiate(z,"x");
    Expression *dz_dy=differentiate(z,"y");
    Expression *dz_dt=differentiate(z,"t");

    //
    // differentiation errors throw a differentiation exception
    //
    try {
      Expression *dz_dp=differentiate(z,"p");
    } catch (Expression::UnsupportedDifferentiation &exception) {
      cout << "exception: " << exception.message << endl;
    }

    cout << "dz_dx=" << format_plain(dz_dx) << endl;
    cout << "dz_dy=" << format_plain(dz_dy) << endl;


    //
    // isconstant() determines if the expression depends on
    // any symbols
    //
    cout << "isconstant(dz_dx)=" << isconstant(dz_dx) << endl;
    cout << "isconstant(dz_dy)=" << isconstant(dz_dy) << endl;
    cout << "isconstant(dz_dt)=" << isconstant(dz_dt) << endl;

    //
    // symbols() returns the set of symbols an expression depends on
    //
    cout << "symbols(z)=" << symbols(z) << endl;

    map<string,double> vars;

    vars["x"]=2.1;
    vars["y"]=3.2;
    vars["p"]=1.5;
    vars["q"]=-1.5;
    
    //
    // evaluate may throw an
    //
    //  Expression::UnsupportedEvaluation
    //
    // exception.  (if a variables value
    // is required but not given)
    //
    
    cout << "z @ " << vars << " = " << evaluate(z,vars) << endl;

    try {
      vars.erase("x");
      evaluate(z,vars);
    } catch (Expression::UnsupportedEvaluation &exception) {
      cout << "exception: " << exception.message << endl;
    }


    //
    // functions return new objects, which must be deleted explicitly
    //
    delete z;
    delete dz_dx;
    delete dz_dy;
    delete dz_dt;

    // the base class of all Expression exceptions is
    //
    // Expression::UnsupportedOperation
    //
  } catch (Expression::UnsupportedOperation &exception) {
    cout << "exception: " << exception.message << endl;
  }

  return 0;
}
