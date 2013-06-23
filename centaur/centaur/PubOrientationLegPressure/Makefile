TARGET=PubOrientationLegPressure
	
bin/$(TARGET):tmp/$(TARGET).o 
	g++ -o bin/$(TARGET)  $^ -lzmq
	
tmp/$(TARGET).o:src/$(TARGET).cpp
	g++ -c -I../../libcentaur/include $< -o tmp/$(TARGET).o
	
clean:
	rm tmp/*.o
	rm bin/$(TARGET)
