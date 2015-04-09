#include "GPIO.h"
#include "AnalogIn.h"
#include "Tone.h"

class Foot
	{
	private: AnalogIn pot;
	private: DigitalOut enable;
	private: DigitalOut direction_l;
	private: DigitalOut direction_r;
	private: Tone pulse_l;
	private: Tone pulse_l;
	private: int cutoff;
	private: double speed;
	private: int destination;
	private: int direction;
	private: volatile bool running;
	private: std::thread thread;
	
	private: void Run();
	public: Foot();
	public: void setSpeed(int _speed);
	public: void setDirection(boolean _direction);
	public: void setDestination(int _destination);
	public: void rotate(int _destination, double _speed);
	public: void drive(int _speed, boolean _direction)
	public: ~Foot();	
	}
