include config.mk
all :
	$(MAKE) -C libs all
	$(MAKE) -C drivers all

clean :
	$(MAKE) -C libs clean
	$(MAKE) -C drivers clean
