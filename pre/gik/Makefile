all : libs progs iks

IK=../../drivers/ik
IK_MAKE=$(IK)/ik_leftarm.ini $(IK)/ik_rightarm.ini $(IK)/src/ik_leftarm.cpp $(IK)/src/ik_rightarm.cpp $(IK)/include/ik_leftarm.h $(IK)/include/ik_rightarm.h $(IK)/src/fk_leftarm.cpp $(IK)/src/fk_rightarm.cpp $(IK)/include/fk_leftarm.h $(IK)/include/fk_rightarm.h $(IK)/ik_leftarm.sci $(IK)/ik_rightarm.sci $(IK)/src/ik_lidar.cpp $(IK)/include/ik_lidar.h $(IK)/ik_lidar.ini $(IK)/ik_lidar.sci $(IK)/include/fk_head.h $(IK)/src/fk_head.cpp $(IK)/include/fk_lidar.h $(IK)/src/fk_lidar.cpp

CPPP_PREFIX="out"
CLEAN_ALSO=$(IK_MAKE)
DEPS=utilities prepreprocess symbolic preprocess mat

iks : $(IK_MAKE)

$(IK)/ik_%.ini : bin/gik
	../context bin/gik ik_$*

$(IK)/src/fk_%.cpp $(IK)/include/fk_%.h : bin/gik
	../context bin/gik fk_$*

$(IK)/ik_%.sci : $(IK)/ik_%.ini ../preprocess/bin/preprocess
	../context ../preprocess/bin/preprocess $< file=$@ scilab

$(IK)/src/ik_%.cpp : $(IK)/ik_%.ini ../preprocess/bin/preprocess
	../context ../preprocess/bin/preprocess $< file=$@ c++


$(IK)/include/ik_%.h : $(IK)/ik_%.ini ../preprocess/bin/preprocess
	../context ../preprocess/bin/preprocess $< file=$@ c++

include ../rules.mk
