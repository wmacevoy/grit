#
# This is the root-level configuration for make file
#

# TRUNK_FLAGS=-O2 -g -Wall -fPIC
TRUNK_FLAGS=-g -Wall -fPIC `python2.7-config --cflags`
TRUNK_LIBS=`python2.7-config --ldflags`

CFLAGS += $(TRUNK_FLAGS)
CXXFLAGS += -std=c++0x $(TRUNK_FLAGS)
LDFLAGS := $(TRUNK_LIBS) $(LDFLAGS)
