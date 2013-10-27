#ifndef SYMBOLIC_PARSE_INTERNAL_HPP
#define SYMBOLIC_PARSE_INTERNAL_HPP

#include <iostream>
#include <sstream>
#include "symbolic.hpp"
#include "symbolic_parse.hpp"

typedef symbolic::Expression* ExpressionPtr;
#define YYSTYPE ExpressionPtr
int symbolic_parse_error(const char *msg);
int symbolic_parse_lex();
int symbolic_parse_parse();
extern YYSTYPE symbolic_parse_lval;
extern YYSTYPE symbolic_parse_result;

#endif
