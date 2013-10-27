DEPS=prepreprocess
include ../build/make.top

all : lib/libprepreprocess.a bin/prepreprocess bin/exampleprepreprocess

tmp/prepreprocess.o : src/prepreprocess.cpp include/prepreprocess.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<


tmp/prepreprocess_main.o : src/prepreprocess_main.cpp include/prepreprocess.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

bin/prepreprocess : tmp/prepreprocess_main.o lib/libprepreprocess.a
	$(CXX) -o $@ $(CXXFLAGS) tmp/prepreprocess_main.o $(LDFLAGS)

lib/libprepreprocess.a : tmp/prepreprocess.o
	ar cr $@ tmp/prepreprocess.o

src/exampleprepreprocess.cpp : src/exampleprepreprocess.cppp bin/prepreprocess
	./bin/prepreprocess -out $@ -in $<

tmp/exampleprepreprocess.o : src/exampleprepreprocess.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

bin/exampleprepreprocess : lib/libprepreprocess.a tmp/exampleprepreprocess.o
	$(CXX) $(CXXFLAGS) -o $@ tmp/exampleprepreprocess.o $(LDFLAGS)

include ../build/make.end
