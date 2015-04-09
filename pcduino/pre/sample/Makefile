DEPS=preprocess
CLEAN=src/sample.sci src/sample.cpp include/sample.hpp
include ../build/make.top

all : dependencies source tests runs

source : src/sample.sci src/sample.cpp include/sample.hpp

tests : tmp/sample.sci.tests tmp/sample.cpp.tests

runs : tmp/sample.sci.runs tmp/sample.cpp.runs

src/sample.sci : sample.ini $(PREPROCESS)
	$(PREPROCESS) sample.ini file=$@ scilab

tmp/sample.sci.tests : src/sample.sci
	echo "exec('src/sample.sci'); sample_tests('all'); exit" | scilab -nogui -nb -nwni | tee tmp/sample.sci.tests

tmp/sample.sci.runs : src/sample.sci
	echo "exec('src/sample.sci'); sample_runs('all'); exit" | scilab -nogui -nb -nwni | tee tmp/sample.sci.runs

src/sample.cpp : sample.ini $(PREPROCESS)
	$(PREPROCESS) sample.ini file=$@ main=true c++

include/sample.hpp : sample.ini $(PREPROCESS)
	$(PREPROCESS) sample.ini file=$@ main=true c++

tmp/sample.o : src/sample.cpp include/sample.hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

bin/sample : tmp/sample.o
	$(CXX) -o $@ tmp/sample.o $(CXXFLAGS) $(LDFLAGS)

tmp/sample.cpp.tests : bin/sample
	./bin/sample -tests | tee tmp/sample.cpp.tests

tmp/sample.cpp.runs : bin/sample
	./bin/sample -runs | tee tmp/sample.cpp.runs

include ../build/make.end
