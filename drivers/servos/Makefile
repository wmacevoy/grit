include ../../setup/config.mk
include ../config.mk

MY_FLAGS=-I../zmqpp/include -I../utilities/include
MY_LIBS=-L../zmqpp/lib -lzmqpp -L../utilities/lib -lutilities -lzmq

CFLAGS += $(MY_FLAGS)
CXXFLAGS += $(MY_FLAGS)
LDFLAGS += $(MY_LIBS)

TARGET=$(shell basename `realpath .`)
all : lib/lib$(TARGET).a lib/lib$(TARGET).so bin/$(TARGET) bin/servos bin/wiggle bin/test_servos
test : bin/servos bin/test_servos
	./tests

include ../rules.mk
