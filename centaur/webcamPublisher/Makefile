CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` -lzmq -lrt

all:
	g++ $(CFLAGS) -c src/main_webcamPublisher.cpp -o webcamPublisher.o
	g++ webcamPublisher.o $(LIBS) -o webcamPublisher
