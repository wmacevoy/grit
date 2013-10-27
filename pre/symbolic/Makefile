
DEPS=symbolic utilities
include ../build/make.top

all : dependencies lib/libsymbolic.a bin/testsymbolic bin/testsymbolic_parse bin/tester bin/examplesymbolic bin/testcoptgen bin/testexpressionpool bin/testsubstitute

src/symbolic_parse.tab.cpp src/symbolic_parse.tab.hpp : src/symbolic_parse.y
	bison -d -n -o src/symbolic_parse.tab.cpp src/symbolic_parse.y

src/symbolic_parse.cpp : src/symbolic_parse.l
	flex -osrc/symbolic_parse.cpp src/symbolic_parse.l

tmp/symbolic.o : src/symbolic.cpp include/symbolic.hpp ../utilities/include/md5.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/formatter.o : src/formatter.cpp include/formatter.hpp include/symbolic.hpp ../utilities/include/md5.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/coptgen.o : src/coptgen.cpp include/symbolic.hpp include/coptgen.hpp include/formatter.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/symbolic_parse.tab.o : src/symbolic_parse.tab.cpp include/symbolic.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/symbolic_parse.o : src/symbolic_parse.cpp src/symbolic_parse.tab.hpp src/symbolic_parse.hpp include/symbolic.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/tester.o : src/tester.cpp include/symbolic.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/testcoptgen.o : src/testcoptgen.cpp include/symbolic.hpp include/coptgen.hpp include/formatter.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/testexpressionpool.o : src/testexpressionpool.cpp include/symbolic.hpp include/coptgen.hpp include/formatter.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/testsubstitute.o : src/testsubstitute.cpp include/symbolic.hpp include/coptgen.hpp include/formatter.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/testsymbolic.o : src/testsymbolic.cpp include/symbolic.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/testsymbolic_parse.o : src/testsymbolic_parse.cpp include/symbolic.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

tmp/examplesymbolic.o : src/examplesymbolic.cpp include/symbolic.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

lib/libsymbolic.a : tmp/symbolic.o tmp/symbolic_parse.o tmp/symbolic_parse.tab.o tmp/coptgen.o tmp/formatter.o
	ar cr lib/libsymbolic.a tmp/symbolic.o tmp/symbolic_parse.o tmp/symbolic_parse.tab.o tmp/coptgen.o tmp/formatter.o

bin/testsymbolic : tmp/testsymbolic.o lib/libsymbolic.a
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS)

bin/testcoptgen : tmp/testcoptgen.o lib/libsymbolic.a
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS)

bin/testexpressionpool : tmp/testexpressionpool.o lib/libsymbolic.a
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS)

bin/testsubstitute : tmp/testsubstitute.o lib/libsymbolic.a
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS)

bin/testsymbolic_parse : tmp/testsymbolic_parse.o lib/libsymbolic.a
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS)

bin/tester : tmp/tester.o lib/libsymbolic.a
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS)

bin/examplesymbolic : tmp/examplesymbolic.o lib/libsymbolic.a 
	$(CXX) -o $@ $(CXXFLAGS) tmp/examplesymbolic.o $(LDFLAGS)

test : all
	./bin/testsymbolic
	./bin/testsymbolic_parse
	bash ./src/tests

include ../build/make.end
