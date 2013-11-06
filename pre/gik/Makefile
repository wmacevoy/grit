all : libs progs iks

IK=../../drivers/ik
IK_MAKE=$(IK)/src/ik_leg.cpp $(IK)/src/ik_leftarm.cpp $(IK)/src/ik_rightarm.cpp $(IK)/include/ik_leg.h $(IK)/include/ik_leftarm.h $(IK)/include/ik_rightarm.h

CPPP_PREFIX="out"
CLEAN_ALSO=$(IK_MAKE)
DEPS=utilities prepreprocess symbolic preprocess mat

iks : $(IK_MAKE)

tmp/ik_%.ini $(IK)/src/fk_%.cpp $(IK)/include/fk_%.h : bin/gik
	../context bin/gik $*

$(IK)/src/ik_%.cpp : tmp/ik_%.ini ../preprocess/bin/preprocess
	../context ../preprocess/bin/preprocess $< file=$@ c++

$(IK)/include/ik_%.h : tmp/ik_%.ini ../preprocess/bin/preprocess
	../context ../preprocess/bin/preprocess $< file=$@ c++

include ../rules.mk
