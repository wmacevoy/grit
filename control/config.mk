#CONTROL_FLAGS=-Iinclude -I../../libs/include -I../../setup -I../../libs/libjson -I../../libs/LeapSDK/include `../../libs/bin/c_urg-config --cflags`
#CONTROL_LIBS=-L../../libs/lib -lartemis -lbson -lbz2 -ldxl -lfreenect -lfreenect_sync -lcppunit2 -lzmq -lSDL -lpthread -lboost_thread-mt -lboost_date_time-mt -ljson -Wl,--no-as-needed -lLeap -ldl -lrt `../../libs/bin/c_urg-config --libs` -lm

CONTROL_FLAGS=-Iinclude -I../../libs/include -I../../libs/LeapSDK/include -I../../setup `../../libs/bin/c_urg-config --cflags`
CONTROL_LIBS=-L../../libs/lib -L../../libs/LeapSDK/lib/x64 -ldxl -lzmq -lSDL -lpthread -lboost_thread-mt -lboost_date_time-mt -Wl,--no-as-needed -lLeap -ldl -lrt `../../libs/bin/c_urg-config --libs` -lm

CFLAGS += $(CONTROL_FLAGS)
CXXFLAGS += $(CONTROL_FLAGS)
LDFLAGS := $(CONTROL_LIBS) $(LDFLAGS)
