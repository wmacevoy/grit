symbolic -- package to work with multinomial symbolic expressions

Symbolic supports double-precision symbolic expressions composed of
the following elements:

    numeric double-precision constants
         Example: 0,1,0.5

         Note: when simplified, a zero-valued expression represented as a NULL expression.

    named constants: %e and %pi
     - additional named constants can be added by adding them to
         Expression::namedConstants.  For example

	 Expression::namedConstants["%phi"]=(sqrt(5)-1)/2;

         Named constants must start with a percent (%) character,
         followed by zero or more alphanumeric, underbar (_), dollar
         ($) and/or percent (%) characters.

     -  Examples: %pi, %e

     - named constants are maintained in an expression as a
       symbolic::Variable with a % as the first character in the
       name.
   

    variables
         can be either

	 - start with an alphabetic character followed by zero or more
         alphanumeric, underbar (_), dollar ($) and/or percent (%)
         characters. 

	 - start with an open curly brace ({) symbol, have any characters
           *except* a close curly brace (}) symbol, and a close curly
	   brace symbol.

	 Examples: x, reaction_rate, {log(NaCl in mol/l)}
	 
     
Operations (from highest to lowest precidence)
        Power: ^
	Unary: +,-
	Binary: *,/
	Binary: +,-

        Example: (%e^(-lambda)+%e^(+lambda))/(2*lambda)

Creation:

	Expression trees can be created by parsing a string, or
	through the constructors, or function builders

        Expression *e = parse("(%e^(-lambda)+%e^(+lambda))/(2*lambda)");

        Expression *e =
          new Power(new Variable("%e"),new Variable("lambda"));

Transform:

       These create new expresions, built from deep copies of the
       original expression:

       Exression *y2 = clone(y1);
       Expression *dydx = differentiate(e,"x");
       Expression *se = simplify(e);


Use

       To evaluate an expression under a given assignment of values:

       std::map < std::string , double > vars;
       vars["x"]=3.0; vars["y"]=2.0;
       double ans = evaluate(e,vars);

       To find what symbols (not including named constants) an
       expression depends on,

       std::set < std::string > varset = symbols(e)

       To check if an expression depends on no variables, you can
       simply use isconstant:

       bool fixed = isconstant(e)

      To generate a string version of an expression, simply direct it
      to a stream under a specific format:

      cout << "x=" << format_plain(x) << endl;

      Supported formats are:

       format_ini, format_c_double, format_c_single, format_scilab

      These formats are available by including the "formatter.hpp"
      header file.

      The format is one of three types, controlled by
      Expression::outMode

      Expression::OUT_MODE_C_<SINGLE|DOUBLE>
        C-compatible syntax, with:
        - %e -> M_E, and %pi ->M_PI
        - other named constants are replaced by their value mapped in Expression::namedConstants
        - x^y -> pow(x,y)
        - {complex} variables are mangled.
      
      Expression::OUT_MODE_INI
        INI-file compatible syntax (parse() compatible), with no
        translations.

      Expression::OUT_MODE_SCI
        SciLAB compatible syntax.
        - %e and %pi values are not changed
        - {complex} variables are mangled (when short < 10 chars), 
           and replaced by xMD5-10, where MD5-10 is a the first
           10 hex digits of the MD5 hash of the C-mangled value

to build

   make all

   builds

   lib/libsymbolic.a
   bin/testsymbolic
   bin/testsymbolic_parse
   bin/tester
   bin/examplesymbolic

to test

   make test

to clean

   make clean

to use

   see examplesymbolic.cpp
