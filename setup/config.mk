#
# This is the root-level configuration for make file
#

TRUNK_FLAGS=-O2 -g -Wall -fPIC
TRUNK_LIBS=
CFLAGS += $(TRUNK_FLAGS)
CXXFLAGS +=-std=c++0x $(TRUNK_FLAGS)
LDFLAGS += $(TRUNK_LIBS)
