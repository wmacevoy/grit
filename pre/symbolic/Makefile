all : progs libs

MY_OBJECTS=tmp/symbolic_parse.cpp.o tmp/symbolic_parse.tab.cpp.o

DEPS=utilities

tmp/symbolic_parse.cpp.d : src/symbolic_parse.cpp src/symbolic_parse.tab.hpp
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst src/%.cpp,tmp/%.cpp.o,$<)' $< -MF $@

src/symbolic_parse.tab.cpp src/symbolic_parse.tab.hpp : src/symbolic_parse.y
	bison --defines=src/symbolic_parse.tab.hpp -o $@ $<

src/symbolic_parse.cpp : src/symbolic_parse.l
	flex -o $@ $<

include ../rules.mk
