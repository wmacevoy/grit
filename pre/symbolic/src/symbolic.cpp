#include "symbolic.hpp"
#include "formatter.hpp"
#include "symbolic_parse.hpp"
#include "utilities.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <math.h>
#include <typeinfo>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//
// Support multinomial expressions, and the differentiation of
// those expressions (except for parameters that appear in exponents),
// since differentiating with respect to such parameters results in
// something that is not a multinomial.
//
//
// Conventions: 
//   a null expression is zero.
//   simplify() returns a new object, even if it is not simplified.
//   
//
namespace symbolic
{
  NamedConstants Expression::namedConstants;

  const std::map < std::string , double > Expression::NONE;

  Expression::Expression(const Expression &) { assert(false); }

#ifdef SYMBOLIC_OUT_MODE
  int Expression::outMode=symbolic::Expression::OUT_MODE_NONE;
#endif
  std::string latex(const std::string &var)
  {
    std::string ans = "";
    int statesub = 0;
    int prev_statesub = 0;
    for (int i=0; i<var.length(); ++i) {
      char c = var[i];
      if ((c <= '9' && c >= '0') || ('+' == c) || ('-' == c)) {
	statesub=1;
	if (!prev_statesub) {
	  ans.append("_{");
	} 
	ans.append(1,c);
      }
      else {
	statesub=0;
	if (prev_statesub) {
	  ans.append("}");
	}
	if (('{' == c) || ('}' == c)) {
	  ans.append("\\");
	}
	ans.append(1,c);
      }
      prev_statesub = statesub;
    }

    return(ans);
  }

  Expression* clone(const Expression *e)
  {
    return ( e == 0 ) ? 0 : e->clone();
  }

  Expression* constant(const double x) {
    return ( x == 0.0 ) ? 0 : new Constant(x);
  }

  Expression* simplify(const Expression *e) {
    return ( e == 0 ) ? 0 : e->simplify();
  }

  Expression* substitute(const std::map < std::string , const Expression * > &substitutions , const Expression *e)
  {
    Expression *ans=0;
    if (e == 0) {
      ans = 0;
    } else if (typeid(*e) == typeid(Sum)) {
      ans = new Sum();
      ((Sum*)ans)->parts.resize(((const Sum*)e)->parts.size());
      for (size_t i=0; i<((Sum*)ans)->parts.size(); ++i) {
	((Sum*)ans)->parts[i]=substitute(substitutions,((const Sum*)e)->parts[i]);
      }
    } else if (typeid(*e) == typeid(Product)) {
      ans = new Product();
      ((Product*)ans)->parts.resize(((const Product*)e)->parts.size());
      for (size_t i=0; i<((Product*)ans)->parts.size(); ++i) {
	((Product*)ans)->parts[i]=substitute(substitutions,((const Product*)e)->parts[i]);
      }
    } else if (typeid(*e) == typeid(Power)) {
      ans = new Power();
      ((Power*)ans)->base = substitute(substitutions,((const Power*)e)->base);
      ((Power*)ans)->power = substitute(substitutions,((const Power*)e)->power);
    } else if (typeid(*e) == typeid(NaturalLog)) {
      ans = new NaturalLog(0);
      ((NaturalLog*)ans)->arg = substitute(substitutions,((const NaturalLog*)e)->arg);
    } else if (typeid(*e) == typeid(Sin)) {
      ans = new Sin(0);
      ((Sin*)ans)->arg = substitute(substitutions,((const Sin*)e)->arg);
    } else if (typeid(*e) == typeid(Cos)) {
      ans = new Cos(0);
      ((Cos*)ans)->arg = substitute(substitutions,((const Cos*)e)->arg);
    } else if (typeid(*e) == typeid(Constant)) {
      ans = new Constant(((const Constant*)e)->value);
    } else if (typeid(*e) == typeid(Variable)) {
      std::map < std::string , const Expression * > :: const_iterator i = substitutions.find(((const Variable*)e)->name);
      if (i != substitutions.end()) {
	ans = symbolic::clone(i->second);
      } else {
	ans = new Variable(((const Variable*)e)->name);
      }
    } else {
      assert(false);
    }
    return ans;
  }

  Expression* differentiate(const Expression *e, const std::string &variable) {
    Expression *unsimplified = ( e == 0 ) ? 0 : e->differentiate(variable);
    Expression *ans = (unsimplified == 0) ? 0 : unsimplified->simplify();
    delete unsimplified;
    return ans;
  }

  bool isconstant(const Expression *e) {
    return ( e == 0 ) ? true : e->isConstant();
  }

  std::set<std::string> symbols(const Expression *e) {
    std::set<std::string> ans;
    if (e != 0) e->symbols(ans);
    return ans;
  }

  double evaluate(const Expression *e, const std::map<std::string, double> &values) {
    return (e == 0) ? 0.0 : e->evaluate(values);
  }

