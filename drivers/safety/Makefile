all : libs progs py

DEPS=utilities zmqpp
MY_FLAGS=`python2.7-config --cflags`
MY_LIBS=`python2.7-config --ldflags`


include ../rules.mk

include/swig_$(TARGET).h :
	swig -c++  -python -external-runtime include/swig_$(TARGET).h

src/$(TARGET).cpp src/$(TARGET).py : include/$(TARGET).i
	swig -c++ -python -o src/$(TARGET).cpp include/$(TARGET).i

lib/_$(TARGET).so : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -shared  -o $@ $^ $(LDFLAGS)

lib/$(TARGET).py : src/$(TARGET).py
	cp $< $@

py : lib/_$(TARGET).so lib/$(TARGET).py include/swig_$(TARGET).h

pyclean :
	/bin/rm -rf src/$(TARGET).cpp src/$(TARGET).py include/swig_$(TARGET).h
