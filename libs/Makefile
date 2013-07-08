DIRS=$(shell find . -mindepth 2 -maxdepth 2 -name Makefile -printf '%h\n')
all :
	for d in $(DIRS); do \
		$(MAKE) -C $$d all ; \
		tar -C $$d --exclude='.*' -cf - lib include | tar xf - ; \
	done
clean :
	for d in $(DIRS); do $(MAKE) -C $$d clean ; done
	/bin/rm -rf lib include
	find . -regex '.*[~#]' -print0 | xargs -0 /bin/rm -rf