  int compare(const Expression *a, const Expression *b)
  {
    if (a == 0) {
      if (b == 0) return 0;
      if (b != 0) return -3;
    } else {
      if (b == 0) return 3;
      if (typeid(*a) == typeid(*b)) {
	int ans=a->compareTo(b);

	if (ans < 0) return -1;
	if (ans > 0) return 1;
	return 0;
      } else {
	int ans = strcmp(typeid(*a).name(),typeid(*b).name());
	if (ans < 0) return -2;
	if (ans > 0) return 2;
	return 0;
      }
    }
    assert(false);
  }

  static bool less(const Expression *a, const Expression *b)
  {
    return compare(a,b) < 0;
  }

#ifdef SYMBOLIC_PRINT
  std::string tostring(const Expression *e)
  {
    std::ostringstream oss;
    oss << e;
    return oss.str();
  }
#endif

  NamedConstants::NamedConstants()
  {
    (*this)["%e"]=M_E;
    (*this)["%pi"]=M_PI;
  }

  Expression::Expression() 
  {
#ifdef SYMBOLIC_OUT_MODE
    if (outMode == OUT_MODE_NONE) outMode=OUT_MODE_C_DOUBLE;
#endif
  }

  Expression::~Expression() {};

  // Natural Log
  NaturalLog::NaturalLog(Expression *_arg) : arg(_arg) {}

  bool NaturalLog::isConstant() const 
  {
    return isconstant(arg);
  }

  double NaturalLog::evaluate(const std::map<std::string, double> &values) const
  {
    return log(symbolic::evaluate(arg,values));
  }  

#ifdef SYMBOLIC_PRINT
  void NaturalLog::print(std::ostream &out) const
  {
    switch(outMode) {
    case OUT_MODE_C_SINGLE:
          out << "logf" << "(" << arg << ")";
	  break;
    case OUT_MODE_LATEX:
      out << "{\\log{\\left(" << arg << "\\right)}}";
      break;
    default:
          out << "log" << "(" << arg << ")";
	  break;
    }
  }
#endif

  Expression* NaturalLog::clone() const
  {
    return new NaturalLog(symbolic::clone(arg));
  }

  int NaturalLog::compareTo(const Expression *e) const
  {
    assert(typeid(*e) == typeid(*this));
    return compare(arg,((const NaturalLog*)e)->arg);
  }

  Expression* NaturalLog::differentiate(const std::string &variable) const
  {
    return new Product(new Power(symbolic::clone(arg),new Constant(-1)),symbolic::differentiate(arg,variable));
  }

  Expression* NaturalLog::simplify() const
  {
    if (isConstant()) 
    {
      double ans = evaluate();

      return constant(ans);
    } 
    else 
    {
      Expression *s_arg = symbolic::simplify(arg);

      if (typeid(*s_arg) == typeid(Product)) 
      {
	Product *p = (Product*) s_arg;
	Sum *sum = new Sum();
	for (std::vector<Expression*>::iterator i = p->parts.begin(); i != p->parts.end(); ++i) {
	  sum->parts.push_back(new NaturalLog(*i));
	}
	p->parts.clear();


	Expression *ans = symbolic::simplify(sum);


	delete s_arg;
	delete sum;
	return ans;
      } 
      else if (typeid(*s_arg) == typeid(Power)) 
      {
	Power *pwr = (Power*) s_arg;
        Product *prod = new Product(pwr->power,new NaturalLog(pwr->base));
	Expression *ans = symbolic::simplify(prod);
	
        pwr->power=0;
	pwr->base=0;
	delete pwr;
	delete prod;
	return ans;
      }
      else
      {   
        return new NaturalLog(s_arg);
      }

    }
  }

  void NaturalLog::symbols(std::set<std::string> &names) const {
    if (arg != NULL) arg->symbols(names);
  }

  NaturalLog::~NaturalLog() 
  {
    delete arg;
  }

  // Cos
  Cos::Cos(Expression *_arg) : arg(_arg) {}

  bool Cos::isConstant() const 
  {
    return isconstant(arg);
  }

  double Cos::evaluate(const std::map<std::string, double> &values) const
  {
    return cos(symbolic::evaluate(arg,values));
  }  

#ifdef SYMBOLIC_PRINT
  void Cos::print(std::ostream &out) const
  {
    switch(outMode) {
    case OUT_MODE_C_SINGLE:
          out << "cosf" << "(" << arg << ")";
	  break;
    case OUT_MODE_LATEX:
      out << "{\\cos{\\left(" << arg << "\\right)}}";
      break;
    default:
          out << "cos" << "(" << arg << ")";
	  break;
    }
  }
#endif

  Expression* Cos::clone() const
  {
    return new Cos(symbolic::clone(arg));
  }

  int Cos::compareTo(const Expression *e) const
  {
    assert(typeid(*e) == typeid(*this));
    return compare(arg,((const Cos*)e)->arg);
  }

