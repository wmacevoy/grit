#include "coptgen.hpp"
#include <typeinfo>
#include <sstream>
#include <assert.h>

using namespace std;
using namespace symbolic;

namespace symbolic
{
  COptGen::E::E(symbolic::Expression *_e) : e(_e) {}
  bool COptGen::E::operator<(const E&to) const 
  { 
    return compare(this->e,to.e)<0; 
  }

  bool COptGen::E::operator==(const E&to) const 
  {
    return compare(this->e,to.e)==0; 
  }

  COptGen::E::~E() {}
  
  COptGen::COptGen() 
  { 
    type="double";
    format = &symbolic::format_c_double;
    prefix="";
    count=0;
    assignments = new std::map< E , std::list < std::string > > ();
  }

  COptGen::~COptGen()
  {
    std::list < Expression * > cleanup;
    for (std::map< E , std::list < std::string > > :: iterator i = assignments->begin(); i!=assignments->end(); ++i) {
      cleanup.push_back(i->first.e);
    }
    delete assignments;
    for (std::list < Expression * > :: iterator i = cleanup.begin(); i!=cleanup.end(); ++i) {
      delete *i;
    }
  }
  
  std::string COptGen::gettmp(int id) 
  {
    std::ostringstream oss;
    oss << prefix << "tmp" << id;
    return oss.str();
  }

  int COptGen::nexttmpid()
  {
    return count++;
  }

  symbolic::Expression * COptGen::newtmp(symbolic::Expression *e)
  {
    int id=nexttmpid();
    std::string tmp=gettmp(id);
    declarations.push_back(tmp);
    
    std::ostringstream oss;
    oss << tmp << "=" << format(e) << ";";
    code.push_back(oss.str());
    (*assignments)[E(e)].push_back(tmp);
    
    return new Variable(tmp);
  }
  

  symbolic::Expression * COptGen::eval(symbolic::Expression *e)
  {
    if (symbolic::isconstant(e) || typeid(*e) == typeid(Variable)) {
      return symbolic::clone(e);
    }

    std::map< E , std::list < std::string > > :: iterator i 
      = assignments->find(E(e));

    if (i != assignments->end() && ! i->second.empty()) {
      return new symbolic::Variable(*(i->second.begin()));
    }

    if (typeid(*e) == typeid(NaturalLog)) {
      NaturalLog *log_e = (NaturalLog *) e;
      Expression *arg_e = eval(log_e->arg);
      return newtmp(new NaturalLog(arg_e));
    } else if (typeid(*e) == typeid(Cos)) {
      Cos *cos_e = (Cos *) e;
      Expression *arg_e = eval(cos_e->arg);
      return newtmp(new Cos(arg_e));
    } else if (typeid(*e) == typeid(Sin)) {
      Sin *sin_e = (Sin *) e;
      Expression *arg_e = eval(sin_e->arg);
      return newtmp(new Sin(arg_e));
    } else if (typeid(*e) == typeid(Product)) {
      Product *prod_e = (Product *) e;
      Product *prod = new Product();
      for (std::vector < Expression * > :: iterator i = prod_e->parts.begin(); i!=prod_e->parts.end(); ++i) {
	prod->parts.push_back(eval(*i));
      }
      return newtmp(prod);
    } else if (typeid(*e) == typeid(Sum)) {
      Sum *sum_e = (Sum *) e;
      Sum *sum = new Sum();
      for (std::vector < Expression * > :: iterator i = sum_e->parts.begin(); i!=sum_e->parts.end(); ++i) {
	sum->parts.push_back(eval(*i));
      }
      return newtmp(sum);
    } else if (typeid(*e) == typeid(Power)) {
      Power *power_e = (Power*)e;
      return newtmp(new Power(eval(power_e->base),eval(power_e->power)));
    } else if (typeid(*e) == typeid(Variable)) {
      return clone(e);
    } else if (typeid(*e) == typeid(Constant)) {
      return clone(e);
    } else {
      assert(false);
    }

    return 0;
  }

  void COptGen::assign(symbolic::Expression *lhs, symbolic::Expression *rhs)
  {
    assert(typeid(*lhs) == typeid(symbolic::Variable));
    symbolic::Expression *val=eval(rhs);
    std::ostringstream oss;
    oss << format(lhs) << "=" << format(val) << ";";
    code.push_back(oss.str());
    (*assignments)[E(clone(rhs))].push_back(((symbolic::Variable*)lhs)->name);
  }

  void COptGen::assign(const std::string &lhs, symbolic::Expression *rhs)
  {
    symbolic::Variable var(lhs);
    assign(&var,rhs);
  }

  void COptGen::assign(const std::string &lhs, const std::string &rhs)
  {
    symbolic::Variable var(lhs);
    symbolic::Expression *rhs_sym = parse(rhs);
    assign(&var,rhs_sym);
    delete rhs_sym;
  }

  void COptGen::print(std::ostream &out) const
  {
    declare(out);
    define(out);
  }

  
  void COptGen::declare(std::ostream &out) const
  {
    for (std::list < std::string > :: const_iterator i = declarations.begin(); i!=declarations.end(); ++i) {
      out << type << " " << prefix << *i << ";" << std::endl;
    }
  }
  
  void COptGen::define(std::ostream &out) const
  {
    for (std::list < std::string > :: const_iterator i = code.begin(); i!=code.end(); ++i) {
      out << *i << std::endl;
    }
  }
};
