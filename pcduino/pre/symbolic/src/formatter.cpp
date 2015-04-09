#include "formatter.hpp"
#include "utilities.hpp"
#include "math.h"

#include <iomanip>
#include <typeinfo>
#include <assert.h>
#include <sstream>

namespace symbolic
{
  const symbolic::Constant ZERO(0.0);

  void Formatter::formatSum(std::ostream &out, const symbolic::Sum *e, int priorityContext) const
  {
    for (size_t i=0; i<e->parts.size(); ++i) {
      if (i != 0) out << "+";
      format(out,e->parts[i],PRIORITY_SUM);
    }
  }

  void Formatter::formatProduct(std::ostream &out, const symbolic::Product *e, int priorityContext) const
  {
    for (size_t i=0; i<e->parts.size(); ++i) {
      if (i != 0) out << "*";
      format(out,e->parts[i],PRIORITY_PRODUCT);
    }
  }

  void Formatter::formatPower(std::ostream &out, const symbolic::Power *e, int priorityContext) const
  {
    if (priorityContext >= PRIORITY_POWER) formatStartGroup(out);
    format(out,e->base,PRIORITY_POWER);
    out << "^";
    format(out,e->power,PRIORITY_POWER);
    if (priorityContext >= PRIORITY_POWER) formatStartGroup(out);
  }

  void Formatter::formatNaturalLog(std::ostream &out, const symbolic::NaturalLog *e, int priorityContext) const
  {
    out << "log";
    formatStartGroup(out);
    format(out,e->arg,PRIORITY_NONE);
    formatEndGroup(out);
  }

  void Formatter::formatCos(std::ostream &out, const symbolic::Cos *e, int priorityContext) const
  {
    out << "cos";
    formatStartGroup(out);
    format(out,e->arg,PRIORITY_NONE);
    formatEndGroup(out);
  }

  void Formatter::formatSin(std::ostream &out, const symbolic::Sin *e, int priorityContext) const
  {
    out << "sin";
    formatStartGroup(out);
    format(out,e->arg,PRIORITY_NONE);
    formatEndGroup(out);
  }

  void Formatter::formatVariable(std::ostream &out, const symbolic::Variable *e, int priorityContext) const
  {
    out << e->name;
  }

  void Formatter::formatNamedConstant(std::ostream &out, const symbolic::Variable *e, int priorityContext) const
  {
    out << e->name;
  }

  void Formatter::formatConstant(std::ostream &out, const symbolic::Constant *e, int priorityContext) const
  {
    out << std::setprecision(16) << e->value;
  }

  void Formatter::format(std::ostream &out, const symbolic::Expression *e, int priorityContext) const
  {
    if (e == 0) {
      if (priorityContext > PRIORITY_TERM) formatStartGroup(out);
      formatConstant(out,&ZERO,priorityContext);
      if (priorityContext > PRIORITY_TERM) formatEndGroup(out);
    } else if (typeid(*e) == typeid(const Sum)) {
      if (priorityContext > PRIORITY_SUM) formatStartGroup(out);
      formatSum(out,(const Sum*) e,priorityContext);
      if (priorityContext > PRIORITY_SUM) formatEndGroup(out);
    } else if (typeid(*e) == typeid(const Product)) {
      if (priorityContext > PRIORITY_PRODUCT) formatStartGroup(out);
      formatProduct(out,(const Product*) e,priorityContext);
      if (priorityContext > PRIORITY_PRODUCT) formatEndGroup(out);
    } else if (typeid(*e) == typeid(const Power)) {
      if (priorityContext >= PRIORITY_POWER) formatStartGroup(out);
      formatPower(out,(const Power*) e,priorityContext);
      if (priorityContext >= PRIORITY_POWER) formatEndGroup(out);
    } else if (typeid(*e) == typeid(const NaturalLog)) {
      if (priorityContext >= PRIORITY_TERM) formatStartGroup(out);
      formatNaturalLog(out,(const NaturalLog*) e,priorityContext);
      if (priorityContext >= PRIORITY_TERM) formatEndGroup(out);
    } else if (typeid(*e) == typeid(const Cos)) {
      if (priorityContext >= PRIORITY_TERM) formatStartGroup(out);
      formatCos(out,(const Cos*) e,priorityContext);
      if (priorityContext >= PRIORITY_TERM) formatEndGroup(out);
    } else if (typeid(*e) == typeid(const Sin)) {
      if (priorityContext >= PRIORITY_TERM) formatStartGroup(out);
      formatSin(out,(const Sin*) e,priorityContext);
      if (priorityContext >= PRIORITY_TERM) formatEndGroup(out);
    } else if (typeid(*e) == typeid(const Variable)) {
      if (priorityContext >= PRIORITY_TERM) formatStartGroup(out);
      const Variable *v = (const Variable * ) e;
      if (Expression::namedConstants.find(v->name) == Expression::namedConstants.end()) {
	formatVariable(out,v,priorityContext);
      } else {
	formatNamedConstant(out,v,priorityContext);
      }
      if (priorityContext >= PRIORITY_TERM) formatEndGroup(out);
    } else if (typeid(*e) == typeid(const Constant)) {
      if (priorityContext >= PRIORITY_TERM || ( (((const Constant *)e)->value) < 0 && priorityContext >= PRIORITY_SUM))
        formatStartGroup(out);
      formatConstant(out,(const Constant*) e,priorityContext);
      if (priorityContext >= PRIORITY_TERM || ( (((const Constant *)e)->value) < 0 && priorityContext >= PRIORITY_SUM))  
        formatEndGroup(out);
    } else {
      std::cout << "unsupported format on type " << typeid(*e).name() << std::endl;
      assert(false);
    }
  }

