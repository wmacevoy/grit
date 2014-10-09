DRIVER_FLAGS=-Iinclude -I../../libs/include -I../../setup
#DRIVER_LIBS=-L../../libs/lib -lartemis -lbson -lbz2 -ldxl -lfreenect -lfreenect_sync -lzmq -lSDL -ldl -lpthread -lboost_thread-mt -lboost_date_time-mt -Wl,--no-as-needed -lrt
DRIVER_LIBS=-L../../libs/lib -ldxl -lzmq -ldl -lpthread -lboost_thread-mt -lboost_date_time-mt -Wl,--no-as-needed -lrt

CFLAGS += $(DRIVER_FLAGS)
CXXFLAGS += $(DRIVER_FLAGS)
LDFLAGS := $(DRIVER_LIBS) $(LDFLAGS)
