#include <Arduino.h>
//0
struct Id{
 uint16_t val;
 static const uint16_t limit = 127;
 static const uint16_t address = 0;
 Id(){val = 4;}
}_id;
//2
struct PotPin{
 uint16_t val;
 static const uint16_t address = 2;
 PotPin(){val = A0;}
}_potPin; 
//4
struct DirPin{
 uint16_t val;
 static const uint16_t address = 4;
 DirPin(){val = 5;}
}_dirPin;
//6
struct StepPin{
 uint16_t val;
 static const uint16_t address = 6;
 StepPin(){val = 4;}
}_stepPin;
//8
struct EnablePin{
 uint16_t val;
 static const uint16_t address = 8;
 EnablePin(){val = 6;}
}_enablePin;
//10
struct LedPin{
 uint16_t val;
 static const uint16_t address = 10;
 LedPin(){val = 13;}
}_ledPin;
//12
struct MinFreq{
 uint16_t val;
 static const uint16_t limit = 0;
 static const uint16_t address = 12; 
 MinFreq(){val = 0;}
}_minFreq;
//14
struct MaxFreq{
 uint16_t val;
 static const uint16_t limit = 31000;
 static const uint16_t address = 14;
 MaxFreq(){val = 31000;}
}_maxFreq;
//16
struct MinPos{
 uint16_t val;
 static const uint16_t limit = 0;
 static const uint16_t address = 16;
 MinPos(){val = 0;}
}_minPos;
//18
struct MaxPos{
 uint16_t val;
 static const uint16_t limit = 1024;
 static const uint16_t address = 18;
 MaxPos(){val = 1024;}
}_maxPos;
//20
struct MaxTemp{
 uint16_t val;
 static const uint16_t limit = 85;
 static const uint16_t address = 20;
 MaxTemp(){val = 85;}
}_maxTemp;
//22
struct KP{
  float val;
  static const float limit = 2.0;
  static const uint16_t address = 22;
  KP(){val = 2.0;}
}_Kp;
//26
struct KI{
  float val;
  static const float limit = 0.0;
  static const uint16_t address = 26;
  KI(){val = 0.0;}
}_Ki;
//30
struct KD{
  float val;
  static const float limit = 0.0;
  static const uint16_t address = 30;
  KD(){val = 0.0;}
}_Kd;
