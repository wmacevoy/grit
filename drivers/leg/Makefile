CXXFLAGS=-O2 -std=gnu++0x -pthread

all : bin/leg

clean :
	/bin/rm -rf tmp/* bin/leg

tmp/leg_main.o : src/main_leg.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
bin/leg : tmp/main_leg.o
	$(CXX) $(CXXFLAGS) -o $@ $^
