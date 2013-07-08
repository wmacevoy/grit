CFLAGS= -g -O2 -Iinclude -I../../libs/include
CXXFLAGS=-std=c++0x $(CFLAGS) `pkg-config --cflags opencv`
LDFLAGSS=-L../../libs/lib -lzmq -lglut -lGLU -lGL -lXmu -lX11
LDFLAGSP=-L../../libs/lib -lzmq -lfreenect `pkg-config --libs opencv` 

all : bin/pubKinect bin/subKinect

tmp/pubKinect.o : src/pubKinect.cpp include/libfreenect.h
	$(CC) -c $(CFLAGS) -o $@ $<

tmp/subKinect.o : src/subKinect.c
	$(CC) -c $(CFLAGS) -o $@ $<

bin/pubKinect : tmp/pubKinect.o
	$(CXX) $(CFLAGS) -o $@ tmp/pubKinect.o $(LDFLAGSP)

bin/subKinect : tmp/subKinect.o
	$(CXX) $(CFLAGS) -o $@ tmp/subKinect.o $(LDFLAGSS)

pub: bin/pubKinect

tmp/pubKinect.o : src/pubKinect.cpp include/libfreenect.h
	$(CC) -c $(CFLAGS) -o $@ $<

bin/pubKinect : tmp/pubKinect.o
	$(CXX) $(CXXFLAGS) -o $@ tmp/pubKinect.o $(LDFLAGSP)

sub: bin/subKinect

tmp/subKinect.o : src/subKinect.c
	$(CC) -c $(CFLAGS) -o $@ $<

bin/subKinect : tmp/subKinect.o
	$(CXX) $(CFLAGS) -o $@ tmp/subKinect.o $(LDFLAGSS)

clean :
	/bin/rm -rf tmp/* bin/*

run : bin/pubKinect bin/subKinect
	gnome-terminal -x /bin/bash -c ./bin/pubKinect &
	bin/subKinect &


