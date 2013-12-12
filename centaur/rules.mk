include ../../setup/config.mk
include ../config.mk

TARGET=$(shell basename `realpath .`)

CFLAGS += $(MY_FLAGS)
CXXFLAGS += $(MY_FLAGS)
LDFLAGS := $(MY_LIBS) $(LDFLAGS)

CFLAGS += $(patsubst %,-I ../%/include,$(DEPS))
CXXFLAGS += $(patsubst %,-I ../%/include,$(DEPS))
LDFLAGS := $(patsubst %,-L ../%/lib,$(DEPS)) $(patsubst %,-l%,$(DEPS)) $(LDFLAGS)

CFLAGS += $(patsubst %,-I ../../drivers/%/include,$(DRIVER_DEPS))
CXXFLAGS += $(patsubst %,-I ../../drivers/%/include,$(DRIVER_DEPS))
LDFLAGS := $(patsubst %,-L ../../drivers/%/lib,$(DRIVER_DEPS)) $(patsubst %,-l%,$(DRIVER_DEPS)) $(LDFLAGS)

# Add .d to Make's recognized suffixes.
SUFFIXES += .d

#We don't need to clean up when we're making these targets
NODEPS:=clean tags svn

#Find all the C++ files in the src/ directory (but not test_.. or main_...)
SOURCES:=$(shell find src/ -regex ".*/[a-zA-Z0-9][^/]*\.\(c\|cpp\)" -a -! -regex ".*/test_[^/]*\.\(c\|cpp\)" -a -! -regex ".*/main_[^/]*\.\(c\|cpp\)" -a -! -name '$(TARGET).cpp' )
MAIN_SOURCES:=$(shell find src/ -regex ".*/main_[^/]*\.\(c\|cpp\)")
TEST_SOURCES:=$(shell find src/ -regex ".*/test_[^/]*\.\(c\|cpp\)")
ALL_SOURCES=$(SOURCES) $(MAIN_SOURCES) $(TEST_SOURCES)

OBJECTS=$(patsubst src/%,tmp/%.o,$(SOURCES))
MAIN_OBJECTS=$(patsubst src/%,tmp/%.o,$(MAIN_SOURCES))
TEST_OBJECTS=$(patsubst src/%,tmp/%.o,$(TEST_SOURCES))
ALL_OBJECTS=$(OBJECTS) $(MAIN_OBJECTS) $(TEST_OBJECTS)

PROGS=$(patsubst src/main_%.cpp,bin/%,$(MAIN_SOURCES))
TESTS=$(patsubst src/test_%.cpp,bin/test_%,$(TEST_SOURCES))

#These are the dependency files, which make will clean up after it creates them
DEPFILES:=$(patsubst src/%,tmp/%.d,$(ALL_SOURCES))

#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
    #Chances are, these files don't exist.  GMake will create them and
    #clean up automatically afterwards
    -include $(DEPFILES)
endif

#This is the rule for creating the cpp dependency files
tmp/%.cpp.d: src/%.cpp
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst src/%.cpp,tmp/%.cpp.o,$<)' $< -MF $@

#This is the rule for creating the c dependency files
tmp/%.c.d: src/%.c
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -MM -MT '$(patsubst src/%.c,tmp/%.c.o,$<)' $< -MF $@

#This rule does the compilation to object files
tmp/%.cpp.o: src/%.cpp tmp/%.cpp.d
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) -o $@ -c $<

tmp/%.c.o: src/%.c tmp/%.c.d
	$(CC) $(CFLAGS) -o $@ -c $<

lib/lib$(TARGET).a : $(OBJECTS)
	ar crv $@ $^

lib/lib$(TARGET).so : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -shared  -o $@ $^ $(LDFLAGS)

bin/test_% : tmp/test_%.cpp.o lib/lib$(TARGET).so lib/lib$(TARGET).a
	$(CXX) $(CXXFLAGS) -o $@ $< -Llib -l$(TARGET) $(LDFLAGS)

bin/test_% : tmp/test_%.c.o lib/lib$(TARGET).so lib/lib$(TARGET).a
	$(CC) $(CFLAGS) -o $@ $< -Llib -l$(TARGET) $(LDFLAGS)

bin/% : tmp/main_%.cpp.o lib/lib$(TARGET).so lib/lib$(TARGET).a
	$(CXX) $(CXXFLAGS) -o $@ $< -Llib -l$(TARGET) $(LDFLAGS)

bin/% : tmp/main_%.c.o lib/lib$(TARGET).so lib/lib$(TARGET).a
	$(CC) $(CFLAGS) -o $@ $< -Llib -l$(TARGET) $(LDFLAGS)

.PHONY: all
.PHONY: deps
.PHONY: depsall
.PHONY: clean
.PHONY: depsclean
.PHONY: libs
.PHONY: progs
.PHONY: tests
.PHONY: run

all : libs progs

depsall : deps all

clean : 
	/bin/rm -rf tmp/* bin/* lib/*

depsclean : clean
	for d in $(DEPS); do $(MAKE) -C ../$$d clean; done
	for d in $(DRIVER_DEPS); do $(MAKE) -C ../../drivers/$$d clean; done

libs : lib/lib$(TARGET).a lib/lib$(TARGET).so 

progs : $(PROGS)

tests : $(TESTS)
	for t in $(TESTS); do ../context $$t; done

deps : 
	for d in $(DRIVER_DEPS); do $(MAKE) -C ../../drivers/$$d libs; done
	for d in $(DEPS); do $(MAKE) -C ../$$d libs; done

run : bin/$(TARGET)
	../context bin/$(TARGET)

include/swig_$(TARGET).h :
	swig -c++ -I../ex/include -python -external-runtime include/swig_$(TARGET).h

src/$(TARGET).cpp : include/$(TARGET).i
	swig -c++ -I../ex/include -python -o src/$(TARGET).cpp include/$(TARGET).i

src/$(TARGET).py : include/$(TARGET).i
	swig -c++ -I../ex/include -python -o src/$(TARGET).cpp include/$(TARGET).i

lib/_$(TARGET).so : tmp/$(TARGET).cpp.o lib/lib$(TARGET).so
	$(CXX) $(CXXFLAGS) -L../$(TARGET)/lib -shared  -o $@ $< -l$(TARGET) $(LDFLAGS)

lib/$(TARGET).py : src/$(TARGET).py
	cp $< $@

py : lib/_$(TARGET).so lib/$(TARGET).py include/swig_$(TARGET).h

pyclean :
	/bin/rm -rf src/$(TARGET).cpp src/$(TARGET).py include/swig_$(TARGET).h
