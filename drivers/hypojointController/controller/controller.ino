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

//These 2 will be deprecated soon
const int cutoff=4;
const int maxbuffer=5;
char input[maxbuffer];
/////////////////////////////////


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
int  addr     = 0;       //eeprom memory address

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

void setup()
{
   Serial.begin(9600);
   
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
   digitalWrite(dirPin,LOW);
   digitalWrite(stepPin,LOW);
   
   step.freq = maxFrequency;
}


void loop()
{  
  //this code is only used to test that the motor will go to designated position
   if (Serial.available() > 0) {
    Serial.readBytes(input,maxbuffer);
    step.goal=0;
    for(int i=0;i<maxbuffer-1;i++)
    {
      step.goal*=10;
      step.goal += input[i]-'0';//Serial.print(goal);Serial.print(" ");delay(500);
    }
    if(step.goal > maxPosition) {
      step.goal = maxPosition;
    }
    
    Serial.print(step.goal);
    Serial.print('\n');
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