  Expression* Cos::differentiate(const std::string &variable) const
  {
    return new Product(new Sin(symbolic::clone(arg)),new Product(new Constant(-1.0),symbolic::differentiate(arg,variable)));
  }

  Expression* Cos::simplify() const
  {
    if (isConstant()) 
    {
      double ans = evaluate();

      return constant(ans);
    } 
    else 
    {
      return new Cos(symbolic::simplify(arg));
    }
  }

  void Cos::symbols(std::set<std::string> &names) const {
    if (arg != NULL) arg->symbols(names);
  }

  Cos::~Cos() 
  {
    delete arg;
  }

  // Sin
  Sin::Sin(Expression *_arg) : arg(_arg) {}

  bool Sin::isConstant() const 
  {
    return isconstant(arg);
  }

  double Sin::evaluate(const std::map<std::string, double> &values) const
  {
    return sin(symbolic::evaluate(arg,values));
  }  

#ifdef SYMBOLIC_PRINT
  void Sin::print(std::ostream &out) const
  {
    switch(outMode) {
    case OUT_MODE_C_SINGLE:
          out << "sinf" << "(" << arg << ")";
	  break;
    case OUT_MODE_LATEX:
      out << "{\\sin{\\left(" << arg << "\\right)}}";
      break;
    default:
          out << "sin" << "(" << arg << ")";
	  break;
    }
  }
#endif

  Expression* Sin::clone() const
  {
    return new Sin(symbolic::clone(arg));
  }

  int Sin::compareTo(const Expression *e) const
  {
    assert(typeid(*e) == typeid(*this));
    return compare(arg,((const Sin*)e)->arg);
  }

  Expression* Sin::differentiate(const std::string &variable) const
  {
    return new Product(new Cos(symbolic::clone(arg)),symbolic::differentiate(arg,variable));
  }

  Expression* Sin::simplify() const
  {
    if (isConstant()) 
    {
      double ans = evaluate();

      return constant(ans);
    } 
    else 
    {    
      return new Sin(symbolic::simplify(arg));
    }
  }

  void Sin::symbols(std::set<std::string> &names) const {
    if (arg != NULL) arg->symbols(names);
  }

  Sin::~Sin() 
  {
    delete arg;
  }


  // Product
  Product::Product() {}
  Product::Product(Expression *factor0, Expression *factor1) : parts(2) {
    parts[0]=factor0;
    parts[1]=factor1;
  }

  bool Product::isConstant() const {
    for (size_t i=0; i<parts.size(); ++i) {
      if (!isconstant(parts[i])) return false;
    }
    return true;
  }

  double Product::evaluate(const std::map<std::string, double> &values) const {
    double ans=1.0;
    for (size_t i=0; i<parts.size(); ++i) {
      ans *= symbolic::evaluate(parts[i],values);
    }
    return ans;
  }
  
#ifdef SYMBOLIC_PRINT
  void Product::print(std::ostream &out) const
  {
    if (outMode == OUT_MODE_LATEX) {
      out << "{\\left(";
    }
    else {
      out << "(";
    }
    for (size_t i=0; i<parts.size(); ++i) {
      if (i>0) out << "*";
      out << parts[i];
    }
    if (outMode == OUT_MODE_LATEX) {
      out << "\\right)}";
    }
    else {
      out << ")";
    }
  }
#endif

  Expression* Product::clone() const
  {
    Product *ans = new Product();
    for (size_t i=0; i<parts.size(); ++i) {
      ans->parts.push_back(symbolic::clone(parts[i]));
    }
    return ans;
  }

  int Product::compareTo(const Expression *e) const
  {
    assert(typeid(*e) == typeid(*this));
    const Product *prod = (const Product *)e;
    int i=0;
    while (i < parts.size() && i < prod->parts.size()) {
      int ans = compare(parts[i],prod->parts[i]);
      if (ans != 0) return ans;
      ++i;
    }
    if (i < prod->parts.size()) return -1;
    if (i < parts.size()) return 1;
    return 0;
  }

  Expression* Product::differentiate(const std::string &variable) const
  {
    Sum *ans = 0;
    for (size_t i=0; i<parts.size(); ++i) {
      Expression *producti=symbolic::differentiate(parts[i],variable);
      if (producti != 0) {
	if (ans == 0) ans = new Sum();
	Product *part=new Product();
	for (size_t j=0; j<parts.size(); ++j) {
	  if (i !=j) { 
	    part->parts.push_back(symbolic::clone(parts[j]));
	  } else {
	    part->parts.push_back(producti);
	  }
	}
	ans->parts.push_back(part);
      }
    }
    return ans;
  }

  static const Expression* baseof(const Expression *e) 
  {
    if (e != 0 && typeid(*e) == typeid(const Power)) {
      return ((const Power*) e)->base;
    } else {
      return e;
    }
  }

  static Expression* powerof(const Expression *e)
  {
    if (e != 0 && typeid(*e) == typeid(const Power)) {
      return symbolic::clone(((const Power*) e)->power);
    } else {
      return symbolic::constant(1.0);
    }
  }


