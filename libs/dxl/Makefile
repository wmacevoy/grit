CFLAGS=-O2 -Wall -Iinclude -fPIC
# Add .d to Make's recognized suffixes.
SUFFIXES += .d

TARGET=lib/libdxl.so

all : $(TARGET)

#We don't need to clean up when we're making these targets
NODEPS:=clean tags svn
#Find all the C files in the src/ directory
SOURCES:=$(shell find src/ -name "*.c")

OBJECTS:=$(patsubst src/%,tmp/%,$(patsubst %.c,%.o,$(SOURCES)))

#These are the dependency files, which make will clean up after it creates them
DEPFILES:=$(patsubst src/%,tmp/%,$(patsubst %.c,%.d,$(SOURCES)))

#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
    #Chances are, these files don't exist.  GMake will create them and
    #clean up automatically afterwards
    -include $(DEPFILES)
endif

#This is the rule for creating the dependency files
tmp/%.d: src/%.c
	mkdir -p `dirname '$@'`
	$(CC) $(CFLAGS) -MM -MT '$(patsubst src/%.c,tmp/%.o,$<)' $< -MF $@

#This rule does the compilation
tmp/%.o: src/%.c tmp/%.d
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET) : $(OBJECTS)
	mkdir -p `dirname '$@'`
	$(CC) -shared -o $@ $(OBJECTS)

clean : 
	/bin/rm -rf tmp/* lib/*
