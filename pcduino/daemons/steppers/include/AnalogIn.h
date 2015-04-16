#pragma once

class AnalogIn
{
 public: static const char * const PATH; 
 public: const int pin;
 public: const int maximum;
 private: int fd;
 public: AnalogIn(int _pin);
 public: int value() const;
 public: ~AnalogIn();
};