  Expression* Product::simplify() const
  {
    if (isConstant()) {
      return constant(evaluate());
    } else {
      Product *ans = new Product();
      double const_part=1.0;
      for (size_t i=0; i<parts.size(); ++i) {
	Expression *part=symbolic::simplify(parts[i]);
	if (part == 0) {
	  delete ans;
	  return 0;
	} else {
	  if (isconstant(part)) {
	    const_part *= symbolic::evaluate(part);
	    delete part;
	    continue;
	  }
	  if (part != 0 && typeid(*part) == typeid(Product)) {
	    Product *product_part=(Product*) part;
	    for (size_t j=0; j<product_part->parts.size(); ++j) {
	      if (isconstant(product_part->parts[j])) {
		const_part *= symbolic::evaluate(product_part->parts[j]);
	      } else {
		ans->parts.push_back(product_part->parts[j]);
		product_part->parts[j]=0;
	      }
	    }
	    delete product_part;
	  } else {
	    for (std::vector<Expression*>::iterator i=ans->parts.begin();
		 i != ans->parts.end(); ++i) {
	      if (compare(baseof(*i),baseof(part))==0) {
		Expression *old_part = *i;
		Expression *unsimplified_part = new Power(symbolic::clone(baseof(old_part)),new Sum(powerof(old_part),powerof(part)));
		Expression *simplified_part = symbolic::simplify(unsimplified_part);
		delete part;
		delete old_part;
		delete unsimplified_part;
		*i = simplified_part;
		part = 0;
		break;
	      }
	    }
	    if (part != 0) {
	      ans->parts.push_back(part);
	    }
	  }
	}
      }

      std::sort(ans->parts.begin(),ans->parts.end(),less);


      if (const_part == 0.0) {
	delete ans;
	return 0;
      } else if (const_part != 1.0) {
        if (ans->parts.size() == 1 && typeid(*ans->parts[0]) == typeid(Sum)) {
	  Sum *sum_ans = (Sum*) (ans->parts[0]);
	  // distribute const through sum
	  Sum *sum_ans_dist_unsimplified = new Sum();
	  for (size_t i=0; i< sum_ans->parts.size(); ++i) {
	    sum_ans_dist_unsimplified->parts.push_back(new Product(new Constant(const_part),symbolic::clone(sum_ans->parts[i])));
	  }
	  Expression *ans_simplified = symbolic::simplify(sum_ans_dist_unsimplified);
	  delete sum_ans_dist_unsimplified;
	  delete ans;
	  return ans_simplified;
        } else {
	  ans->parts.insert(ans->parts.begin(),new Constant(const_part));
        }
      }

      if (ans->parts.size() == 0) {
	delete ans;
	return new Constant(1.0);
      }
      if (ans->parts.size() == 1) {
	Expression *real_ans=ans->parts[0];
	
	ans->parts.resize(0);
	delete ans;
	return real_ans;
      }

      

      return ans;
    }
  }

  void Product::symbols(std::set<std::string> &names) const {
    for (size_t i=0; i<parts.size(); ++i) {
      if (parts[i]) parts[i]->symbols(names);
    }
  }

  Product::~Product() { 
    for (size_t i=0; i<parts.size(); ++i) {
      delete parts[i];
    }
  }

  Sum::Sum() {}
  Sum::Sum(Expression *term0, Expression *term1) : parts(2) {
    parts[0]=term0;
    parts[1]=term1;
  }
  
  bool Sum::isConstant() const {
    for (size_t i=0; i<parts.size(); ++i) {
      if (!symbolic::isconstant(parts[i])) return false;
    }
    return true;
  }

  double Sum::evaluate(const std::map<std::string, double> &values) const {
    double ans=0.0;
    for (size_t i=0; i<parts.size(); ++i) {
      ans += symbolic::evaluate(parts[i],values);
    }
    return ans;
  }
  
#ifdef SYMBOLIC_PRINT
  void Sum::print(std::ostream &out) const
  {
    if (outMode == OUT_MODE_LATEX) {
      out << "{\\left(";
    }
    else {
      out << "(";
    }
    for (size_t i=0; i<parts.size(); ++i) {
      if (i>0) out << "+";
      out << parts[i];
    }
    if (outMode == OUT_MODE_LATEX) {
      out << "\\right)}";
    }
    else {
      out << ")";
    }
  }
#endif

  Expression* Sum::clone() const
  {
    Sum *ans = new Sum();
    for (size_t i=0; i<parts.size(); ++i) {
      ans->parts.push_back(symbolic::clone(parts[i]));
    }
    return ans;
  }
  
