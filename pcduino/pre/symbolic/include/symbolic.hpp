#ifndef SYMBOLIC_HPP
#define SYMBOLIC_HPP

// #define SYMBOLIC_PRINT
// #define SYMBOLIC_OUT_MODE

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>

namespace symbolic {

  class NamedConstants : public std::map < std::string , double > 
  {
  public:
    NamedConstants();
  };

  class Expression
  {
  protected:
    Expression();
  private:
    Expression(const Expression &);
  public:

#ifdef SYMBOLIC_OUT_MODE
    enum { OUT_MODE_NONE=0,OUT_MODE_C_SINGLE=1, OUT_MODE_C_DOUBLE=2, OUT_MODE_SCI=3, OUT_MODE_INI=4, OUT_MODE_LATEX=5};
    static int outMode;
#endif
    static NamedConstants namedConstants;

    class UnsupportedOperation { public: std::string message; };
    class UnsupportedDifferentiation : public UnsupportedOperation {};
    class UnsupportedEvaluation : public UnsupportedOperation {};
    class UnsupportedSyntax : public UnsupportedOperation {};
    static const std::map < std::string , double > NONE;
    virtual bool isConstant() const=0;
    virtual int compareTo(const Expression *e) const=0;
    virtual double evaluate(const std::map<std::string, double> &values=NONE) const=0;
#ifdef SYMBOLIC_PRINT
    virtual void print(std::ostream &out) const=0;
#endif
    virtual Expression* clone() const=0;
    virtual Expression* differentiate(const std::string &variable) const=0;
    virtual Expression* simplify() const=0;
    virtual void symbols(std::set<std::string> &names) const=0;
    virtual ~Expression();
  };

  
  class NaturalLog : public Expression
  {
  public:
    Expression *arg;
    explicit NaturalLog(Expression *_arg);
    virtual bool isConstant() const;
    virtual double evaluate(const std::map<std::string, double> &values=NONE) const;
#ifdef SYMBOLIC_PRINT
    virtual void print(std::ostream &out) const;
#endif
    virtual Expression* clone() const;
    virtual int compareTo(const Expression *e) const;

    virtual Expression* differentiate(const std::string &variable) const;
    virtual Expression* simplify() const;
    virtual void symbols(std::set<std::string> &names) const;
    ~NaturalLog();
  };

  class Cos : public Expression
  {
  public:
    Expression *arg;
    explicit Cos(Expression *_arg);
    virtual bool isConstant() const;
    virtual double evaluate(const std::map<std::string, double> &values=NONE) const;
#ifdef SYMBOLIC_PRINT
    virtual void print(std::ostream &out) const;
#endif
    virtual Expression* clone() const;
    virtual int compareTo(const Expression *e) const;

    virtual Expression* differentiate(const std::string &variable) const;
    virtual Expression* simplify() const;
    virtual void symbols(std::set<std::string> &names) const;
    ~Cos();
  };

  class Sin : public Expression
  {
  public:
    Expression *arg;
    explicit Sin(Expression *_arg);
    virtual bool isConstant() const;
    virtual double evaluate(const std::map<std::string, double> &values=NONE) const;
#ifdef SYMBOLIC_PRINT
    virtual void print(std::ostream &out) const;
#endif
    virtual Expression* clone() const;
    virtual int compareTo(const Expression *e) const;

    virtual Expression* differentiate(const std::string &variable) const;
    virtual Expression* simplify() const;
    virtual void symbols(std::set<std::string> &names) const;
    ~Sin();
  };

  class Product : public Expression
  {
  public:
    std::vector<Expression*> parts;
    Product();
    Product(Expression *factor0, Expression *factor1);
    virtual bool isConstant() const;
    virtual double evaluate(const std::map<std::string, double> &values=NONE) const;
    virtual Expression* clone() const;
    virtual int compareTo(const Expression *e) const;
#ifdef SYMBOLIC_PRINT
    virtual void print(std::ostream &out) const;
#endif
    virtual Expression* differentiate(const std::string &variable) const;
    virtual Expression* simplify() const;
    virtual void symbols(std::set<std::string> &names) const;
    ~Product();
  };

  class Sum : public Expression
  {
  public:
    std::vector<Expression*> parts;
    Sum();
    Sum(Expression *term0, Expression *term1);
    virtual bool isConstant() const;
    virtual double evaluate(const std::map<std::string, double> &values=NONE) const;
#ifdef SYMBOLIC_PRINT
    virtual void print(std::ostream &out) const;
#endif
    virtual Expression* clone() const;
    virtual int compareTo(const Expression *e) const;
    virtual Expression* differentiate(const std::string &variable) const;
    virtual Expression* simplify() const;
    virtual void symbols(std::set<std::string> &names) const;
    ~Sum();
  };

  class Power : public Expression
  {
  public:
    Power();
    Power(Expression *_base, Expression *_power);
    Expression *base;
    Expression *power;
    virtual bool isConstant() const;
    virtual double evaluate(const std::map<std::string, double> &values=NONE) const;
#ifdef SYMBOLIC_PRINT
    virtual void print(std::ostream &out) const;
#endif
    virtual Expression* clone() const;
    virtual int compareTo(const Expression *e) const;

