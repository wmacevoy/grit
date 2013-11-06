all : libs progs iks

CPPP_PREFIX="out"
CLEAN_ALSO=../ik_leftarm/ik_leftarm.ini ../ik_rightarm/ik_rightarm.ini ../ik_leg/ik_leg.ini
DEPS=utilities prepreprocess symbolic preprocess mat

iks : ../ik/src/ik_leftarm.cpp ../ik/src/ik_rightarm.cpp ../ik/src/ik_leg.cpp ../ik/include/ik_leftarm.h ../ik/include/ik_rightarm.h ../ik/include/ik_leg.h

tmp/ik_%.ini : bin/gik
	../context bin/gik $*

../ik/src/ik_%.cpp : tmp/ik_%.ini ../preprocess/bin/preprocess
	../context ../preprocess/bin/preprocess $< file=$@ c++

../ik/include/ik_%.h : tmp/ik_%.ini ../preprocess/bin/preprocess
	../context ../preprocess/bin/preprocess $< file=$@ c++

include ../rules.mk
