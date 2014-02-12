// this is the code for the individual joints..... the master is located on the main body
// and will have a different code uploaded to it.

//////////////////////////////input/////////////////////////////

//input is 3 characters    026=26    100=100 001=1


#include <Wire.h>
#include <EEPROM.h>

const int potPin=A3;
const int dirPin=8;
const int stepPin=9;
const int cutoff=4;
const int maxbuffer=4;
const int maxFrequency = 3500;

char input[maxbuffer];
int goal;
int position=0;
int dir;// 1 counter clockwise/0 stop/-1 clockwise
int addy=2; 
int wait=100;
int address=1;//change this address to the address you wish to asign to it prior to upload

void setup()
{
   Serial.begin(9600);
   addy=EEPROM.read(address);// will need to set the eeprom address
   Wire.begin(addy);                // join i2c bus with address read from above
   Wire.onRequest(requestEvent);
   pinMode(dirPin,OUTPUT);
   pinMode(stepPin,OUTPUT);
   digitalWrite(dirPin,LOW);
   digitalWrite(stepPin,LOW);
   t=millis();
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











