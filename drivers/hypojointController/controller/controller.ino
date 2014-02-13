// Motor Controller
// this is the code for the individual joints..... the master is located on the main body
// and will have a different code uploaded to it.
//
//
//
//
//


#include <Wire.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"
//#include "default.hpp"

//These 2 will be deprecated soon
const int cutoff=4;
const int maxbuffer=5;
char serialInput[maxbuffer];
/////////////////////////////////

const int totalBytes = 21;

//Items to be read from eeprom in setup()
byte id;
int  potPin;
int  dirPin;
int  stepPin;
int  ledPin;
int  minFrequency;
long maxFrequency;
int  minPosition;
long maxPosition;

const int byteBuffer = 6;
byte bytes[byteBuffer];

int  i        = 0;
int  position = 0;
int  dir      = 0;          // 1 counter clockwise/0 stop/-1 clockwise
int  wait     = 100;
int  addr     = 0;          //eeprom memory address

struct Id{
 byte val;
 static const int address = 0;
 Id(){ val = 0;}
}_id;
struct PotPin{
 int val;
 static const int address = 1;
 PotPin(){val = A3;}
}_potPin; 
struct DirPin{
 int val;
 static const int address = 3;
 DirPin(){val = 8;}
}_dirPin;
struct StepPin{
 int val;
 static const int address = 5;
 StepPin(){val = 9;}
}_stepPin;
struct LedPin{
 int val;
 static const int address = 7;
 LedPin(){val = 13;}
}_ledPin;
struct MinFreq{
 int val;
 static const int address = 9; 
 MinFreq(){val = 0;}
}_minFreq;
struct MaxFreq{
 long val;
 static const int address = 11;
 MaxFreq(){val = 3500;}
}_maxFreq;
struct MinPos{
 int val;
 static const int address = 15;
 MinPos(){val = 0;}
}_minPos;
struct MaxPos{
 long val;
 static const int address = 17;
 MaxPos(){val = 1024;}
}_maxPos;

struct Response{
  int pos;
  int temp;
  int checksum;
} response;

struct Step{
 int freq;
 int goal;
 int checksum;
} step;

bool checksum();
void configure();

void setup()
{
   Serial.begin(9600);
   delay(1000);
   
   if(!checksum()) {
    Serial.print("Checksum no matchy...\nDefaulting...\n");
    defaultConfigure();    
   }
   
   addr+=EEPROM_readAnything(addr, id);             //Read ID
   addr+=EEPROM_readAnything(addr, potPin);         //Read potPin
   addr+=EEPROM_readAnything(addr, dirPin);         //Read dirPin
   addr+=EEPROM_readAnything(addr, stepPin);        //Read stepPin
   addr+=EEPROM_readAnything(addr, ledPin);         //Read stepPin
   addr+=EEPROM_readAnything(addr, minFrequency);   //Read minFrequency
   addr+=EEPROM_readAnything(addr, maxFrequency);   //Read maxFrequency
   addr+=EEPROM_readAnything(addr, minPosition);    //Read minPosition
   addr+=EEPROM_readAnything(addr, maxPosition);    //Read maxPosition
     
   Wire.begin(id);                                  // join i2c bus with address read from above
   Wire.onRequest(requestEvent);
   pinMode(dirPin,OUTPUT);
   pinMode(stepPin,OUTPUT);
   pinMode(ledPin, OUTPUT);
   digitalWrite(dirPin,LOW);
   digitalWrite(stepPin,LOW);
   
   step.freq = maxFrequency;
}


