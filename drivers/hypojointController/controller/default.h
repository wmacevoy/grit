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
 PotPin(){val = A3;}
}_potPin; 

struct LedPin{
 int val;
 static const int address = 3;
 LedPin(){val = 13;}
}_ledPin;

struct MinPos{
 int val;
 static const int limit = 0;
 static const int address = 5;
 MinPos(){val = 0;}
}_minPos;

struct MaxPos{
 long val;
 static const long limit = 1024;
 static const int address = 7;
 MaxPos(){val = 1024;}
}_maxPos;

struct numSteps{
 int val;
 static const int address = 11;
 numSteps(){val = 200;}
}_numSteps;

struct KP{
  float val;
  static const float limit = 1;
  static const int address = 15;
  KP(){val = 1.0;}
}_Kp;

struct KI{
  float val;
  static const float limit = 0.0;
  static const int address = 19;
  KI(){val = 0.0;}
}_Ki;

struct KD{
  float val;
  static const float limit = 0.0;
  static const int address = 23;
  KD(){val = 0.0;}
}_Kd;
