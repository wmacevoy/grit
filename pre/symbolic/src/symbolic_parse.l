%option prefix="symbolic_parse_"
%option noyywrap

%{
#define DEBUG 0
#include "symbolic_parse_internal.hpp"
#include "symbolic_parse.tab.hpp"
YYSTYPE symbolic_parse_result=0;
using namespace symbolic;
using namespace std;
%}
NUMBER      [0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?
VARIABLE    ([A-Za-z%][A-Za-z0-9_$%]*|\{[^{}]+\})
OP          [-()+*/^,]
WS          [ \t\n\r]
%% 
{NUMBER} {
   if (DEBUG) { cerr << "NUMBER: " << yytext << endl; }
   symbolic_parse_lval = new Constant(atof(yytext));
   return NUMBER;
}

{VARIABLE} {
   if (DEBUG) { cerr << "VARIABLE: " << yytext << endl; }
   symbolic_parse_lval = new Variable(yytext);
   return VARIABLE;
}

{OP} {
   if (DEBUG) { cerr << "OP: " << yytext << endl; }
  symbolic_parse_lval = 0;
  return yytext[0];
}

{WS} { 
   if (DEBUG) { cerr << "WS: " << yytext << endl; }
}

. { 
   if (DEBUG) { cerr << "?: " << yytext << endl; }

   std::ostringstream oss;
   oss << "unsupported character in stream: " << yytext << ".";
   symbolic::Expression::UnsupportedSyntax exception;
   exception.message = oss.str();
   throw exception;
}

%%

int symbolic_parse_error(const char *msg)
{
//  std::cerr << "error: " << msg << std::endl;
  return 0;
}

bool symbolic_parse(const std::string &expression, symbolic::Expression *&result)
{
  YY_BUFFER_STATE yy_string_buffer=yy_scan_string(expression.c_str());
  int status=symbolic_parse_parse();
  yy_delete_buffer(yy_string_buffer);

  if (status==0) {
     result=symbolic_parse_result;
     return true;
  } else {
     result=0;
     return false;
  }
}

namespace symbolic
{
  bool parse(const std::string &e_str, Expression *&e_sym)
  {
    Expression *e_tmp=0;
    if (!symbolic_parse(e_str,e_tmp)) {
      delete e_tmp;
      return false;
    } else {
      e_sym=symbolic::simplify(e_tmp);
      delete e_tmp;
      return true;
    }
  }

  Expression *parse(const std::string &e_str)
  {
    Expression *e_sym=0;
    if (!parse(e_str,e_sym)) {
      Expression::UnsupportedSyntax exception;
      exception.message = "not a supported multinomial expression: \"";
      exception.message += e_str;
      exception.message += "\"";
      throw exception;
    }
    return e_sym;
  }
}

