DEPS=utilities
include ../build/make.top

all : lib/libutilities.a include/utilities.hpp bin/test

tmp/md5.o : src/md5.cpp include/md5.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/toupper.o : src/toupper.cpp include/toupper.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/tolower.o : src/tolower.cpp include/tolower.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/beginswith.o : src/beginswith.cpp include/beginswith.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/endswith.o : src/endswith.cpp include/endswith.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/encode_string.o : src/encode_string.cpp include/encode_string.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/stopwatch.o : src/stopwatch.cpp include/stopwatch.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/ceildiv.o : src/ceildiv.cpp include/ceildiv.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

lib/libutilities.a : tmp/md5.o tmp/toupper.o tmp/tolower.o tmp/beginswith.o tmp/endswith.o tmp/encode_string.o tmp/stopwatch.o tmp/ceildiv.o
	ar cr lib/libutilities.a tmp/md5.o tmp/toupper.o tmp/tolower.o tmp/beginswith.o tmp/endswith.o tmp/encode_string.o tmp/stopwatch.o tmp/ceildiv.o

include/utilities.hpp : include/md5.hpp include/toupper.hpp include/tolower.hpp include/beginswith.hpp include/endswith.hpp include/encode_string.hpp include/stopwatch.hpp include/ceildiv.hpp
	touch $@

tmp/test.o : src/test.cpp include/utilities.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/test : tmp/test.o lib/libutilities.a
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

test : bin/test
	./bin/test

include ../build/make.end