    virtual Expression* differentiate(const std::string &variable) const;
    virtual Expression* simplify() const;
    virtual void symbols(std::set<std::string> &names) const;
    ~Power();
    
  };

  class Variable : public Expression
  {
  public:
    std::string name;
    Variable();
    static std::string mangle(const std::string &word);
    explicit Variable(const std::string &_name);
    virtual bool isConstant() const;
    virtual double evaluate(const std::map<std::string, double> &values=NONE) const;
#ifdef SYMBOLIC_PRINT
    virtual void print(std::ostream &out) const;
#endif
    virtual Expression* clone() const;
    virtual int compareTo(const Expression *e) const;

    virtual Expression* differentiate(const std::string &variable) const;
    virtual Expression* simplify() const;
    virtual void symbols(std::set<std::string> &names) const;
    ~Variable();
  };


  class Constant : public Expression
  {
  public:
    double value;
    Constant();
    explicit Constant(double _value);
    virtual bool isConstant() const;
    virtual double evaluate(const std::map<std::string, double> &values=NONE) const;
#ifdef SYMBOLIC_PRINT
    virtual void print(std::ostream &out) const;
#endif
    virtual Expression* clone() const;
    virtual int compareTo(const Expression *e) const;

    virtual Expression* differentiate(const std::string &variable) const;
    virtual Expression* simplify() const;
    virtual void symbols(std::set<std::string> &names) const;
    ~Constant();
  };

#ifdef SYMBOLIC_PRINT
  std::ostream &operator<< (std::ostream &out, const Expression *e);
#else
  template <typename STREAM>
  STREAM &operator<< (STREAM &out, const Expression *e)
  {
    return out.not_supported(); // not supported
  }
#endif


  // duplicate an expression
  Expression* clone(const Expression *e);

  //
  // create an expresion from a constant
  //
  Expression* constant(const double x);

  // simplify a given expression (or duplicate if already simple)
  Expression* simplify(const Expression *e);

  // substitute expressions for variables in an expression
  Expression* substitute(const std::map < std::string , const Expression * > &substitutions , const Expression *e);

  //
  // differentate with respect to an expression.  Note differentiation
  // with respect to symbols in the exponent is not supported, since
  // the result is not a multinomial.
  //
  // May throw Expression::UnsupporedDifferentiation exception.
  //
  Expression* differentiate(const Expression *e, const std::string &variable);

  //
  // determine if a given expression is a constant (depends on no symbols)
  //
  bool isconstant(const Expression *e);

  //
  // determine the symbols an expression depends on
  //
  std::set<std::string> symbols(const Expression *e);

  //
  // Evaluate an expression for certain values of the symbols.  Each
  // symbol in the expression must have a value assigned to it.
  //
  // May throw an Expression::UnsupportedEvaluation exception.
  //
  double evaluate(const Expression *e, const std::map<std::string, double> &values=Expression::NONE);

  //
  // Parse a multinomial expression.  Expressions may contain:
  //
  //  "+" "-" unary or binary addition and substraction
  //  "*" "/" binary multiplication and division
  //  "^" binary exponentiation
  //  "(" ")" grouping
  //  log() and exp() functions
  //  constants are double precision
  //  variables are
  //    1) a letter or underscore followed by 0 or more letters,
  //       underscores, and digits, or
  //
  //    2) an open curly "{" followed by any characters except
  //       a close curly "}" follewed by a close curly.
  //
  // this parse version returns false with a syntax error.
  bool parse(const std::string &e_str, Expression *&e_sym);

  //
  // like parse() above, but instead of returning false,
  // may throw an Expression::UnsupportedSyntax exception.
  //
  Expression *parse(const std::string &e_str);

  // compare two expressions.  They will be equal if they are
  // the same expressions algebraically.  The ordering of
  // of unequal expressions has nothing to do with their
  // algebraic ordering; it is only a way to allow expressions
  // to be the key in structures like maps.
  //
  int compare(const Expression *a, const Expression *b);

  class expression_order {
  public:
    bool operator()(const Expression * const &a, const Expression * const &b) const;
  };

  class expression_equals {
  public:
    bool operator()(const Expression * const &a, const Expression * const &b) const;
  };

  class ExpressionPool {
  public:
    typedef std::set < Expression * , expression_order > Pool;
    typedef Pool::iterator iterator;
    typedef Pool::const_iterator const_iterator;
  private:
#ifdef DEBUG_POOL
    class DbgPool : public Pool {
    public:
      std::pair < iterator , bool > insert(Expression *e);
    };
    DbgPool pool;
#else
    Pool pool;
#endif

    Pool simplified;
    std::map < std::string , Expression * > parsed;
    void delete_unpooled(Expression *&e);
  public:
    size_t size() const;
    Expression *insert(Expression *&e);
    Expression *constant(double value);
    Expression *variable(const std::string &name);
    Expression *log(Expression *arg);
    Expression *power(Expression *base, Expression *power);
    Expression *sum(Expression *a, Expression *b);
    Expression *product(Expression *a, Expression *b);
    Expression *parse(const std::string &s);
    Expression *clone(const Expression *e) const;

    Expression *simplify(const Expression *e);
    Expression *differentiate(const Expression *e, const std::string &x);
    Expression *differentiate(const Expression *e, const Expression *x);
    ~ExpressionPool();
  };

};

#endif
