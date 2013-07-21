include ../../setup/config.mk
include ../config.mk

MY_FLAGS=-I../utilities/include
MY_LIBS=-L../utilities/lib -lutilities

CFLAGS += $(MY_FLAGS)
CXXFLAGS += $(MY_FLAGS)
LDFLAGS += $(MY_LIBS)

TARGET=$(shell basename `realpath .`)
all : lib/lib$(TARGET).a lib/lib$(TARGET).so

include ../rules.mk