  int Sum::compareTo(const Expression *e) const
  {
    assert(typeid(*e) == typeid(*this));
    const Sum *sum = (const Sum *)e;
    int i=0;
    while (i < parts.size() && i < sum->parts.size()) {
      int ans = compare(parts[i],sum->parts[i]);
      if (ans != 0) return ans;
      ++i;
    }
    if (i < sum->parts.size()) return -1;
    if (i < parts.size()) return 1;
    return 0;
  }

  Expression* Sum::differentiate(const std::string &variable) const
  {
    Sum *ans = 0;
    for (size_t i=0; i<parts.size(); ++i) {
      Expression *sumi=symbolic::differentiate(parts[i],variable);
      if (sumi != 0) {
	if (ans == 0) ans = new Sum();
	ans->parts.push_back(sumi);
      }
    }
    return ans;
  }

  Expression* Sum::simplify() const
  {
    if (isConstant()) {
      return constant(evaluate());
    } else {
      Sum *ans = new Sum();
      double const_part=0.0;
      for (size_t i=0; i<parts.size(); ++i) {
	Expression *part=symbolic::simplify(parts[i]);
	if (part !=0) {
	  if (isconstant(part)) {
	    const_part += symbolic::evaluate(part);
	    delete part;
	    continue;
	  }
	  if (part != 0 && typeid(*part) == typeid(Sum)) {
	    Sum *sum_part=(Sum*) part;
	    for (size_t j=0; j<sum_part->parts.size(); ++j) {
	      if (isconstant(sum_part->parts[j])) {
		const_part += symbolic::evaluate(sum_part->parts[j]);
	      } else {
		ans->parts.push_back(sum_part->parts[j]);
		sum_part->parts[j]=0;
	      }
	    }
	    sum_part->parts.resize(0);
	    delete sum_part;
	  } else {
	    ans->parts.push_back(part);
	  }
	}
      }

      std::sort(ans->parts.begin(),ans->parts.end(),less);

      if (const_part != 0.0) {
	ans->parts.push_back(new Constant(const_part));
      }

      if (ans->parts.size() == 0) {
	delete ans;
	return 0;
      }
      if (ans->parts.size() == 1) {
	Expression *real_ans=ans->parts[0];
	ans->parts.resize(0);
	delete ans;
	return real_ans;
      }
      return ans;
    }
  }

  void Sum::symbols(std::set<std::string> &names) const {
    for (size_t i=0; i<parts.size(); ++i) {
      if (parts[i]) parts[i]->symbols(names);
    }
  }

  Sum::~Sum() { 
    for (size_t i=0; i<parts.size(); ++i) {
      delete parts[i];
    }
  }
 
  Power::Power() : base(0), power(0) {}
  Power::Power(Expression *_base, Expression *_power) : base(_base), power(_power) {}

  bool Power::isConstant() const {
    return isconstant(base) && isconstant(power);
  }

  double Power::evaluate(const std::map<std::string, double> &values) const {
    return pow(symbolic::evaluate(base,values),symbolic::evaluate(power,values));
  }
  

#ifdef SYMBOLIC_PRINT
  void Power::print(std::ostream &out) const
  {
    switch(outMode) {
    case OUT_MODE_C_SINGLE:
      if (base != 0 && typeid(*base) == typeid(Variable) && ((Variable*)base)->name == "%e") 
	{
	  out << "expf(" << power << ")";
	}
      else
	{
	  out << "powf(" << base << "," << power << ")";
	}
      break;
    case OUT_MODE_C_DOUBLE:
      if (base != 0 && typeid(*base) == typeid(Variable) && ((Variable*)base)->name == "%e") 
	{
	  out << "exp(" << power << ")";
	}
      else
	{
	  out << "pow(" << base << "," << power << ")";
	}
      break;
    case OUT_MODE_LATEX:
      if (base != 0 && typeid(*base) == typeid(Variable) && ((Variable*)base)->name == "%e") 
	{
	  out << "{e^{" << power << "}}";
	}
      else
	{
	  out << "{{" << base << "}" << "^{" << power << "}}";
	}
      break;
    case OUT_MODE_SCI:
    case OUT_MODE_INI:
      out << "((" << base << ')' << '^' << '(' << power << "))";
      break;
    }
  }
#endif

  Expression* Power::clone() const
  {
    Power *ans = new Power();
    ans->base=symbolic::clone(base);
    ans->power=symbolic::clone(power);
    return ans;
  }
  
  int Power::compareTo(const Expression *e) const
  {
    assert(typeid(*e) == typeid(*this));
    const Power *to = (const Power *)e;
    int ans=compare(base,to->base);
    if (ans != 0) return ans;
    return compare(power,to->power);
  }

  Expression* Power::differentiate(const std::string &variable) const
  {
    Product *term1=new Product(symbolic::clone(power),new Power(symbolic::clone(base),new Sum(symbolic::clone(power),symbolic::constant(-1.0))));
    term1->parts.push_back(symbolic::differentiate(base,variable));

    Product *term2=new Product(new NaturalLog(symbolic::clone(base)),clone());
    term2->parts.push_back(symbolic::differentiate(power,variable));

    return new Sum(term1,term2);
  }

