include ../../setup/config.mk
include ../config.mk

MY_FLAGS=-I../servos/include -I../zmqpp/include -I../utilities/include
MY_LIBS=-L../servos/lib -lservos -L../zmqpp/lib -lzmqpp -L../utilities/lib -lutilities -lzmq

CFLAGS += $(MY_FLAGS)
CXXFLAGS += $(MY_FLAGS)
LDFLAGS += $(MY_LIBS)

TARGET=$(shell basename `realpath .`)
all : bin/$(TARGET)

include ../rules.mk
