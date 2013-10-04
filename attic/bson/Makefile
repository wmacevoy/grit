CFLAGS=-O2 -Wall -Isrc -fPIC
CXXFLAGS=$(CFLAGS)
# Add .d to Make's recognized suffixes.
SUFFIXES += .d

TARGET=lib/libbson

.PHONY: all
all : includes  $(TARGET).a $(TARGET).so

#We don't need to clean up when we're making these targets
NODEPS:=clean tags svn
#Find all the C files in the src/ directory
CPP_SOURCES:=$(shell find src -name "*.cpp")
C_SOURCES:=$(shell find src -name "*.c")
SOURCES=$(CPP_SOURCES) $(C_SOURCES)

CPP_OBJECTS:=$(patsubst src/%,tmp/%,$(patsubst %.cpp,%.o,$(CPP_SOURCES)))
C_OBJECTS:=$(patsubst src/%,tmp/%,$(patsubst %.c,%.o,$(C_SOURCES)))
OBJECTS=$(CPP_OBJECTS) $(C_OBJECTS)

c_objects:
	echo $(C_OBJECTS)

SRC_HEADERS:=$(shell find src -name "*.h" -o -name "*.hpp")
INCLUDE_HEADERS=$(patsubst src/%,include/%,$(SRC_HEADERS))

#These are the dependency files, which make will clean up after it creates them
CPP_DEPFILES:=$(patsubst src/%,tmp/%,$(patsubst %.cpp,%.d,$(CPP_SOURCES)))
C_DEPFILES:=$(patsubst src/%,tmp/%,$(patsubst %.c,%.d,$(C_SOURCES)))
DEPFILES=$(CPP_DEPFILES) $(C_DEPFILES)
#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
    #Chances are, these files don't exist.  GMake will create them and
    #clean up automatically afterwards
    -include $(DEPFILES)
endif


includes : $(INCLUDE_HEADERS)

include/%.h : src/%.h
	mkdir -p `dirname '$@'`
	/bin/cp -rf $< $@

include/%.hpp : src/%.hpp
	mkdir -p `dirname '$@'`
	/bin/cp -rf $< $@

#This is the rule for creating the dependency files
tmp/%.d: src/%.cpp
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) -MM -MT  '$(patsubst src/%.cpp,tmp/%.o,$<)' $< -MF $@

#This is the rule for creating the dependency files
tmp/%.d: src/%.c
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -MM -MT  '$(patsubst src/%.cpp,tmp/%.o,$<)' $< -MF $@

#This rule does the compilation
tmp/%.o: src/%.cpp tmp/%.d
	$(CXX) $(CXXFLAGS) -o $@ -c $<

#This rule does the compilation
tmp/%.o: src/%.c tmp/%.d
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET).a : $(OBJECTS)
	ar crv $@ $^

$(TARGET).so : $(OBJECTS)
	$(CC) -shared -o $@ $^

.PHONY: clean
clean : 
	/bin/rm -rf tmp/* lib/* bin/* include/*
