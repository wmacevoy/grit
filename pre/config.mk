PRE_FLAGS=-Iinclude -I../../libs/include -I../../setup
PRE_LIBS=-L../../libs/lib -ldl -lpthread -lboost_thread-mt -lboost_date_time-mt -Wl,--no-as-needed -lrt

CFLAGS += $(PRE_FLAGS)
CXXFLAGS += $(PRE_FLAGS)
LDFLAGS := $(PRE_LIBS) $(LDFLAGS)
