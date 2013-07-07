CFLAGS=-g -O2 -Wall -Iinclude -fPIC -I../../libs/include
CXXFLAGS=$(CFLAGS) -std=c++0x
LDFLAGS=

# Add .d to Make's recognized suffixes.
SUFFIXES += .d

TARGET=$(shell basename `realpath .`)
all : lib/$(TARGET).a lib/$(TARGET).so

#We don't need to clean up when we're making these targets
NODEPS:=clean tags svn

#Find all the C++ files in the src/ directory
SOURCES:=$(shell find src/ -regex "[a-zA-Z0-9].*\.\(c\|cpp\)")
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
	$(CC) $(CFLAGS) -MM -MT '$(patsubst src/%.c,tmp/%.o,$<)' $< -MF $@

#This rule does the compilation
tmp/%.cpp.o: src/%.cpp tmp/%.cpp.d
	$(CXX) $(CXXFLAGS) -o $@ -c $<

tmp/%.c.o: src/%.c tmp/%.c.d
	$(CC) $(CFLAGS) -o $@ -c $<

lib/$(TARGET).a : $(OBJECTS)
	ar crv $@ $^

lib/$(TARGET).so : $(OBJECTS)
	$(CXX) -shared  -o $@ $^

bin/$(TARGET) : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean : 
	/bin/rm -rf tmp/* bin/* lib/*
