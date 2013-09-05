all : libs progs

DEPS=
DRIVER_DEPS=centaur

MY_FLAGS=-I../../libs/libjson -DNDEBUG
MY_LIBS=../../libs/libjson/libjson.a -lglut -lGLU -lGL -lXmu -lX11

include ../rules.mk

#run : bin/pubKinect bin/subKinect
#	gnome-terminal -x /bin/bash -c ./bin/pubKinect &
#	bin/subKinect &
