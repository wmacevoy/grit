all : libs progs swig

DEPS=
MY_FLAGS=-I/usr/include/python2.7/
MY_LIBS=

include ../rules.mk

src/SwigMyClass.cpp : include/MyClass.i include/MyClass.h
	swig -c++ -python  -o src/SwigMyClass.cpp include/MyClass.i

lib/_$(TARGET).so : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -shared  -o $@ $^ $(LDFLAGS)

py : lib/_$(TARGET).so lib/

