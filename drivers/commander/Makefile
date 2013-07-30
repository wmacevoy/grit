include ../../setup/config.mk
include ../config.mk

MY_FLAGS=-I../body/include -I../zmqpp/include -I../utilities/include
MY_LIBS=-L../zmqpp/lib -lzmqpp -L../utilities/lib -lutilities -lzmq

CFLAGS += $(MY_FLAGS)
CXXFLAGS += $(MY_FLAGS)
LDFLAGS += $(MY_LIBS)

TARGET=$(shell basename `realpath .`)
all : bin/$(TARGET)

include ../rules.mk
