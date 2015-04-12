PRE_FLAGS=-Iinclude -I../../libs/include -I../../setup
PRE_LIBS=-L../../libs/lib -ldl -Wl,--no-as-needed -lrt

CFLAGS += $(PRE_FLAGS)
CXXFLAGS += $(PRE_FLAGS)
LDFLAGS := $(PRE_LIBS) $(LDFLAGS)
