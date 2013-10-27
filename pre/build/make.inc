HOSTNAME=$(shell hostname -s)
PREPROCESS=../preprocess/bin/preprocess
PREPREPROCESS=../prepreprocess/bin/prepreprocess
BISON=bison
FLEX=flex
THIS=$(shell pwd | sed -e 's|.*/||')

include ../build/make.cxx
ifeq ($(findstring cuda,$(THIS)),cuda)
  include ../build/make.cuda
endif

dependencies :
	true $(foreach DEP,$(DEPS),&& ( if [ "$(DEP)" != "$(THIS)" ] ; then $(MAKE) -C ../$(DEP) all; fi ) )

clean :
	/bin/rm -rf tmp/* bin/* lib/* *.linkinfo $(CLEAN)
	find . -name '*~' -o -name '#*' -exec /bin/rm -rf {} \;

cleanall :
	$(MAKE) clean
	true $(foreach DEP,$(DEPS),&& ( if [ "$(DEP)" != "$(THIS)" ] ; then $(MAKE) -C ../$(DEP) cleanall; fi ) )

