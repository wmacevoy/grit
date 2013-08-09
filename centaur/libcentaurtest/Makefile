CFLAGS=-O2 -fPIC -std=c++0x
CINCLUDES= -I../../libs/bzip2/include -I../../libs/bson/include -I../../libs/cppunit/include -I../../libs/zmq/include -I../libcentaur/include -I../../drivers/utilities/include -I../../libs/bson/include -Iinclude -Isrc
CLIBS= -ldl -lutilities -lcentaur -lbson -lpthread -lboost_thread -lzmq -lSDL -lcppunit2 -lrt -lbz2 -L../../libs/cppunit/lib -L../../libs/zmq/lib -L../../libs/bson/lib -L../../libs/bzip2/lib -L../libcentaur/lib -L../../drivers/utilities/lib -L../../libs/lib
CC=g++ 

TARGET=bin/libcentaurtest

.PHONY: all
all : $(TARGET)

$(TARGET) : 
	$(CC) $(CFLAGS) $(CINCLUDES) src/main.cpp src/CentaurMessageTestCase.cpp src/CentaurTypesTestCase.cpp src/ExampleTestCase.cpp $(CLIBS) -o bin/libcentaurtest
	
.PHONY: clean
clean : 
	/bin/rm -rf tmp/* lib/* bin/*

