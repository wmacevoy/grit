#include <Arduino.h>

struct Id{
 byte val;
 static const byte limit = 255;
 static const int address = 0;
 Id(){val = 0;}
}_id;

struct PotPin{
 int val;
 static const int address = 1;
 PotPin(){val = A0;}
}_potPin; 

struct DirPin{
 int val;
 static const int address = 3;
 DirPin(){val = 5;}
}_dirPin;

struct StepPin{
 int val;
 static const int address = 5;
 StepPin(){val = 4;}
}_stepPin;

struct EnablePin{
 int val;
 static const int address = 7;
 EnablePin(){val = 6;}
}_enablePin;

struct LedPin{
 int val;
 static const int address = 9;
 LedPin(){val = 13;}
}_ledPin;

struct MinFreq{
 int val;
 static const int limit = 0;
 static const int address = 11; 
 MinFreq(){val = 0;}
}_minFreq;

struct MaxFreq{
 long val;
 static const long limit = 3200;
 static const int address = 13;
 MaxFreq(){val = 32000;}
}_maxFreq;

struct MinPos{
 int val;
 static const int limit = 0;
 static const int address = 17;
 MinPos(){val = 0;}
}_minPos;

struct MaxPos{
 long val;
 static const long limit = 1024;
 static const int address = 19;
 MaxPos(){val = 1024;}
}_maxPos;

struct KP{
  float val;
  static const float limit = 1;
  static const int address = 23;
  KP(){val = 1.0;}
}_Kp;

struct KI{
  float val;
  static const float limit = 0.0;
  static const int address = 27;
  KI(){val = 0.0;}
}_Ki;

struct KD{
  float val;
  static const float limit = 0.0;
  static const int address = 31;
  KD(){val = 0.0;}
}_Kd;
