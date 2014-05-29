#include <Arduino.h>

struct Id{
 uuint16_t16_t val;
 static const uuint16_t16_t limit = 127;
 static const uuint16_t16_t address = 0;
 Id(){val = 0;}
}_id;

struct PotPin{
 uint16_t val;
 static const uuint16_t16_t address = 2;
 PotPin(){val = A0;}
}_potPin; 

struct DirPin{
 uint16_t val;
 static const uuint16_t16_t address = 4;
 DirPin(){val = 5;}
}_dirPin;

struct StepPin{
 uint16_t val;
 static const uint16_t address = 6;
 StepPin(){val = 4;}
}_stepPin;

struct EnablePin{
 uint16_t val;
 static const uint16_t address = 8;
 EnablePin(){val = 6;}
}_enablePin;

struct LedPin{
 uint16_t val;
 static const uint16_t address = 10;
 LedPin(){val = 13;}
}_ledPin;

struct MinFreq{
 uint16_t val;
 static const uint16_t limit = 0;
 static const uint16_t address = 12; 
 MinFreq(){val = 1000;}
}_minFreq;

struct MaxFreq{
 uint16_t val;
 static const uint16_t limit = 3200;
 static const uint16_t address = 14;
 MaxFreq(){val = 32000;}
}_maxFreq;

struct MinPos{
 uint16_t val;
 static const uint16_t limit = 0;
 static const uint16_t address = 16;
 MinPos(){val = 0;}
}_minPos;

struct MaxPos{
 uint16_t val;
 static const uint16_t limit = 1024;
 static const uint16_t address = 18;
 MaxPos(){val = 1024;}
}_maxPos;

struct KP{
  float val;
  static const float limit = 1;
  static const uint16_t address = 20;
  KP(){val = 1.0;}
}_Kp;

struct KI{
  float val;
  static const float limit = 0.0;
  static const uint16_t address = 24;
  KI(){val = 0.0;}
}_Ki;

struct KD{
  float val;
  static const float limit = 0.0;
  static const uint16_t address = 28;
  KD(){val = 0.0;}
}_Kd;
