#include "GPIO.h"
#include "AnalogIn.h"
#include "Tone.h"
#include <atomic>

class Foot
	{
	private: AnalogIn pot;
	private: DigitalOut enable;
	private: DigitalOut direction_l;
	private: DigitalOut direction_r;
	private: Tone pulse_l;
	private: Tone pulse_r;
	private: double cutoff;
	private: double error;
	private: double pos;
	private: double tmp_destination;
	private: std::atomic<double> spd_rotate, spd_move;
	private: std::atomic<double> destination;
	private: std::atomic<int> direction;
	public:  std::atomic<bool> running;
	private: std::thread thread;
	
	private: void Run();
	private: void move(int speed_l, int speed_r, int dir, bool rotate);
	public: Foot(int potpin, int enablepin, int dlpin, int drpin, int plpin, int prpin);
	public: void setDirSpeed(int _speed);
	public: void setMovSpeed(int _speed);
	public: void setDirection(int _direction);
	public: void setDestination(double _destination);
	public: bool isRunning();
	public: ~Foot();	
	};
