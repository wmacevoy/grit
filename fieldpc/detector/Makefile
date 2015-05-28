DEPS=
DRIVER_DEPS=utilities centaur zmqservos zmqpp threed ik

MY_FLAGS=`pkg-config --cflags opencv`
MY_LIBS=`pkg-config --libs opencv` -lboost_system

all : libs progs

include ../rules.mk
