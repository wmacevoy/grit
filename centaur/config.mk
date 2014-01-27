# CENTAUR_FLAGS=-Iinclude -I../../libs/include -I../../setup -I../../libs/libjson `../../libs/hokuyo/urg-0.8.18/c_urg-config --cflags`
# CENTAUR_LIBS=-L../../libs/lib -L../../libs/libjson -lartemis -lbson -lbz2 -ldxl -lfreenect -lfreenect_sync -lcppunit2 -lzmq -lSDL -lpthread -lboost_thread -ljson -Wl,--no-as-needed -ldl -lrt `../../libs/hokuyo/urg-0.8.18/c_urg-config --libs` -lm

CENTAUR_FLAGS=-Iinclude -I../../libs/include -I../../setup `../../libs/hokuyo/urg-0.8.18/c_urg-config --cflags`
CENTAUR_LIBS=-L../../libs/lib -ldxl -lzmq -lSDL -lpthread -lboost_thread -lboost_filesystem -lboost_system -Wl,--no-as-needed -ldl -lrt `../../libs/hokuyo/urg-0.8.18/c_urg-config --libs` -lm


CFLAGS += $(CENTAUR_FLAGS)
CXXFLAGS += $(CENTAUR_FLAGS)
LDFLAGS := $(CENTAUR_LIBS) $(LDFLAGS)
