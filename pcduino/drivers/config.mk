DRIVER_FLAGS=-Iinclude -I../../libs/include -I../../setup
DRIVER_LIBS=-L../../libs/lib -lzmq -ldl -lpthread -Wl,--no-as-needed -lrt

CFLAGS += $(DRIVER_FLAGS)
CXXFLAGS += $(DRIVER_FLAGS)
LDFLAGS := $(DRIVER_LIBS) $(LDFLAGS)
