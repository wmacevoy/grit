#ifndef SYMBOLIC_PARSE_HPP
#define SYMBOLIC_PARSE_HPP

#include "symbolic.hpp"
#include <string>

bool symbolic_parse(const std::string &expression, symbolic::Expression *&result);

#endif
