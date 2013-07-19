include ../../setup/config.mk
include ../config.mk

MY_FLAGS=-I../libservos/include -I../libzmqpp/include -I../libnow/include
MY_LIBS=-L../libservos/lib -lservos -L../libzmqpp/lib -lzmqpp -L../libnow/lib -lnow -lzmq

CFLAGS += $(MY_FLAGS)
CXXFLAGS += $(MY_FLAGS)
LDFLAGS += $(MY_LIBS)

# Add .d to Make's recognized suffixes.
SUFFIXES += .d

TARGET=$(shell basename `realpath .`)
all : bin/$(TARGET)

#We don't need to clean up when we're making these targets
NODEPS:=clean tags svn

#Find all the C++ files in the src/ directory (but not Test.. or Main...)
SOURCES:=$(shell find src/ -regex ".*/[a-zA-Z0-9][^/]*\.\(c\|cpp\)" -a -! -regex ".*/test_[^/]*\.\(c\|cpp\)" -a -! -regex ".*/main_[^/]*\.\(c\|cpp\)" )

OBJECTS=$(patsubst src/%,tmp/%.o,$(SOURCES))

#These are the dependency files, which make will clean up after it creates them
DEPFILES:=$(patsubst src/%,tmp/%.d,$(SOURCES))

#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
    #Chances are, these files don't exist.  GMake will create them and
    #clean up automatically afterwards
    -include $(DEPFILES)
endif

#This is the rule for creating the dependency files
tmp/%.cpp.d: src/%.cpp
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst src/%.cpp,tmp/%.o,$<)' $< -MF $@

#This is the rule for creating the dependency files
tmp/%.c.d: src/%.c
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -MM -MT '$(patsubst src/%.c,tmp/%.o,$<)' $< -MF $@

#This rule does the compilation
tmp/%.cpp.o: src/%.cpp tmp/%.cpp.d
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) -o $@ -c $<

tmp/%.c.o: src/%.c tmp/%.c.d
	$(CC) $(CFLAGS) -o $@ -c $<

lib/$(TARGET).a : $(OBJECTS)
	ar crv $@ $^

lib/$(TARGET).so : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -shared  -o $@ $^ $(LDFLAGS)

clean : 
	/bin/rm -rf tmp/* bin/* lib/*

run : all
	./run bin/walker

bin/test_% : tmp/test_%.cpp.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

bin/test_% : tmp/test_%.c.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/% : tmp/main_%.cpp.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

bin/% : tmp/main_%.c.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

