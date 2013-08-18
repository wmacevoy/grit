1. All source files NOT starting with main_ or test_ are compiled into a 
   static (.a) and dynamic (.so) library in lib
2. Any main_XXX.cpp file corresponds to bin/XXX applications which are 
   dynamically linked against the above library (and any libraries 
   in the dependencies list)
3. Any test_XXX.cpp file corresponds to bin/test_XXX (presumably test) 
   applications created like 2.
4. The DEPS flag gives the other projects that need to be built to make 
   this up to date.  The libs, and DEPS projects include directories and
    linking against the corresponding libraries is automatic.

The DEPS, MY_FLAGS and MY_LIBS flags, and the all target are all that 
  have to be modified for a specific component that are included in the 
  g++ / gcc command lines.  The make files are really very small with 
  the following conventions

make clean
make all
make libs 
make progs
make tests
make depsall
make depsclean

The deps version updates the dependencies (other than libs) as well.  
Deps clean on the third party is pretty slow and since we aren't editing
 that should very rarely need to be done.
