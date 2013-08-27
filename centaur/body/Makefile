DEPS=servos
DRIVER_DEPS=centaur utilities zmqpp

MY_FLAGS=-I/usr/include/python2.7/
MY_LIBS=-lpython2.7

all : libs progs py

include ../rules.mk


include/$(TARGET).i : include/$(TARGET).i.py include/Body.h
	cd include; python $(TARGET).i.py > $(TARGET).i

src/$(TARGET).cpp src/$(TARGET).py : include/$(TARGET).i
	swig -python -c++ -o src/$(TARGET).cpp include/$(TARGET).i

lib/_$(TARGET).so : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -shared  -o $@ $^ $(LDFLAGS)

lib/$(TARGET).py : src/$(TARGET).py
	cp $< $@

py : lib/_$(TARGET).so lib/$(TARGET).py