  void Formatter::format(std::string &str, const symbolic::Expression *e, int priorityContext) const
  {
    std::ostringstream oss;
    format(oss,e,priorityContext);
    str=oss.str();
  }

  void Formatter::formatStartGroup(std::ostream &out) const
  {
    out << "(";
  }

  void Formatter::formatEndGroup(std::ostream &out) const
  {
    out << ")";
  }

  Formatter::~Formatter() {}

  static const Formatter STATIC_PLAIN_FORMATTER;
  const Formatter &PLAIN_FORMATTER(STATIC_PLAIN_FORMATTER);

  std::pair < const symbolic::Formatter * , const symbolic::Expression * > format_plain(const symbolic::Expression *e)
  {
    return   std::pair < const symbolic::Formatter * , const symbolic::Expression * > (&PLAIN_FORMATTER, e); 
  }

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


  static std::string c_mangle(const std::string &word)
  {
    if (word == "" || word[0] != '{') return word;
    std::string ans=word.substr(1,word.length()-2);
    subst(ans,"(","_b_");
    subst(ans,")","_d_");
    subst(ans,"+","_p_");
    subst(ans,"-","_m_");
    subst(ans,"__","");

    return ans;
  }

  static std::string scilab_mangle(const std::string &word)
  {
    if (word == "" || word[0] != '{') return word;
    std::string ans=word.substr(1,word.length()-2);
    subst(ans,"(","_b_");
    subst(ans,")","_d_");
    subst(ans,"+","_p_");
    subst(ans,"-","_m_");
    subst(ans,"__","");

    if (ans.length() > 10) {
      ans = "x" + utilities::md5(ans).substr(0,9);
    }

    return ans;
  }

  static std::string matlab_mangle(const std::string &word)
  {
    if (word == "" || word[0] != '{') return word;
    std::string ans=word.substr(1,word.length()-2);
    subst(ans,"(","_b_");
    subst(ans,")","_d_");
    subst(ans,"+","_p_");
    subst(ans,"-","_m_");
    subst(ans,"__","");

    //    if (ans.length() > 32) {
    //      ans = "x" + utilities::md5(ans).substr(0,9);
    //    }

    return ans;
  }

  void format_double(std::ostream &out, double x)
  {
    if (x == (long long) x) out << ((long long) x) << ".0";
    else {
      out << std::setprecision(15) << x;
    }
  }


  class CDoubleFormatter : public Formatter {
    virtual void formatPower(std::ostream &out, const symbolic::Power *e, int priorityContext) const
    {
      if (e->base != 0 && typeid(*e->base) == typeid(const Variable) && ((const Variable *)(e->base))->name == "%e") {
	out << "exp("; format(out,e->power); out << ")";
      } else {
	out << "pow("; format(out,e->base); out << ","; format(out,e->power); out << ")";
      }
    }

    virtual void formatVariable(std::ostream &out, const symbolic::Variable *e, int priorityContext) const
    {
      out << c_mangle(e->name);
    }

    virtual void formatNamedConstant(std::ostream &out, const symbolic::Variable *e, int priorityContext) const
    {
      if (e->name == "%e") {
	out << "M_E";
      } else if (e->name == "%pi") {
	out << "M_PI";
      } else {
	double x=e->Expression::namedConstants[e->name];
	if (x<0) out << "(";
	format_double(out,x);
	if (x<0) out << ")";
      }
    }

    virtual void formatConstant(std::ostream &out, const symbolic::Constant *e, int priorityContext) const
    {
      double x=e->value;
      if (x<0) out << "(";
      format_double(out,x);
      if (x<0) out << ")";
    }

  };

  static const CDoubleFormatter STATIC_C_DOUBLE_FORMATTER;
  const Formatter &C_DOUBLE_FORMATTER(STATIC_C_DOUBLE_FORMATTER);

