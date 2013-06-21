CFLAGS=-Iinclude
LDFLAGS=-lzmq -lfreenect -lglut -lGLU -lGL -lXmu -lX11

all : bin/pubKinect bin/subKinect

tmp/pubKinect.o : src/pubKinect.c include/libfreenect.h
	$(CC) -c $(CFLAGS) -o $@ $<

tmp/subKinect.o : src/subKinect.c
	$(CC) -c $(CFLAGS) -o $@ $<

bin/pubKinect : tmp/pubKinect.o
	$(CC) $(CFLAGS) -o $@ tmp/pubKinect.o $(LDFLAGS)

bin/subKinect : tmp/subKinect.o
	$(CC) $(CFLAGS) -o $@ tmp/subKinect.o $(LDFLAGS)

clean :
	/bin/rm -rf tmp/* bin/*

run : bin/pubKinect bin/subKinect
	gnome-terminal -x /bin/bash -c ./bin/pubKinect &
	bin/subKinect &


