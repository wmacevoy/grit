DEPS=preprocess prepreprocess symbolic utilities
include ../build/make.top

GENERATOR_OBJS=tmp/scilabcodegenerator.o tmp/matlabcodegenerator.o tmp/cppcodegenerator.o tmp/linearalgebra.o tmp/logtransform.o tmp/loglogtransform.o 
# GENERATOR_OBJS=tmp/cppcodegenerator.o tmp/linearalgebra.o tmp/logtransform.o tmp/scilabcodegenerator.o tmp/matlabcodegenerator.o
SOLVERS_CPP=tmp/solve_newton.cpp tmp/solve_damped_newton.cpp tmp/solve_empty.cpp

all : dependencies lib/libpreprocess.a include/preprocess.hpp bin/testconfiguration bin/preprocess

tmp/configuration.o : src/configuration.cpp include/configuration.hpp ../utilities/include/encode_string.hpp ../symbolic/include/symbolic.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/testconfiguration.o : src/testconfiguration.cpp include/configuration.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/codegenerator.o : src/codegenerator.cpp include/codegenerator.hpp include/configuration.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/logtransform.o : src/logtransform.cpp include/codegenerator.hpp include/configuration.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/loglogtransform.o : src/loglogtransform.cpp include/codegenerator.hpp include/configuration.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

lib/libpreprocess.a : tmp/configuration.o tmp/codegenerator.o
	ar cr lib/libpreprocess.a tmp/configuration.o tmp/codegenerator.o

include/preprocess.hpp : include/configuration.hpp include/codegenerator.hpp
	touch $@

tmp/linearalgebra.cpp : src/linearalgebra.cppp ../prepreprocess/bin/prepreprocess 
	../prepreprocess/bin/prepreprocess -prefix "o()" -out $@ -in $<

tmp/scilabcodegenerator.cpp : src/scilabcodegenerator.cppp ../prepreprocess/bin/prepreprocess 
	../prepreprocess/bin/prepreprocess -prefix "o()" -out $@ -in $<

tmp/scilabcodegenerator.o : tmp/scilabcodegenerator.cpp include/preprocess.hpp ../symbolic/include/symbolic.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/matlabcodegenerator.cpp : src/matlabcodegenerator.cppp ../prepreprocess/bin/prepreprocess 
	../prepreprocess/bin/prepreprocess -prefix "o()" -out $@ -in $<

tmp/matlabcodegenerator.o : tmp/matlabcodegenerator.cpp include/preprocess.hpp ../symbolic/include/symbolic.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/cppcodegenerator.cpp : src/cppcodegenerator.cppp ../prepreprocess/bin/prepreprocess 
	../prepreprocess/bin/prepreprocess -prefix "o()" -out $@ -in $<

tmp/cppcodegenerator.o : tmp/cppcodegenerator.cpp $(SOLVERS_CPP) src/linearalgebra.hpp include/preprocess.hpp ../symbolic/include/symbolic.hpp ../symbolic/include/coptgen.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/linear_solve.cpp : src/linear_solve.cppp
	../prepreprocess/bin/prepreprocess -prefix "o()" -out $@ -in $<

tmp/solve_newton.cpp : src/solve_newton.cppp
	../prepreprocess/bin/prepreprocess -prefix "o()" -out $@ -in $<

tmp/solve_damped_newton.cpp : src/solve_damped_newton.cppp
	../prepreprocess/bin/prepreprocess -prefix "o()" -out $@ -in $<

tmp/solve_empty.cpp : src/solve_empty.cppp
	../prepreprocess/bin/prepreprocess -prefix "o()" -out $@ -in $<

tmp/linearalgebra.o : tmp/linearalgebra.cpp tmp/linear_solve.cpp include/linearalgebra.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/preprocess.o : src/preprocess.cpp include/codegenerator.hpp include/configuration.hpp ../utilities/include/encode_string.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<


bin/testconfiguration : tmp/testconfiguration.o lib/libpreprocess.a ../symbolic/lib/libsymbolic.a ../utilities/lib/libutilities.a
	$(CXX) $(CXXFLAGS) -o $@ tmp/testconfiguration.o $(LDFLAGS)

bin/preprocess : tmp/preprocess.o $(GENERATOR_OBJS) lib/libpreprocess.a ../symbolic/lib/libsymbolic.a ../utilities/lib/libutilities.a
	$(CXX) $(CXXFLAGS) -o $@ tmp/preprocess.o $(GENERATOR_OBJS) $(LDFLAGS)

include ../build/make.end