  std::pair < const symbolic::Formatter * , const symbolic::Expression * > format_c_double(const symbolic::Expression *e)
  {
    return   std::pair < const symbolic::Formatter * , const symbolic::Expression * > (&C_DOUBLE_FORMATTER, e); 
  }

  void format_single(std::ostream &out, double x)
  {
    if (x == (long long) x) out << ((long long) x) << ".0f";
    else {
      out << std::setprecision(8) << x << "f";
    }
  }

  class CSingleFormatter : public Formatter {
    virtual void formatNaturalLog(std::ostream &out, const symbolic::NaturalLog *e, int priorityContext) const
    {
      out << "logf("; format(out,e->arg); out << ")";
    }

    virtual void formatPower(std::ostream &out, const symbolic::Power *e, int priorityContext) const
    {
      if (e->base != 0 && typeid(*e->base) == typeid(const Variable) && ((const Variable *)(e->base))->name == "%e") {
	out << "expf("; format(out,e->power); out << ")";
      } else {
	out << "powf("; format(out,e->base); out << ","; format(out,e->power); out << ")";
      }
    }

    virtual void formatVariable(std::ostream &out, const symbolic::Variable *e, int priorityContext) const
    {
      out << c_mangle(e->name);
    }

    virtual void formatNamedConstant(std::ostream &out, const symbolic::Variable *e, int priorityContext) const
    {
      if (e->name == "%e") {
	out << "float(M_E)";
      } else if (e->name == "%pi") {
	out << "float(M_PI)";
      } else {
	format_single(out,Expression::namedConstants[e->name]);
      }
    }

    virtual void formatConstant(std::ostream &out, const symbolic::Constant *e, int priorityContext) const
    {
      format_single(out,e->value);
    }

  };

  static const CSingleFormatter STATIC_C_SINGLE_FORMATTER;
  const Formatter &C_SINGLE_FORMATTER(STATIC_C_SINGLE_FORMATTER);

  std::pair < const symbolic::Formatter * , const symbolic::Expression * > format_c_single(const symbolic::Expression *e)
  {
    return   std::pair < const symbolic::Formatter * , const symbolic::Expression * > (&C_SINGLE_FORMATTER, e); 
  }


  class INIFormatter : public Formatter {
    virtual void formatConstant(std::ostream &out, const symbolic::Constant *e, int priorityContext) const
    {
      double x=e->value;
      if (x == 0) { out << "0"; return; }

      int power=(int) floor(log(fabs(x))/log(1000.0));
      if (power == 0) {
	format_double(out,x);
      } else {
	format_double(out, x / pow(10.0,3*power)); out << "*10^" << ((power < 0) ? "(" : "") << (3*power) << ((power < 0) ? ")" : "");
      }
    }
  };

  static const INIFormatter STATIC_INI_FORMATTER;
  const Formatter &INI_FORMATTER(STATIC_INI_FORMATTER);

  std::pair < const symbolic::Formatter * , const symbolic::Expression * > format_ini(const symbolic::Expression *e)
  {
    return   std::pair < const symbolic::Formatter * , const symbolic::Expression * > (&INI_FORMATTER, e); 
  }

