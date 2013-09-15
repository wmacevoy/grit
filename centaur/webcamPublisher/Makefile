CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

all:
	g++ $(CFLAGS) -c main_webcamPublisher.cpp -o webcamPublisher.o
	g++ webcamPublisher.o $(LIBS) -o webcamPublisher
