DEPS=mat preprocess prepreprocess symbolic utilities
include ../build/make.top

all : dependencies bin/gik ik_tip.ini include/ik_tip.hpp src/ik_tip.cpp bin/test_ik_tip lib/libik.a

lib/libik.a : tmp/ik_tip.o
	ar cr lib/libik.a tmp/ik_tip.o

tmp/gik.o : src/gik.cpp ../mat/include/mat.h ../symbolic/include/symbolic.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

src/gsolve.cpp : src/gsolve.cppp ../prepreprocess/bin/prepreprocess
	../prepreprocess/bin/prepreprocess -prefix "out" -out "$@" -in "$<"

tmp/gsolve.o : src/gsolve.cpp ../mat/include/mat.h
	$(CXX) -c -o $@ $(CXXFLAGS) $<

bin/gsolve : tmp/gsolve.o 
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS)

bin/gik : tmp/gik.o tmp/gsolve.o ../ik/lib/libik.a ../prepreprocess/lib/libprepreprocess.a ../preprocess/lib/libpreprocess.a ../mat/lib/libmat.a ../symbolic/lib/libsymbolic.a
	$(CXX) -o $@ $(CXXFLAGS) tmp/gik.o tmp/gsolve.o $(LDFLAGS)

ik_%.ini : bin/gik
	bin/gik $*

src/ik_%.cpp : ik_%.ini $(PREPROCESS)
	$(PREPROCESS) ik_$*.ini file=$@ main=false c++

include/ik_%.hpp : ik_%.ini $(PREPROCESS)
	$(PREPROCESS) ik_$*.ini file=$@ main=false c++

tmp/ik_%.o : src/ik_%.cpp include/ik_%.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/test_ik_%.cpp : ik_%.ini $(PREPROCESS)
	$(PREPROCESS) ik_$*.ini file=$@ main=true c++

tmp/test_ik_%.o : src/test_ik_%.cpp include/ik_%.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

bin/test_ik_% : tmp/test_ik_%.o
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

include ../build/make.end