  static void print_latex(std::ostream &out, const symbolic::Expression *e, int prec)
  {
    if (typeid(*e) == typeid(const symbolic::Sum)) {
      const symbolic::Sum *e_sum = (const symbolic::Sum *) e;
      if (prec > 1) { out << "\\left("; };
      out << "{";
      for (size_t i=0; i<e_sum->parts.size(); ++i) {
	if (i > 0) out << "+";
	print_latex(out,e_sum->parts[i],1);
      }
      out << "}";
      if (prec > 1) { out << "\\right)"; };
    } else if (typeid(*e) == typeid(const symbolic::Product)) {
      const symbolic::Product *e_product = (const symbolic::Product *) e;
      if (prec > 2) { out << "\\left("; };
      out << "{";
      for (size_t i=0; i<e_product->parts.size(); ++i) {
	print_latex(out,e_product->parts[i],2);
      }
      out << "}";
      if (prec > 2) { out << "\\right)"; };
    } else if (typeid(*e) == typeid(const symbolic::Power)) {
      const symbolic::Power *e_power = (const symbolic::Power *) e;
      if (prec >= 3) { out << "\\left("; };
      out << "{";
      print_latex(out,e_power->base,3);
      out << "}^{";
      print_latex(out,e_power->power,3);
      out << "}";      
      if (prec >= 3) { out << "\\right)"; };
    } else if (typeid(*e) == typeid(const symbolic::NaturalLog)) {
      const symbolic::NaturalLog *e_log = (const symbolic::NaturalLog *) e;
      if (prec >= 3) { out << "\\left("; };
      out << "{\\log\\left(";
      print_latex(out,e_log->arg,0);
      out << "\\right)}";
      if (prec >= 3) { out << "\\right)"; };
    } else if (typeid(*e) == typeid(const symbolic::Variable)) {
      const symbolic::Variable *e_variable = (const symbolic::Variable *) e;
      if (e_variable->name == "%pi") {
	out << "\\pi";
      } else if (e_variable->name.size()>0 && e_variable->name[0] == '%') {
	out << e_variable->name.substr(1);
      } else {
	if (e_variable->name.size() > 0 && e_variable->name[0] == '{') {
	  out << "{";

	  int state=0,pos=1,n=e_variable->name.size()-1;
	  while (pos < n) {
	    char c=e_variable->name[pos];
	  retry:
	    switch(state) {
	    case 0: 
	      if ('A' <= c && c <= 'Z') {
		out << "{\\text{" << c;
		state = 1;
	      } else if (c == '+' || c == '-') {
		out << "^{" << c;
		state = 2;
	      } else if ('0' <= c && c <= '9') {
		out << "_{" << c;
		state = 3;
	      } else {
		out << c;
	      }
	      break;
	    case 1:
	      if ('a' <= c && c <= 'z') {
		out << c;
	      } else {
		out << "}}";
		state=0;
		goto retry;
	      }
	      break;
	    case 2:
	      if (c == '+' || c == '-') {
		out << c;
	      } else {
		out << "}";
		state=0;
		goto retry;
	      }
	      break;
	    case 3:
	      if ('0' <= c && c <= '9') {
		out << c;
	      } else {
		out << "}";
		state=0;
		goto retry;
	      }
	      break;
	    }
	    ++pos;
	  }
	  switch(state) {
	  case 0: out << "}";   break;
	  case 1: out << "}}}"; break;
	  case 2: out << "}}";  break;
	  case 3: out << "}}";  break;
	  }
	} else {
	  out << e_variable->name;
	}
      }
    } else if (typeid(*e) == typeid(const symbolic::Constant)) {
      const symbolic::Constant *e_constant = (const symbolic::Constant *) e;
      double x=e_constant->value;

      if (prec >= 2 && x < 0) { out << "\\left("; }
      if ((fabs(x) < 1e9) && (x == (long long) x)) out << ((long long) x);
      else {
	int power=(int) floor(log(fabs(x))/log(1000.0));
	if (power == 0) {
	  out << std::setprecision(15) << x;
	} else {
	  out << std::setprecision(15) << x / pow(10.0,3*power);
	  bool paren = (power < 0 || power >= 10);
	  out << "\\times 10^" << (paren ? "(" : "") << (3*power) << (paren ? ")" : "");
	}
      }
      if (prec >= 2 && x < 0) { out << "\\right)"; }
    } else {
      assert(false);
    }
  }
      
  class LatexFormatter : public Formatter {
  public:
    virtual void format(std::ostream &out, const symbolic::Expression *e, int priorityContext) const
    {
      print_latex(out,e,0);
    }
  };

  static const LatexFormatter STATIC_LATEX_FORMATTER;
  const Formatter &LATEX_FORMATTER(STATIC_LATEX_FORMATTER);

  std::pair < const symbolic::Formatter * , const symbolic::Expression * > format_latex(const symbolic::Expression *e)
  {
    return   std::pair < const symbolic::Formatter * , const symbolic::Expression * > (&LATEX_FORMATTER, e); 
  }

  class ScilabFormatter : public Formatter {
    virtual void formatVariable(std::ostream &out, const symbolic::Variable *e, int priorityContext) const
    {
      out << scilab_mangle(e->name);
    }
  };

  static const ScilabFormatter STATIC_SCILAB_FORMATTER;
  const Formatter &SCILAB_FORMATTER(STATIC_SCILAB_FORMATTER);

  std::pair < const symbolic::Formatter * , const symbolic::Expression * > format_scilab(const symbolic::Expression *e)
  {
    return   std::pair < const symbolic::Formatter * , const symbolic::Expression * > (&SCILAB_FORMATTER, e); 
  }

  class MatlabFormatter : public Formatter {
    virtual void formatVariable(std::ostream &out, const symbolic::Variable *e, int priorityContext) const
    {
      out << matlab_mangle(e->name);
    }
  };

  static const MatlabFormatter STATIC_MATLAB_FORMATTER;
  const Formatter &MATLAB_FORMATTER(STATIC_MATLAB_FORMATTER);

  std::pair < const symbolic::Formatter * , const symbolic::Expression * > format_matlab(const symbolic::Expression *e)
  {
    return   std::pair < const symbolic::Formatter * , const symbolic::Expression * > (&MATLAB_FORMATTER, e); 
  }

  void print(Expression *e) { std::cout << format_plain(e) << std::endl; }
}
