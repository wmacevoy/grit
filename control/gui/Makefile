all : bin/gui.py

clean :
	/bin/rm -rf tmp/* bin/* lib/*

bin/%.py : src/%.py
	cp $< $@
	chmod +x $@