void loop()
{  
   //Serial will be used when a config manager connects to the board
   if (Serial.available() > 0) {
    Serial.readBytes(serialInput,maxbuffer);
    //Will be deprecated, for testing only
    step.goal=0;
    for(int i=0;i<maxbuffer-1;i++)
    {
      step.goal*=10;
      step.goal += serialInput[i]-'0';//Serial.print(goal);Serial.print(" ");delay(500);
    }
    if(step.goal > maxPosition) {
      step.goal = maxPosition;
    }
    
    Serial.print(step.goal);
    Serial.print('\n');
    //End testing
   }
  
   //Read six incoming bytes, 
   /*if(Wire.available() % byteBuffer == 0) {
    for(int i = 0; i < byteBuffer; ++i){
     bytes[i] = Wire.read();
    }
    //Check validity of packet
    //if(valid) {
    memcpy(&step, bytes, byteBuffer); 
   }*/
   
   if (step.goal < 10) step.goal=10;
   if (step.goal > 1010) step.goal=1010;
   
   position = analogRead(potPin);
   
   if (dir == 0 && abs(step.goal-position) <= cutoff) {
     return;
   }
   if (position < step.goal) {
      if (dir != 1) {
        digitalWrite(dirPin,0); 
      }
      dir = 1;
      int f;
      int d=step.goal-position;
      if (d > 100) {
        f=step.freq;
      } else {
        f=map(d,0,100,0,step.freq);
      }
      tone(stepPin,f);
   } else if (position > step.goal) {
      if (dir != -1) {
        digitalWrite(dirPin,1);
      }
      int f;
      int d=position-step.goal;
      if (d > 100) {
        f=step.freq;
      } else {
        f=map(d,0,100,0,step.freq);
      }
      tone(stepPin,f);
      dir = -1;
   } else {
     if (dir != 0) {
       noTone(stepPin);
     }
     dir = 0;
   }
}

void requestEvent()
{
  Wire.write("Address "); // respond with message of 6 bytes
                          // as expected by master
}

bool checksum(){
  byte b = 0, c;
  for(int i = 0; i < totalBytes; ++i){
   b ^= EEPROM.read(i);
  }
  Serial.print("Checksum is: ");
  Serial.print(b);
  Serial.print('\n');
  
  c = EEPROM.read(totalBytes);
  if(b == c){
    return true;
  } else {
    return false;
  }
}

void defaultConfigure(){
  Serial.print("Writing default values to memory...\n");
  digitalWrite(_ledPin.val, HIGH);
  
  //Write device id
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_id.address, _id.val);
  Serial.print("   Value is: ");
  Serial.print(_id.val);
  Serial.print('\n'); 
  
  //Write potentiometer pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_potPin.address, _potPin.val);
  Serial.print("   Value is: ");
  Serial.print(_potPin.val);
  Serial.print('\n');

  //Write direction pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_dirPin.address, _dirPin.val);
  Serial.print("   Value is: ");
  Serial.print(_dirPin.val);
  Serial.print('\n');
  
  //Write stepper motor pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_stepPin.address, _stepPin.val);
  Serial.print("   Value is: ");
  Serial.print(_stepPin.val);
  Serial.print('\n');
  
  //Write led pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_ledPin.address, _ledPin.val);
  Serial.print("   Value is: ");
  Serial.print(_ledPin.val);
  Serial.print('\n');
  
  //Write minFrequency
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_minFreq.address, _minFreq.val);
  Serial.print("   Value is: ");
  Serial.print(_minFreq.val);
  Serial.print('\n');
  
  //Write maxFrequency
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_maxFreq.address, _maxFreq.val);
  Serial.print("   Value is: ");
  Serial.print(_maxFreq.val);
  Serial.print('\n');
  
  //Write minPosition
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_minPos.address, _minPos.val);
  Serial.print("   Value is: ");
  Serial.print(_minPos.val);
  Serial.print('\n');
  
  //Write maxPosition
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_maxPos.address, _maxPos.val);
  Serial.print("   Value is: ");
  Serial.print(_maxPos.val);
  Serial.print('\n');

  //Return address to 0
  addr = 0;
  
  byte b = 0;
  for(int i = 0; i < totalBytes; ++i){
   b ^= EEPROM.read(i);
  }
  Serial.print("Checksum is: ");
  Serial.print(b);
  Serial.print('\n');
  EEPROM.write(totalBytes, b);
  
  Serial.print("Done writing values to memory");
  delay(1000);
  digitalWrite(_ledPin.val, LOW);
}