  Expression* Power::simplify() const
  {
    if (isConstant()) {
      return constant(evaluate());
    } else {
      if (isconstant(power)) {
	double p = symbolic::evaluate(power);
	if (p == 0.0) return symbolic::constant(1.0);
	if (p == 1.0) return symbolic::simplify(base);
      }
      if (isconstant(base)) {
	double b = symbolic::evaluate(base);
	if (b == 0.0) return symbolic::constant(0.0);
	if (b == 1.0) return symbolic::constant(1.0);
      }
      if (base != 0 && typeid(*base) == typeid(Power)) {
        const Power *power_base = (const Power*) base;
	Product *unsimplified_product=new Product(symbolic::simplify(power_base->power),symbolic::simplify(power));
	Expression *simplified_product=symbolic::simplify(unsimplified_product);
        Power *unsimplified_power=new Power(symbolic::simplify(power_base->base),simplified_product);
        Expression *simplified_power=symbolic::simplify(unsimplified_power);
	delete unsimplified_product;
	delete unsimplified_power;
	return simplified_power;
      }
      return new Power(symbolic::simplify(base),symbolic::simplify(power));
    }
  }

  void Power::symbols(std::set<std::string> &names) const {
    if (base) base->symbols(names);
    if (power) power->symbols(names);
  }

  Power::~Power() 
  {
    delete base;
    delete power;
  }

  Variable::Variable() {}
  Variable::Variable(const std::string &_name) : name(_name) {}

  bool Variable::isConstant() const {
    return (namedConstants.find(name) != namedConstants.end());
  }

  double Variable::evaluate(const std::map<std::string, double> &values) const {
    std::map<std::string,double>::const_iterator i=values.find(name);
    if (i != values.end()) {
      return i->second;
    } else {
      std::map<std::string,double>::const_iterator j=namedConstants.find(name);
      if (j != namedConstants.end()) { 
        return j->second;
      }
      std::ostringstream oss;
      oss << "need value of " << name << " to evaluate expression.";
      UnsupportedEvaluation exception;
      exception.message = oss.str();
      throw exception;
    }
    return 0.0;
  }

#ifdef SYMBOLIC_PRINT
  void Variable::print(std::ostream &out) const
  {
    switch(outMode) {
    case OUT_MODE_SCI:
      out << mangle(name);
      break;
    case OUT_MODE_C_SINGLE:
      if (!isConstant()) {
	out << mangle(name);
      } else {
	if (name == "%e") {
	  out << "((float)M_E)";
	} else if (name == "%pi") {
	  out << "((float)M_PI)";
	} else {
	  out << std::setprecision(15) << namedConstants[name] << "f";
	}
      }
      break;
    case OUT_MODE_C_DOUBLE:
      if (!isConstant()) {
	out << mangle(name);
      } else {
	if (name == "%e") {
	  out << "M_E";
	} else if (name == "%pi") {
	  out << "M_PI";
	} else {
	  out << std::setprecision(15) << namedConstants[name];
	}
      }
      break;

      //convert numbers to _numbers in compounds?
    case OUT_MODE_LATEX:
      if (!isConstant()) {
	out << latex(name);
      } else {
	if (name == "%e") {
	  out << "e";
	} else if (name == "%pi") {
	  out << "\\pi";
	} else {
	  out << namedConstants[name];
	}
      }
      break;
    case OUT_MODE_INI:
      out << name;
      break;
    }
  }
#endif


  static inline void subst(std::string &s, const std::string &a, const std::string &b)
  {
    for (;;) {
      size_t pos = s.find(a);
      if (pos != std::string::npos) {
        s.replace(pos,a.length(),b);
      } else {
        break;
      }
    }
  }

  std::string Variable::mangle(const std::string &word)
  {
    if (word == "" || word[0] != '{') return word;
    std::string ans=word.substr(1,word.length()-2);
    subst(ans,"(","_b_");
    subst(ans,")","_d_");
    subst(ans,"+","_p_");
    subst(ans,"-","_m_");
    subst(ans,"__","");

#ifdef SYMBOLIC_OUT_MODE
    if (outMode == OUT_MODE_SCI) {
      if (ans.length() > 10) {
	ans = "x" + utilities::md5(ans).substr(0,9);
      }
    }
#endif

    return ans;
  }


  Expression* Variable::clone() const
  {
    return new Variable(name);
  }

  int Variable::compareTo(const Expression *e) const
  {
    assert(typeid(*e) == typeid(*this));
    const Variable *variable = (const Variable *)e;

    if (name < variable->name) return -1;
    if (variable->name < name) return  1;
    return 0;
  }
  
  Expression* Variable::differentiate(const std::string &variable) const
  {
    return (name == variable) ? new Constant(1.0) : 0;
  }

  Expression* Variable::simplify() const
  {
    return clone();
  }
  
