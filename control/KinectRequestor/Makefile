CFLAGS= -g -O2 -Iinclude -I../../libs/include -I../libcentaur/include -I../HokuyoProvider/include
CXXFLAGS=-std=c++0x $(CFLAGS) #`pkg-config --cflags opencv`
LDFLAGSS=-L../../libs/bson/lib -L../libcentaur/lib -lzmq -lglut -lGLU -lGL -lXmu -lX11 -lbson -lcentaur -lpthread -lboost_thread -lrt #`pkg-config --libs opencv` 
LDFLAGSP=-L../../libs/lib -lzmq -lfreenect #`pkg-config --libs opencv` 

all : bin/pubKinect bin/subKinect

tmp/pubKinect.o : src/pubKinect.cpp include/libfreenect.h
	$(CC) -c $(CFLAGS) -o $@ $<

tmp/subKinect.o : src/subKinect.cpp 
	$(CC) -c $(CFLAGS) -o $@ $<

bin/pubKinect : tmp/pubKinect.o
	$(CXX) $(CFLAGS) -o $@ tmp/pubKinect.o $(LDFLAGSP)

bin/subKinect : tmp/subKinect.o
	$(CXX) $(CFLAGS) -o $@ tmp/subKinect.o $(LDFLAGSS)

pub : bin/pubKinect

sub : bin/subKinect

clean :
	/bin/rm -rf tmp/* bin/*

run : bin/pubKinect bin/subKinect
	gnome-terminal -x /bin/bash -c ./bin/pubKinect &
	bin/subKinect &


