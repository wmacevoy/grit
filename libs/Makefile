DIRS=$(shell find . -mindepth 2 -maxdepth 2 -regex '.*[^-]/Makefile' -printf '%h\n')

all :
	ok=1; \
	for d in $(DIRS); do \
	  if $(MAKE) -C $$d all ; then \
	    tar -C $$d --exclude='.*' -cf - lib include | tar xf - ; \
	  else \
	    echo "error making $$d"; \
	    ok=0; \
	  fi; \
	done; \
	if [ "$$ok" = "0" ] ; then exit 1 ; fi

clean :
	for d in $(DIRS); do $(MAKE) -C $$d clean; done
	/bin/rm -rf lib include
	find . -regex '.*[~#]' -print0 | xargs -0 /bin/rm -rf
