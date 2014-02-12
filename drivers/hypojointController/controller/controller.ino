// this is the code for the individual joints..... the master is located on the main body
// and will have a different code uploaded to it.

//////////////////////////////input/////////////////////////////

//input is 3 characters    026=26    100=100 001=1


#include <Wire.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"

//These 2 will be deprecated soon
const int cutoff=4;
const int maxbuffer=4;
/////////////////////////////////


//Default values are set but till be read from eeprom in setup()
byte id;
int potPin;
int dirPin;
int stepPin;
int minFrequency;
long maxFrequency;
int minPosition;
long maxPosition;

char input[maxbuffer];
int goal;
int position=0;
int dir;          // 1 counter clockwise/0 stop/-1 clockwise
int wait=100;

int addr=0;       //eeprom memory address

void setup()
{
   Serial.begin(9600);
   
   addr+=EEPROM_readAnything(addr, id); //Read ID
   addr+=EEPROM_readAnything(addr, potPin); //Read potPin
   addr+=EEPROM_readAnything(addr, dirPin); //Read dirPin
   addr+=EEPROM_readAnything(addr, stepPin); //Read stepPin
   addr+=EEPROM_readAnything(addr, minFrequency); //Read minFrequency
   addr+=EEPROM_readAnything(addr, maxFrequency); //Read maxFrequency
   addr+=EEPROM_readAnything(addr, minPosition); //Read minPosition
   addr+=EEPROM_readAnything(addr, maxPosition); //Read maxPosition
     
   Wire.begin(id);                // join i2c bus with address read from above
   Wire.onRequest(requestEvent);
   pinMode(dirPin,OUTPUT);
   pinMode(stepPin,OUTPUT);
   digitalWrite(dirPin,LOW);
   digitalWrite(stepPin,LOW);
}


void loop()
{  
  //this code is only used to test that the motor will go to designated position
   if (Serial.available() > 0) {
    Serial.readBytes(input,maxbuffer);//need to check to see if this is in degrees or steps
    goal=0;
    for(int i=0;i<maxbuffer-1;i++)
    {
      goal*=10;
      goal += input[i]-'0';//Serial.print(goal);Serial.print(" ");delay(500);
    }
  }
  /*if(Wire.available()){
    goal = Wire.read();
  }*/
   position = map(analogRead(potPin),0,1023,0,199);
   
   if (goal < 10) goal=10;
   if (goal > 190) goal=190;
   digitalWrite(stepPin,LOW);
   if (dir == 0 && abs(goal-position) <= cutoff) {
     return;
   }
   if (position < goal) {
      if (dir != 1) {
        digitalWrite(dirPin,0); 
      }
      dir = 1;
      int f;
      int d=goal-position;
      if (d > 100) {
        f=maxFrequency;
      } else {
        f=map(d,0,100,0,maxFrequency);
      }
      tone(stepPin,f);
   } else if (position > goal) {
      if (dir != -1) {
        digitalWrite(dirPin,1);
      }
      int f;
      int d=position-goal;
      if (d > 100) {
        f=maxFrequency;
      } else {
        f=map(d,0,100,0,maxFrequency);
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











