DRIVER_FLAGS=-Iinclude -I../../libs/include
DRIVER_LIBS=-L../../libs/lib -ldxl -lpthread -Wl,--no-as-needed -lrt

CFLAGS += $(DRIVER_FLAGS)
CXXFLAGS += $(DRIVER_FLAGS)
LDFLAGS += $(DRIVER_LIBS)
