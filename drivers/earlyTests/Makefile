include ../../config.mk
include ../config.mk

all : RevolutionTest Test1
RevolutionTest : RevolutionTest.cpp DynamixelDriver.cpp DynamixelDriver.hpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
Test1 : Test1.cpp DynamixelDriver.cpp DynamixelDriver.hpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
