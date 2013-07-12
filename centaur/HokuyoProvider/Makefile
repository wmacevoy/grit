CFLAGS=-O2 -fPIC -std=c++0x
CINCLUDES= -I../../libs/hokuyo/include -I../../libs/zmq/include -I../libcentaur/include -I../../libs/bson/include -Iinclude  
CLIBS= -lcentaur -lbson -lpthread -lboost_thread -lhokuyo -lzmq -lSDL -L../../libs/hokuyo/lib -L../../libs/zmq/lib -L../../libs/bson/lib -L../libcentaur/lib 
CC=g++ 

TARGET=bin/HokuyoProvider

.PHONY: all
all : $(TARGET)

$(TARGET) : 
	$(CC) $(CFLAGS) $(CINCLUDES) src/main.cpp src/HokuyoProvider.cpp $(CLIBS) -o bin/HokuyoProvider
	
.PHONY: clean
clean : 
	/bin/rm -rf tmp/* lib/* bin/*