  void Variable::symbols(std::set<std::string> &names) const {
    if (!isConstant()) { names.insert(name); }
  }

  Variable::~Variable() {}

  Constant::Constant() { value=0.0; }
  Constant::Constant(double _value) { value=_value; }

  bool Constant::isConstant() const {
    return true;
  }

  double Constant::evaluate(const std::map<std::string, double> &values) const {
    return value;
  }

#ifdef SYMBOLIC_PRINT
  void Constant::print(std::ostream &out) const
  {
    switch(outMode) {
    case OUT_MODE_C_SINGLE:
      if (value == (long long)value) {
	out << std::setprecision(15) << (long long)value << ".0" << "f";
      }
      else {
	out << std::setprecision(15) << value << "f";
      }
      break;
    case OUT_MODE_LATEX:
      out << value;
      break;
    default:
      if (value == (long long)value) {
	out << std::setprecision(15) << (long long)value << ".0";
      }
      else {
	out << std::setprecision(15) << value;
      }
      break;
    }
  }
#endif

  Expression* Constant::clone() const
  {
    return new Constant(value);
  }

  int Constant::compareTo(const Expression *e) const
  {
    assert(typeid(*e) == typeid(*this));
    const Constant *constant = (const Constant *)e;
    if (value < constant->value) return -1;
    if (constant->value < value) return  1;
    return 0;
  }
  
  Expression* Constant::differentiate(const std::string &variable) const
  {
    return 0;
  }

  Expression* Constant::simplify() const
  {
    return (value != 0.0) ? clone() : 0;
  }

  void Constant::symbols(std::set<std::string> &names) const {
  }

  Constant::~Constant() {}

#ifdef SYMBOLIC_PRINT
  std::ostream &operator<< (std::ostream &out, const Expression *e)
  {
    if (e == 0){
      if (symbolic::Expression::outMode == symbolic::Expression::OUT_MODE_C_SINGLE){
	out << "0.0f";
      }
      else {
	out << "0.0";
      }
    }
    else e->print(out);
    return out;
  }
#endif

  bool expression_order::operator()(const Expression * const &a, const Expression * const &b) const
  {
    return compare(a,b) < 0;
  }

  bool expression_equals::operator()(const Expression * const &a, const Expression * const &b) const
  {
    return compare(a,b) == 0;
  }

  void ExpressionPool::delete_unpooled(Expression *&e)
  {
    if (pool.find(e) != pool.end()) return;
    
      if (typeid(*e) == typeid(Constant) || typeid(*e) == typeid(Variable)) {
	delete e;
	e=0;
      } else if (typeid(*e) == typeid(Power)) {
	delete_unpooled(((Power*)e)->base);
	delete_unpooled(((Power*)e)->power);
	delete e;
	e=0;
      } else if (typeid(*e) == typeid(NaturalLog)) {
	delete_unpooled(((NaturalLog*)e)->arg);
	delete e;
	e=0;
      } else if (typeid(*e) == typeid(Sum)) {
	for (size_t i=0; i<((Sum*)e)->parts.size(); ++i) {
	  delete_unpooled(((Sum*)e)->parts[i]);
	}
	delete e;
	e=0;
      } else if (typeid(*e) == typeid(Product)) {
	for (size_t i=0; i<((Product*)e)->parts.size(); ++i) {
	  delete_unpooled(((Product*)e)->parts[i]);
	}
	delete e;
	e=0;
      } else {
	assert(false);
      }
  }

  Expression *ExpressionPool::clone(const Expression *e) const
  {
    if (pool.find((Expression*)e) == pool.end()) return symbolic::clone(e);

    if (typeid(*e) == typeid(Constant) || typeid(*e) == typeid(Variable)) {
      return symbolic::clone(e);
    } else if (typeid(*e) == typeid(Power)) {
      return new Power(clone(((Power*)e)->base),clone(((Power*)e)->power));
    } else if (typeid(*e) == typeid(NaturalLog)) {
      return new NaturalLog(clone(((NaturalLog*)e)->arg));
    } else if (typeid(*e) == typeid(Sum)) {
      Sum *ans =new Sum();
      ans->parts.resize(((Sum*)e)->parts.size());
      for (size_t i=0; i<ans->parts.size(); ++i) {
	ans->parts[i]=clone(((Sum*)e)->parts[i]);
      }
      return ans;
    } else if (typeid(*e) == typeid(Product)) {
      Product *ans =new Product();
      ans->parts.resize(((Product*)e)->parts.size());
      for (size_t i=0; i<ans->parts.size(); ++i) {
	ans->parts[i]=clone(((Product*)e)->parts[i]);
      }
      return ans;
    } else {
      assert(false);
    }
  }

