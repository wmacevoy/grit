CXXFLAGS=-g
LDFLAGS=
all : bin/i2ctest
bin/i2ctest : tmp/i2ctest.o
	$(CXX) $(CXXFLAGS) -o $@ $^
tmp/i2ctest.o : src/i2ctest.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $^
clean :
	/bin/rm -rf tmp/*

