include ../../setup/config.mk
include ../config.mk

MY_FLAGS=
MY_LIBS=

CFLAGS += $(MY_FLAGS)
CXXFLAGS += $(MY_FLAGS)
LDFLAGS += $(MY_LIBS)

TARGET=$(shell basename `realpath .`)
all : lib/lib$(TARGET).a lib/lib$(TARGET).so bin/test_now

include ../rules.mk
