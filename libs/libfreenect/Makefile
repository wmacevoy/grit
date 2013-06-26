all :
	mkdir -p build bin lib include
	/bin/bash -c 'cd build ; cmake ..'
	$(MAKE) -C build
	cp -r build/lib .
	cp -r build/bin .

install : all
	$(MAKE) -C build install

clean : 
	/bin/rm -rf build lib bin
