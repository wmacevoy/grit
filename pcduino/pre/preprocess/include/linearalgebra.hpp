#ifndef LINEARALGEBRA_HPP
#define LINEARALGEBRA_HPP

#include <iostream>
#include <string>
#include <map>

namespace preprocess {
  class CodeGenerator;

  class LinearAlgebra {
  public: std::string target;
  public: std::string type;
  public: std::string prefix;
  public: std::string ns;

  public: std::map < std::string , std::string > flags;
  public: CodeGenerator *generator;
  public: void configure();
  public: std::string flag(const std::string &name, const std::string defval="");
  public: std::ostream& o();
  public: void prototype_dist2(const std::string &end=";");
  public: void prototype_dist(const std::string &end=";");
  public: void prototype_norm2(const std::string &end=";");
  public: void prototype_norm(const std::string &end=";");
  public: void prototype_linear_solve(const std::string &end=";");
  public: void prototype_all();
  public: void define_dist2();
  public: void define_dist();
  public: void define_norm2();
  public: void define_norm();
  public: void define_linear_solve();
  public: void define_all();
  };
};

#endif
