CFLAGS=-Iinclude
LDFLAGSS=-lzmq -lglut -lGLU -lGL -lXmu -lX11
LDFLAGSP=-lzmq -lfreenect

all : bin/pubKinect bin/subKinect

tmp/pubKinect.o : src/pubKinect.c include/libfreenect.h
	$(CC) -c $(CFLAGS) -o $@ $<

tmp/subKinect.o : src/subKinect.c
	$(CC) -c $(CFLAGS) -o $@ $<

bin/pubKinect : tmp/pubKinect.o
	$(CC) $(CFLAGS) -o $@ tmp/pubKinect.o $(LDFLAGSP)

bin/subKinect : tmp/subKinect.o
	$(CC) $(CFLAGS) -o $@ tmp/subKinect.o $(LDFLAGSS)

pub: bin/pubKinect

tmp/pubKinect.o : src/pubKinect.c include/libfreenect.h
	$(CC) -c $(CFLAGS) -o $@ $<

bin/pubKinect : tmp/pubKinect.o
	$(CC) $(CFLAGS) -o $@ tmp/pubKinect.o $(LDFLAGSP)

sub: bin/subKinect

tmp/subKinect.o : src/subKinect.c
	$(CC) -c $(CFLAGS) -o $@ $<

bin/subKinect : tmp/subKinect.o
	$(CC) $(CFLAGS) -o $@ tmp/subKinect.o $(LDFLAGSS)

clean :
	/bin/rm -rf tmp/* bin/*

run : bin/pubKinect bin/subKinect
	gnome-terminal -x /bin/bash -c ./bin/pubKinect &
	bin/subKinect &


