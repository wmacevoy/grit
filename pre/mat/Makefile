DEPS=mat symbolic utilities
include ../build/make.top

all : dependencies lib/libmat.a bin/testmat

tmp/mat.o : src/mat.cpp include/mat.h ../symbolic/include/symbolic.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/testmat.o : src/testmat.cpp include/mat.h ../symbolic/include/symbolic.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
lib/libmat.a : tmp/mat.o
	ar cr $@ tmp/mat.o

bin/testmat : tmp/testmat.o lib/libmat.a ../symbolic/lib/libsymbolic.a
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS)

test : all
	./bin/testmat

include ../build/make.end
