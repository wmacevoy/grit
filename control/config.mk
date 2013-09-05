CONTROL_FLAGS=-Iinclude -I../../libs/include -I../../setup
CONTROL_LIBS=-L../../libs/lib -lartemis -lbson -lbz2 -ldxl -lfreenect -lfreenect_sync -lcppunit2 -lzmq -lSDL -lpthread -lboost_thread -Wl,--no-as-needed -ldl -lrt

CFLAGS += $(CONTROL_FLAGS)
CXXFLAGS += $(CONTROL_FLAGS)
LDFLAGS := $(CONTROL_LIBS) $(LDFLAGS)
