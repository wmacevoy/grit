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
	private: double dir_speed, mov_speed;
	private: int destination;
	private: int direction;
	private: volatile bool running;
	private: std::thread thread;
	private: double cutoff;
	
	private: void Run();
	public: Foot(int potpin, int enablepin, int dlpin, int drpin, int plpin, int prpin);
	public: void setDirSpeed(int _speed);
	public: void setMovSpeed(int _speed);
	public: void setDirection(boolean _direction);
	public: void setDestination(double _destination);
	public: ~Foot();	
	}
