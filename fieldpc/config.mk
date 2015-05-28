FIELDPC_FLAGS=-Iinclude -I../../libs/include -I../../setup 
FIELDPC_LIBS=-L../../libs/lib -ldxl -lzmq -lSDL -lSDL_net -lpthread -lboost_thread-mt -lboost_date_time-mt -Wl,--no-as-needed -ldl -lrt -lm

CFLAGS += $(FIELDPC_FLAGS)
CXXFLAGS += $(FIELDPC_FLAGS)
LDFLAGS := $(FIELDPC_LIBS) $(LDFLAGS)
