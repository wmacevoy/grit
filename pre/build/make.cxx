CXX=g++
# 
# debugging options
#
CXXFLAGS=-g -fPIC

# 
# optimized options
#
# CXXFLAGS=-O3 -fPIC

CXXFLAGS += -Iinclude $(foreach DEP,$(DEPS), -I../$(DEP)/include)
LDFLAGS  += -Llib $(foreach DEP,$(DEPS), -L../$(DEP)/lib -l$(DEP))
