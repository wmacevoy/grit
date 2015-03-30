CXXFLAGS=-O2 -std=gnu++0x -pthread -Iinclude

all : bin/leg

clean :
	/bin/rm -rf tmp/* bin/leg

tmp/GPIO.o : src/GPIO.cpp include/GPIO.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/test_gpio.o : src/test_gpio.cpp include/GPIO.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/test_gpio : tmp/test_gpio.o tmp/GPIO.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

tmp/AnalogIn.o : src/AnalogIn.cpp include/AnalogIn.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/test_analogin.o : src/test_analogin.cpp include/AnalogIn.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/test_analogin : tmp/test_analogin.o tmp/AnalogIn.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

tmp/Tone.o : src/Tone.cpp include/Tone.h include/GPIO.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tmp/test_tone.o : src/test_tone.cpp include/GPIO.h include/Tone.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/test_tone : tmp/test_tone.o tmp/GPIO.o tmp/Tone.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
tmp/main_leg.o : src/main_leg.cpp include/GPIO.h include/AnalogIn.h include/Tone.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<
bin/leg : tmp/main_leg.o tmp/GPIO.o tmp/AnalogIn.o tmp/Tone.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
