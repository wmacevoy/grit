#pragma once

class GPIO
{
 public: enum { 
    INPUT='0',
    OUTPUT='1',
    INPUT_PULLUP='8',
    HIGH='1', 
    LOW='0' };

 public: static const char * const MODE_PATH;
 public: static const char * const PIN_PATH;

 public: const int pin;
 private: int fdMode;
 private: int fdPin;
  
 private: void write(int fd, char value);
 private: int read(int fd) const;
 private: void init();

 public: GPIO(int _pin);
 public: GPIO(int _pin, char _mode);
 public: inline void mode(int value)   { write(fdMode,value); }
 public: inline int mode() const       { return read(fdMode); }
 public: inline void modeInput()       { write(fdMode,INPUT);  }
 public: inline void modeInputPullup() { write(fdMode,INPUT_PULLUP);  }
 public: inline void modeOutput()      { write(fdMode,OUTPUT); }
 public: inline void value(bool truth) { write(fdPin,truth ? HIGH : LOW); }
 public: inline bool value() const     { return read(fdPin) == HIGH; }
 public: ~GPIO();
};

class DigitalIn : public GPIO
{
 public: DigitalIn(int pin);
};

class DigitalInPullup : public GPIO
{
 public: DigitalInPullup(int pin);
};

class DigitalOut : public GPIO
{
 public: DigitalOut(int pin);
};
