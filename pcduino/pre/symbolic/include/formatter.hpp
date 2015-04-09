#ifndef SYMBOLIC_FORMATTER_HPP
#define SYMBOLIC_FORMATTER_HPP

#include "symbolic.hpp"
#include <iostream>

namespace symbolic {

  class Formatter 
  {
  public:

    enum { 
      PRIORITY_NONE=0, 
      PRIORITY_SUM=1, 
      PRIORITY_PRODUCT=2, 
      PRIORITY_TERM=3,
      PRIORITY_POWER=4
    };

    virtual void formatSum(std::ostream &out, const symbolic::Sum *e, int priorityContext) const;
    virtual void formatProduct(std::ostream &out, const symbolic::Product *e, int priorityContext) const;
    virtual void formatPower(std::ostream &out, const symbolic::Power *e, int priortyContext) const;
    virtual void formatCos(std::ostream &out, const symbolic::Cos *e, int priorityContext) const;
    virtual void formatSin(std::ostream &out, const symbolic::Sin *e, int priorityContext) const;
    virtual void formatNaturalLog(std::ostream &out, const symbolic::NaturalLog *e, int priorityContext) const;
    virtual void formatVariable(std::ostream &out, const symbolic::Variable *e, int priorityContext) const;
    virtual void formatNamedConstant(std::ostream &out, const symbolic::Variable *e, int priorityContext) const;
    virtual void formatConstant(std::ostream &out, const symbolic::Constant *e, int priorityContext) const;
    virtual void formatStartGroup(std::ostream &out) const;
    virtual void formatEndGroup(std::ostream &out) const;
    virtual void format(std::ostream &out, const symbolic::Expression *e, int priorityContext=PRIORITY_NONE) const;
    virtual void format(std::string &str, const symbolic::Expression *e, int priorityContext=PRIORITY_NONE) const;
    virtual ~Formatter();
  };

  extern const Formatter &INI_FORMATTER;
  extern const Formatter &LATEX_FORMATTER;
  extern const Formatter &C_DOUBLE_FORMATTER;
  extern const Formatter &C_SINGLE_FORMATTER;
  extern const Formatter &SCILAB_FORMATTER;
  extern const Formatter &MATLAB_FORMATTER;

  typedef std::pair < const symbolic::Formatter * , const symbolic::Expression * > FormatResult;
  typedef FormatResult Format(const symbolic::Expression *);

  FormatResult format_plain(const symbolic::Expression *e);
  FormatResult format_latex(const symbolic::Expression *e);
  FormatResult format_ini(const symbolic::Expression *e);
  FormatResult format_c_single(const symbolic::Expression *e);
  FormatResult format_c_double(const symbolic::Expression *e);
  FormatResult format_scilab(const symbolic::Expression *e);
  FormatResult format_matlab(const symbolic::Expression *e);

  inline std::ostream& operator<< ( std::ostream &out, const FormatResult &fr)
  {
    fr.first->format(out,fr.second);
    return out;
  }

  void print(Expression *e);
}

#endif