  Expression * ExpressionPool::insert(Expression *&e)
  {
    iterator i=pool.find(e);

    if (i != pool.end())  {
      if (*i != e) {
	delete_unpooled(e);
	e=*i;
      }
      return *i;
    }

    if (e == 0 || typeid(*e) == typeid(Constant) || typeid(*e) == typeid(Variable)) {
      pool.insert(e);
      return e;
    }
    if (typeid(*e) == typeid(NaturalLog)) {
      insert(((NaturalLog*)e)->arg);
      pool.insert(e);
      return e;
    }
    if (typeid(*e) == typeid(Power)) {
      insert(((Power*)e)->base);
      insert(((Power*)e)->power);
      pool.insert(e);
      return e;
    }
    if (typeid(*e) == typeid(Sum)) {
      for (size_t i=0; i<((Sum*)e)->parts.size(); ++i) {
	insert(((Sum*)e)->parts[i]);
      }
      pool.insert(e);
      return e;
    }
    if (typeid(*e) == typeid(Product)) {
      for (size_t i=0; i<((Product*)e)->parts.size(); ++i) {
	insert(((Product*)e)->parts[i]);
      }
      pool.insert(e);
      return e;
    }
    assert(false);
  }

  Expression* ExpressionPool::constant(double value) {
    Expression *ans = (value == 0) ? 0 : new Constant(value);
    return insert(ans);
  }

  Expression* ExpressionPool::variable(const std::string &name) {
    Expression *ans = new Variable(name);
    return insert(ans);
  }

  Expression* ExpressionPool::log(Expression *arg) {
    Expression *ans = new NaturalLog(arg);
    return insert(ans);
  }

  Expression* ExpressionPool::power(Expression *base, Expression *power) {
    Expression *ans = new Power(base,power);
    return insert(ans);
  }

  Expression* ExpressionPool::sum(Expression *a, Expression *b) {
    Expression *ans = new Sum(a,b);
    return insert(ans);
  }

  Expression* ExpressionPool::product(Expression *a, Expression *b) {
    Expression *ans = new Product(a,b);
    return insert(ans);
  }

  Expression* ExpressionPool::parse(const std::string &s)
  {
    std::map < std::string , Expression * > :: iterator i = parsed.find(s);
    if (i != parsed.end()) return i->second;

    Expression *ans = symbolic::parse(s);
    insert(ans);
    
    parsed[s]=ans;
    return ans;
  }

  Expression* ExpressionPool::simplify(const Expression *e) 
  {
    iterator i=simplified.find((Expression*)e);
    if (i != simplified.end()) return *i;

    Expression *ans = symbolic::simplify(e);
    insert(ans);

    simplified.insert(ans);
    return ans;
  }

  Expression* ExpressionPool::differentiate(const Expression *e, const std::string &x)
  {
    Expression *ans = symbolic::differentiate(e,x);
    return insert(ans);
  }

  Expression* ExpressionPool::differentiate(const Expression *e, const Expression *x)
  {
    assert(typeid(*x) == typeid(const Variable));
    return differentiate(e,((const Variable*)x)->name);
  }

  ExpressionPool::~ExpressionPool()
  {
    // make sure all children are directly in the pool, and remove them from sub-expressions
    for (iterator i=pool.begin(); i!=pool.end(); ++i) {
      Expression *e = *i;
      if (e == 0 || typeid(*e) == typeid(Constant) || typeid(*e) == typeid(Variable)) {
	// no cleanup
      } else if (typeid(*e) == typeid(Power)) {
	assert(pool.find(((Power*)e)->base) != pool.end());
	assert(pool.find(((Power*)e)->power) != pool.end());
	((Power*)e)->base=0;
	((Power*)e)->power=0;
      } else if (typeid(*e) == typeid(NaturalLog)) {
	assert(pool.find(((NaturalLog*)e)->arg) != pool.end());
	((NaturalLog*)e)->arg=0;
      } else if (typeid(*e) == typeid(Sum)) {
	for (size_t i=0; i<((Sum*)e)->parts.size(); ++i) {
	  assert(pool.find(((Sum*)e)->parts[i]) != pool.end());
	  ((Sum*)e)->parts[i]=0;
	}
      } else if (typeid(*e) == typeid(Product)) {
	for (size_t i=0; i<((Product*)e)->parts.size(); ++i) {
	  assert(pool.find(((Product*)e)->parts[i]) != pool.end());
	  ((Product*)e)->parts[i]=0;
	}
      } else {
	assert(false);
      }
    }

    // delete pool
    for (iterator i=pool.begin(); i!=pool.end(); ++i) {
      delete *i;
    }
  }

  size_t ExpressionPool::size() const { return pool.size(); }

#ifdef DEBUG_POOL
  std::pair < ExpressionPool::iterator , bool > ExpressionPool::DbgPool::insert(Expression *e)
  {
    std::pair < iterator , bool > ans = Pool::insert(e);
    std::cerr << "insert(" << format_plain(e) << "@" << ((void*) e) << ")=" << "(" << format_plain(*ans.first) << "@" << ((void*)(*ans.first)) << "," << ans.second << ")" << std::endl;
  }
#endif

}

