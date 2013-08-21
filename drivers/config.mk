DRIVER_FLAGS=-Iinclude -I../../libs/include -I../../setup
DRIVER_LIBS=-L../../libs/lib -lartemis -lbson -lbz2 -lcppunit2 -ldxl -lfreenect -lfreenect_sync -lhokuyo -lzmq -lSDL -ldl -lpthread -lboost_thread -Wl,--no-as-needed -lrt

CFLAGS += $(DRIVER_FLAGS)
CXXFLAGS += $(DRIVER_FLAGS)
LDFLAGS := $(DRIVER_LIBS) $(LDFLAGS)
