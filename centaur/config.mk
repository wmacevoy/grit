CENTAUR_FLAGS=-Iinclude -I../../libs/include -I../../setup -I../../libs/libjson -I../../libs/hokuyo/include/c
CENTAUR_LIBS=-L../../libs/lib -L../../libs/libjson -lartemis -lbson -lbz2 -ldxl -lfreenect -lfreenect_sync -lcppunit2 -lzmq -lSDL -lpthread -lboost_thread -ljson -Wl,--no-as-needed -ldl -lrt

CFLAGS += $(CENTAUR_FLAGS)
CXXFLAGS += $(CENTAUR_FLAGS)
LDFLAGS := $(CENTAUR_LIBS) $(LDFLAGS)
