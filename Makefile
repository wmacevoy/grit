% : 
	$(MAKE) -C libs "$@"
	$(MAKE) -C drivers "$@"
	$(MAKE) -C centaur "$@"
	$(MAKE) -C control "$@"
