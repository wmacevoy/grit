#ifndef COPTGEN_HPP
#define COPTGEN_HPP

#include <map>
#include <list>
#include <string>

#include "symbolic.hpp"
#include "formatter.hpp"

namespace symbolic {

  class COptGen
  {
  public:
    Format *format;
    class E {
    public: 
      Expression *e;
      E(Expression *_e = 0);
      bool operator<(const E&to) const;
      bool operator==(const E&to) const;
      ~E();
    };
  
    std::string type;
    std::string prefix;

    int count;
    std::map< E , std::list < std::string > > *assignments;

    COptGen();
    ~COptGen();
    std::string gettmp(int id);
    int nexttmpid();

    std::list < std::string > declarations;
    std::list < std::string > code;

    Expression * newtmp(Expression *e);

    Expression * eval(Expression *e);

    void assign(symbolic::Expression *lhs, symbolic::Expression *rhs);
    void assign(const std::string &lhs, symbolic::Expression *rhs);
    void assign(const std::string &lhs, const std::string &rhs);
    void declare(std::ostream &out) const;
    void define(std::ostream &out) const;
    void print(std::ostream &out) const;
  };

  inline std::ostream & operator<< (std::ostream &out, const COptGen &coptgen) 
  {
    coptgen.print(out);
    return out;
  }
}


#endif
