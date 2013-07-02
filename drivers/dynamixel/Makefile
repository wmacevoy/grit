CXXFLAGS=-std=c++0x -g -O2 -Wall -Iinclude -fPIC -I../../libs/include
# LDFLAGS=-L../../libs/lib -ldxl -lzmq -lpthread -lboost_thread-mt -Wl,--no-as-needed -lrt
LDFLAGS=-L../../libs/lib -ldxl -lzmq -lpthread -Wl,--no-as-needed -lrt
# Add .d to Make's recognized suffixes.
SUFFIXES += .d

TARGET=bin/ZMQServoServer bin/TestZMQServoController bin/RevolutionTest bin/basic

all : $(TARGET)

#We don't need to clean up when we're making these targets
NODEPS:=clean tags svn
#Find all the C++ files in the src/ directory
SOURCES:=$(shell find src/ -name "*.cpp")

OBJECTS:=$(patsubst src/%,tmp/%,$(patsubst %.cpp,%.o,$(SOURCES)))

#These are the dependency files, which make will clean up after it creates them
DEPFILES:=$(patsubst src/%,tmp/%,$(patsubst %.cpp,%.d,$(SOURCES)))

#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
    #Chances are, these files don't exist.  GMake will create them and
    #clean up automatically afterwards
    -include $(DEPFILES)
endif

#This is the rule for creating the dependency files
tmp/%.d: src/%.cpp
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst src/%.cpp,tmp/%.o,$<)' $< -MF $@

#This rule does the compilation
tmp/%.o: src/%.cpp tmp/%.d
	$(CXX) $(CXXFLAGS) -o $@ -c $<

bin/RevolutionTest : tmp/RevolutionTest.o tmp/CreateZMQServoController.o tmp/ServoController.o tmp/Servo.o tmp/now.o tmp/DynamixelDriver.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

bin/TestDrive1 : tmp/TestDrive1.o tmp/DynamixelDriver.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
bin/ZMQServoServer : tmp/ZMQServoServer.o tmp/ServoController.o tmp/CreateFakeServoController.o tmp/CreateDynamixelServoController.o tmp/FakeServoController.o tmp/FakeServo.o tmp/Servo.o tmp/now.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
bin/TestZMQServoController : tmp/TestZMQServoController.o tmp/CreateZMQServoController.o tmp/ServoController.o tmp/Servo.o tmp/now.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

bin/basic : tmp/basic.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean : 
	/bin/rm -rf tmp/* bin/*
