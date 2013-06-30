CFLAGS=-O2 -Wall -fPIC -std=c++0x
CINCLUDES= -I../../libs/hokuyo/include -I../../libs/zmq/include -I../libcentaur/include -Iinclude  
CLIBS=-lSDL -lboost_thread -lhokuyo -lzmq -lcentaur -L../../libs/hokuyo/lib -L../../libs/zmq/lib -L../libcentaur/lib
CC=g++ 

TARGET=bin/HokuyoProvider

.PHONY: all
all : $(TARGET)

$(TARGET) : 
	$(CC) $(CFLAGS) $(CINCLUDES) src/main.cpp src/HokuyoProvider.cpp  $(CLIBS) -o bin/HokuyoProvider
	
.PHONY: clean
clean : 
	/bin/rm -rf tmp/* lib/* bin/*

