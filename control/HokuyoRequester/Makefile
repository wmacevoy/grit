CFLAGS=-g -O2 -fPIC -std=c++0x
CINCLUDES=-I../../libs/zmq/include -I../libcentaur/include -I../../libs/bson/src -I../HokuyoProvider/include
CLIBS= -lbson -lcentaur  -lpthread -lzmq -lboost_thread -lrt -L../../libs/zmq/lib -L../../libs/bson/lib -L../libcentaur/lib 
CC=g++ 

TARGET=bin/HokuyoRequester

.PHONY: all
all : $(TARGET)

$(TARGET) : 
	$(CC) $(CFLAGS) $(CINCLUDES) src/main.cpp $(CLIBS) -o bin/HokuyoRequester
	
.PHONY: clean
clean : 
	/bin/rm -rf tmp/* lib/* bin/*
