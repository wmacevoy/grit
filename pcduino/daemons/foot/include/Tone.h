#pragma once

#include <thread>
#include "GPIO.h"

class Tone
{
 private: GPIO io;
 private: volatile unsigned delay;
 private: volatile bool running;
 private: std::thread thread;

 private: void run();
 public: Tone(int pin, double frequency = 0.0);
 public: void value(double frequency);
 public: ~Tone();
};
