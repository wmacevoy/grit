CFLAGS=-O2 -Wall -Iinclude -fPIC
CC=g++ 
# Add .d to Make's recognized suffixes.
SUFFIXES += .d

TARGET=lib/libhokuyo.a

.PHONY: all
all : $(TARGET)

#We don't need to clean up when we're making these targets
NODEPS:=clean tags svn
#Find all the C files in the src/ directory
SOURCES:=$(shell find src/ -name "*.cpp")
OBJECTS:=$(patsubst src/%,tmp/%,$(patsubst %.cpp,%.o,$(SOURCES)))

#These are the dependency files, which make will clean up after it creates them
DEPFILES:=$(patsubst src/%,tmp/%,$(patsubst %.cpp,%.d,$(SOURCES)))

#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
    #Chances are, these files don't exist.  GMake will create them and
    #clean up automatically afterwards
    -include $(DEPFILES)
endif

#This is the rule for creating the dependency files
tmp/%.d: src/%.cpp
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -MM -MT  '$(patsubst src/%.cpp,tmp/%.o,$<)' $< -MF $@

#This rule does the compilation
tmp/%.o: src/%.cpp tmp/%.d
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET) : $(OBJECTS)
	ar crv $@ $^

test: $(TARGET)
	$(CC) test/main.cpp -Iinclude -lhokuyo -Llib -lSDL -I../catch -o bin/hokuyoTest
	bin/hokuyoTest

.PHONY: clean
clean : 
	/bin/rm -rf tmp/* lib/* bin/*

