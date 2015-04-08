DAEMON_FLAGS=-Iinclude -I../../libs/include -I../../setup
DAEMON_LIBS=-L../../libs/lib -lzmq -Wl,--no-as-needed -ldl -lrt -lm

CFLAGS += $(DAEMON_FLAGS)
CXXFLAGS += $(DAEMON_FLAGS)
LDFLAGS := $(DAEMON_LIBS) $(LDFLAGS)
