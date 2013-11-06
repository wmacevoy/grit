DEPS=preprocess
include ../rules.mk
PREPROCESS=../preprocess/bin/preprocess
CLEAN_ALSO=src/$(TARGET).sci src/$(TARGET).cpp include/$(TARGET).hpp

all : source tests runs

source : src/$(TARGET).sci src/$(TARGET).cpp include/$(TARGET).hpp

tests : tmp/$(TARGET).sci.tests tmp/$(TARGET).cpp.tests

runs : tmp/$(TARGET).sci.runs tmp/$(TARGET).cpp.runs

src/$(TARGET).sci : $(TARGET).ini $(PREPROCESS)
	$(PREPROCESS) $(TARGET).ini file=$@ scilab

tmp/$(TARGET).sci.tests : src/$(TARGET).sci
	echo "exec('src/$(TARGET).sci'); $(TARGET)_tests('all'); exit" | scilab -nogui -nb -nwni | tee tmp/$(TARGET).sci.tests

tmp/$(TARGET).sci.runs : src/$(TARGET).sci
	echo "exec('src/$(TARGET).sci'); $(TARGET)_runs('all'); exit" | scilab -nogui -nb -nwni | tee tmp/$(TARGET).sci.runs

src/$(TARGET).cpp : $(TARGET).ini $(PREPROCESS)
	$(PREPROCESS) $(TARGET).ini file=$@ main=true c++

include/$(TARGET).hpp : $(TARGET).ini $(PREPROCESS)
	$(PREPROCESS) $(TARGET).ini file=$@ main=true c++

tmp/$(TARGET).o : src/$(TARGET).cpp include/$(TARGET).hpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

bin/$(TARGET) : tmp/$(TARGET).o
	$(CXX) -o $@ tmp/$(TARGET).o $(CXXFLAGS) $(LDFLAGS)

tmp/$(TARGET).cpp.tests : bin/$(TARGET)
	./bin/$(TARGET) -tests | tee tmp/$(TARGET).cpp.tests

tmp/$(TARGET).cpp.runs : bin/$(TARGET)
	./bin/$(TARGET) -runs | tee tmp/$(TARGET).cpp.runs


