CENTAUR_FLAGS=-Iinclude -I../../libs/include -I../../setup
CENTAUR_LIBS=-L../../libs/lib -lartemis -lbson -lbz2 -lcppunit2 -ldxl -lfreenect -lfreenect_sync -lhokuyo -lzmq -lSDL -lpthread -lboost_thread -Wl,--no-as-needed -ldl -lrt

CFLAGS += $(CENTAUR_FLAGS)
CXXFLAGS += $(CENTAUR_FLAGS)
LDFLAGS := $(CENTAUR_LIBS) $(LDFLAGS)
