preprocess -- package to make solvers for systems of multinomial equations

preprocess can read .ini files describing a system of equations, and
generate code to solve (or assist in solving) that system of equations.

to build

   make all
   -- recursively builds ../symbolic, which this depends on.

   builds

   lib/libpreprocess.a
   bin/preprocess
   bin/testconfiguration

to test

   none available

to clean

   make clean

to use

   see ../sample for example solvers

to extend

   see cppcodegenerator.cpp and scilabcodegenerator.cpp and the Makefile
   to add additional codegenerators to the the preprocessor.
