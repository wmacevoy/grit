CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` -lzmq -lrt

all:
	g++ $(CFLAGS) -c src/main_webcamRequester.cpp -o webcamRequester.o
	g++ webcamRequester.o $(LIBS) -o webcamRequester
