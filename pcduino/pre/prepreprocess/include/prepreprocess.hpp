#ifndef PREPREPROCESS_HPP
#define PREPREPROCESS_HPP

#include <iostream>
#include <string>

namespace preprocess {

  class Prepreprocess
  {
  public:
    std::string file;
    std::string prefix;
    std::string separator;
    std::string suffix;
    std::ostream *out;
    std::istream *in;

    enum { 
      START, // before generating anything on a line
      READY, // ready for next term or to terminate expression
      SEPARATED, // separator emitted
      LITERAL // in middle of string quote
    };
    int state;

    Prepreprocess();

    void emit_char(char c);
    void emit_expression(const std::string &expression);
    void emit_end(bool use_suffix=true);
    
    void exact_quote(const std::string &line);
    void magic_quote(const std::string &line);
    void process();
  };
  
}

#endif
